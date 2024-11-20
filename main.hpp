#pragma once
#include <string>
#include <filesystem>

namespace curl {
	#include <curl/curl.h>
}

struct Args {
	std::string proxy;
	std::string url;
	std::filesystem::path file;
};

// Parse arguments
Args parse_args(int argc, const char** argv);

// Download the file
[[nodiscard]] int invoke_curl(const Args& args, const std::string& user_agent, curl::curl_off_t resume) noexcept;

// Get the resume position. Returns 0 if the file does not exist.
long get_resume(const std::filesystem::path& filepath);

// Get the pacman user agent
std::string make_pacman_user_agent();
