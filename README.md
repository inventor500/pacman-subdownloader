# Description

This is a simple utility for Pacman. Pacman allows for an external downloader, and the Arch wiki recommends this if using a proxy. 

Curl is an excellent choice for proxying in this way, but has two problems:
1. It does not set the user agent correctly (without user-agent being passed in the config file)
2. It does not print the name of what is being downloaded

This simple program aims to solve both problems by building the user agent string the same way Pacman does, printing the package download URL before downloading, and then invoking curl to perform the download.

# Usage

In `/etc/pacman.conf`, set `XferCommand` like so:
```conf
XferCommand = /path/to/pacman-subdownloader --socks5-hostname <proxy> %u %o
```
