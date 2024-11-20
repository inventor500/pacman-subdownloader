cmake_minimum_required(VERSION 3.30)
project(pacman-subdownloader LANGUAGES CXX)
set(CMAKE_CXX_STANDARD 17)

add_library(packages STATIC packages.cpp packages.hpp)
target_link_libraries(packages alpm)

add_executable(pacman-subdownloader)
target_sources(pacman-subdownloader
  PUBLIC
  ${CMAKE_SOURCE_DIR}/main.cpp
)
target_link_libraries(pacman-subdownloader packages curl)
