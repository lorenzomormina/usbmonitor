cmake_minimum_required(VERSION 3.27)
project(MyProject)

add_executable(main "main.c")

find_package(PkgConfig REQUIRED)
pkg_check_modules(libusb REQUIRED IMPORTED_TARGET libusb-1.0)
target_link_libraries(main PRIVATE PkgConfig::libusb)