cmake_minimum_required(VERSION 3.12)
project(filesystem C)

set(CMAKE_C_STANDARD 11)

include_directories(FileSystemProject)

add_executable(filesystem
        FileSystemProject/filesystem.h
        FileSystemProject/main.c
        FileSystemProject/softwaredisk.c
        FileSystemProject/softwaredisk.h)
