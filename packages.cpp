#include "packages.hpp"
#include <sstream>
#include <sys/utsname.h>
#include <alpm.h>

std::string pacman_version();

std::string make_pacman_user_agent() {
	std::ostringstream os;
	utsname un{};
	uname(&un);
	os << "pacman/" << pacman_version() << " (" << un.sysname << ' ' << un.machine
	   << ") libalpm/" << alpm_version();
	return os.str();
}

std::string pacman_version() {
	alpm_errno_t err;
	auto* handle = alpm_initialize("/", "/var/lib/pacman/", &err);
	if (handle == NULL) {
		throw std::runtime_error(alpm_strerror(err));
	}
	auto* db = alpm_get_localdb(handle);
	auto* pacman = alpm_db_get_pkg(db, "pacman");
	std::string version = alpm_pkg_get_version(pacman);
	alpm_release(handle);
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
