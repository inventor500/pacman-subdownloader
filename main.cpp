//  pacman-subdownloader - Downloads files with cURL using a proxy, with easy syntax to call from Pacman.
//       Copyright (C) 2024  Isaac Ganoung
//
//       This program is free software: you can redistribute it and/or modify it under the terms of
//       the GNU General Public License as published by the Free Software Foundation, either version 3 of the License,
//       or (at your option) any later version.
//
//       This program is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even
//       the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public License for more details.
//
//       You should have received a copy of the GNU General Public License along with this program.
//       If not, see <https://www.gnu.org/licenses/>.

#include "main.hpp"
#include <string>
#include <cstring>
#include <format>
#include <iostream>
#include <filesystem>
#include <cmath>
#include <unistd.h>

namespace sys {
	#include <sys/ioctl.h>
	#include <sys/utsname.h>
}

namespace alpm {
	#include <alpm.h>
}

int main(int argc, const char** argv) {
	try {
		Args args = parse_args(argc, argv);
		std::string user_agent = make_pacman_user_agent();
		curl::curl_off_t resume = get_resume(args.file);
		std::cerr << "Downloading " << args.url << "...\n";
		int result = invoke_curl(args, user_agent, resume);
		std::cerr << '\n';
		return result;
	} catch (const std::runtime_error&) {
		return 1;
	}
}

Args parse_args(int argc, const char** argv) {
	Args args{};
	// Get the hostname
	// This will silently ignore any unsupported flags that precede socks5-hostname
	int position = 1;
	for (; position < argc; position++) {
		if (strcmp(argv[position], "--socks5-hostname") == 0) {
			if (position < (argc - 1)) {
				args.proxy = argv[position+1];
			} else {
				std::cerr << "--socks5-hostname requires a value to be specified\n";
				throw std::runtime_error("Unable to parse arguments");
			}
			position += 2; // Skip over the hostname value
			break;
		}
	}
	if (position+2 == argc) {
		args.url = argv[position];
		args.file = argv[position+1];
	} else {
		std::cerr << std::format("Usage: {} [--socks5-hostname <hostname>:<port>] <url> <filename>\n", argv[0]);
		throw std::runtime_error("Unable to parse arguments");
	}
	return args;
}

/* Package Versions */

std::string make_pacman_user_agent() {
	sys::utsname un{};
	sys::uname(&un);
	return std::format("pacman/{} ({} {}) libalpm/{}", pacman_version(), un.sysname, un.machine, alpm::alpm_version());
}

std::string pacman_version() {
	alpm::alpm_errno_t err;
	auto* handle = alpm::alpm_initialize("/", "/var/lib/pacman/", &err);
	if (handle == NULL) {
		throw std::runtime_error(alpm::alpm_strerror(err));
	}
	auto* db = alpm::alpm_get_localdb(handle);
	auto* pacman = alpm::alpm_db_get_pkg(db, "pacman");
	std::string version = alpm::alpm_pkg_get_version(pacman);
	alpm::alpm_release(handle);
	size_t pos = 0;
	for (int i = 0; i < 3 && pos < version.size(); i++) {
		pos = version.find('.', pos);
		pos++;
	}
	if (version.at(pos-1) == '.') {
		pos--; // Omit the last '.'
	}
	return version.substr(0, pos);
}


/* Downloading Functions */

// Get the width of the console
int get_width() noexcept {
	struct sys::winsize size;
	sys::ioctl(STDOUT_FILENO, TIOCGWINSZ, &size);
	return size.ws_col;
}

// Display download progress
int display_progress(void*, curl::curl_off_t dltotal, curl::curl_off_t dlnow,
					 curl::curl_off_t, curl::curl_off_t) {
	// The last two parameters are for upload progress
	if (dltotal <= 0 || !isatty(fileno(stdin))) { // Avoid divide-by-zero
		return 0;
	}
	const int size = get_width() - 2;
	const double remaining = static_cast<double>(dlnow) / dltotal;
	const int positions = std::floor(size * remaining);
	// TODO: Maybe have '-' for the last character if size * remaining would round up?
	std::cerr << std::format("\r[{}{}]", std::string(positions, '='), std::string(size - positions, ' '));
	return 0;
}

[[nodiscard]] int invoke_curl(const Args& args, const std::string& user_agent,
							  curl::curl_off_t resume) noexcept {
	curl::CURL *hnd = curl::curl_easy_init();
	// Get errors
	char errors[CURL_ERROR_SIZE];
	errors[0] = 0;
	curl::curl_easy_setopt(hnd, curl::CURLOPT_ERRORBUFFER, errors);
	// Set the URL
	curl::curl_easy_setopt(hnd, curl::CURLOPT_URL, args.url.c_str());
	// Use a proxy if provided
	if (args.proxy != "") {
		curl::curl_easy_setopt(hnd, curl::CURLOPT_PROXY, args.proxy.c_str());
		curl::curl_easy_setopt(hnd, curl::CURLOPT_PROXYTYPE, curl::CURLPROXY_SOCKS5_HOSTNAME);
	}
	// Set the user agent
	curl::curl_easy_setopt(hnd, curl::CURLOPT_USERAGENT, user_agent.c_str());
	// Avoid writing headers to file
	curl::curl_easy_setopt(hnd, curl::CURLOPT_HEADER, false);
	// Resume downloading
	if (resume != 0) {
		curl::curl_easy_setopt(hnd, curl::CURLOPT_RESUME_FROM, resume);
	}
	// Display download progress
	curl::curl_easy_setopt(hnd, curl::CURLOPT_XFERINFOFUNCTION, display_progress);
	curl::curl_easy_setopt(hnd, curl::CURLOPT_NOPROGRESS, false);
	// Write to a file
	std::FILE* file = std::fopen(args.file.c_str(), (resume != 0) ? "a" : "w");
	curl::curl_easy_setopt(hnd, curl::CURLOPT_WRITEDATA, file);
	// Do the download
	curl::CURLcode ret = curl::curl_easy_perform(hnd);
	long http_code;
	curl::curl_easy_getinfo(hnd, curl::CURLINFO_RESPONSE_CODE, &http_code);
	// Clean up
	curl::curl_easy_cleanup(hnd);
	if (ret != curl::CURLE_OK || http_code != 200) {
		std::cerr << "Received HTTP code " << http_code << '\n';
		if (errors[0] != 0) {
			std::cerr << errors << '\n';
		}
	}
	// Check for file writing errors
	int is_ok = (std::ferror(file)) ? EXIT_FAILURE : EXIT_SUCCESS;
	std::fclose(file);
	// Return a curl failure if there is one, else return the file error
	return (ret == curl::CURLE_OK && http_code == 200) ? is_ok : http_code;
}

long get_resume(const std::filesystem::path& filename) {
	// File does not exist
	if (!std::filesystem::exists(filename)) {
		return 0;
	}
	// Number of bytes in the file
	return std::filesystem::file_size(filename);
}
