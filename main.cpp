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

#include <unistd.h>
#include <sys/ioctl.h>
#include <string>
#include <cstring>
#include <sstream>
#include <iostream>
#include <cmath>
#include <filesystem>
#include "packages.hpp"
#include "main.hpp"

int main(int argc, const char** argv) {
	try {
		Args args = parse_args(argc, argv);
		std::string user_agent = make_pacman_user_agent();
		curl_off_t resume = get_resume(args.file);
		std::cerr << "Downloading " << args.url << "...\n";
		return invoke_curl(args, user_agent, resume);
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
		std::cerr << "Usage: " << argv[0] << " [--socks5-hostname <hostname>:<port>] <url> <filename>\n";
		throw std::runtime_error("Unable to parse arguments");
	}
	return args;
}

// Get the width of the console
int get_width() noexcept {
	struct winsize size;
	ioctl(STDOUT_FILENO, TIOCGWINSZ, &size);
	return size.ws_col;
}

// Display download progress
int display_progress(void*, curl_off_t dltotal, curl_off_t dlnow,
					 curl_off_t, curl_off_t) {
	// The last two parameters are for upload progress
	if (dltotal <= 0) { // Avoid divide-by-zero
		return 0;
	}
	const int size = get_width() - 2;
	const double remaining = static_cast<double>(dlnow) / dltotal;
	const int positions = std::floor(size * remaining);
	// TODO: Maybe have '-' for the last character if size * remaining would round up?
	std::cerr << '\r' << '[' << std::string(positions, '=') << std::string(size - positions, ' ') << ']';
	return 0;
}

[[nodiscard]] int invoke_curl(const Args& args, const std::string& user_agent, curl_off_t resume) noexcept {
	CURL *hnd = curl_easy_init();
	// Get errors
	char errors[CURL_ERROR_SIZE];
	errors[0] = 0;
	curl_easy_setopt(hnd, CURLOPT_ERRORBUFFER, errors);
	// Set the URL
	curl_easy_setopt(hnd, CURLOPT_URL, args.url.c_str());
	// Use a proxy if provided
	if (args.proxy != "") {
		curl_easy_setopt(hnd, CURLOPT_PROXY, args.proxy.c_str());
		curl_easy_setopt(hnd, CURLOPT_PROXYTYPE, CURLPROXY_SOCKS5_HOSTNAME);
	}
	// Set the user agent
	curl_easy_setopt(hnd, CURLOPT_USERAGENT, user_agent.c_str());
	// Avoid writing headers to file
	curl_easy_setopt(hnd, CURLOPT_HEADER, false);
	// Resume downloading
	if (resume != 0) {
		curl_easy_setopt(hnd, CURLOPT_RESUME_FROM, resume);
	}
	// Display download progress
	curl_easy_setopt(hnd, CURLOPT_XFERINFOFUNCTION, display_progress);
	curl_easy_setopt(hnd, CURLOPT_NOPROGRESS, false);
	// Write to a file
	std::FILE* file = fopen(args.file.c_str(), (resume != 0) ? "a" : "w");
	curl_easy_setopt(hnd, CURLOPT_WRITEDATA, file);
	// Do the download
	CURLcode ret = curl_easy_perform(hnd);
	long http_code;
	curl_easy_getinfo(hnd, CURLINFO_RESPONSE_CODE, &http_code);
	// Clean up
	curl_easy_cleanup(hnd);
	if (ret != CURLE_OK || http_code != 200) {
		std::cerr << "Received HTTP code " << http_code << '\n';
		std::cerr << errors << '\n';
	}
	// Check for file writing errors
	int is_ok = (std::ferror(file)) ? EXIT_FAILURE : EXIT_SUCCESS;
	std::fclose(file);
	// Return a curl failure if there is one, else return the file error
	return (ret == CURLE_OK && http_code == 200) ? is_ok : http_code;
}

long get_resume(const std::filesystem::path& filename) {
	// File does not exist
	if (!std::filesystem::exists(filename)) {
		return 0;
	}
	// Number of bytes in the file
	return std::filesystem::file_size(filename);
}
