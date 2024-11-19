# Description

This is a simple utility for Pacman. Pacman allows for an external downloader, and the Arch wiki recommends this if using a proxy. 

cURL is an excellent choice for proxying in this way, but has two problems:
1. It does not set the user agent correctly (without user-agent being passed in the config file)
2. It does not print the name of what is being downloaded

This simple program aims to solve both problems by building the user agent string the same way Pacman does, printing the package download URL before downloading, and then invoking `curl` to perform the download.

# Usage

In `/etc/pacman.conf`, set `XferCommand` like so:
```conf
XferCommand = /path/to/pacman-subdownloader --socks5-hostname <proxy> %u %o
```

# Compilation

This program uses cgo. It requires `alpm.h` (included with pacman) and `sys/utsname.h` (part of the POSIX standard) to compile.

To compile a binary, simply run `go build`. To compile an Arch Linux package, copy the directory somewhere outside of your go workspace, and then run `makepkg`.

# Caveats

- Unfortunately, Pacman sets its version using a macro at compile-time, so there is no easy way to derive it from any other program.  
I may eventually update this to query the local package database for it, but until then, that part of the user agent is hard-coded.  
All other parts of the user agent are derived the same way as pacman calculates them.
- This program still uses cURL to download packages.

