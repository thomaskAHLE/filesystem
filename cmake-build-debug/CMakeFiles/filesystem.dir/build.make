# CMAKE generated file: DO NOT EDIT!
# Generated by "Unix Makefiles" Generator, CMake Version 3.12

# Delete rule output on recipe failure.
.DELETE_ON_ERROR:


#=============================================================================
# Special targets provided by cmake.

# Disable implicit rules so canonical targets will work.
.SUFFIXES:


# Remove some rules from gmake that .SUFFIXES does not remove.
SUFFIXES =

.SUFFIXES: .hpux_make_needs_suffix_list


# Suppress display of executed commands.
$(VERBOSE).SILENT:


# A target that is always out of date.
cmake_force:

.PHONY : cmake_force

#=============================================================================
# Set environment variables for the build.

# The shell in which to execute make rules.
SHELL = /bin/sh

# The CMake executable.
CMAKE_COMMAND = /Applications/CLion.app/Contents/bin/cmake/mac/bin/cmake

# The command to remove a file.
RM = /Applications/CLion.app/Contents/bin/cmake/mac/bin/cmake -E remove -f

# Escaping for special characters.
EQUALS = =

# The top-level source directory on which CMake was run.
CMAKE_SOURCE_DIR = "/Users/GG46195x/Documents/School/Operating Systems/filesystem/filesystem"

# The top-level build directory on which CMake was run.
CMAKE_BINARY_DIR = "/Users/GG46195x/Documents/School/Operating Systems/filesystem/filesystem/cmake-build-debug"

# Include any dependencies generated for this target.
include CMakeFiles/filesystem.dir/depend.make

# Include the progress variables for this target.
include CMakeFiles/filesystem.dir/progress.make

# Include the compile flags for this target's objects.
include CMakeFiles/filesystem.dir/flags.make

CMakeFiles/filesystem.dir/FileSystemProject/main.c.o: CMakeFiles/filesystem.dir/flags.make
CMakeFiles/filesystem.dir/FileSystemProject/main.c.o: ../FileSystemProject/main.c
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --progress-dir="/Users/GG46195x/Documents/School/Operating Systems/filesystem/filesystem/cmake-build-debug/CMakeFiles" --progress-num=$(CMAKE_PROGRESS_1) "Building C object CMakeFiles/filesystem.dir/FileSystemProject/main.c.o"
	/Library/Developer/CommandLineTools/usr/bin/cc $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -o CMakeFiles/filesystem.dir/FileSystemProject/main.c.o   -c "/Users/GG46195x/Documents/School/Operating Systems/filesystem/filesystem/FileSystemProject/main.c"

CMakeFiles/filesystem.dir/FileSystemProject/main.c.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing C source to CMakeFiles/filesystem.dir/FileSystemProject/main.c.i"
	/Library/Developer/CommandLineTools/usr/bin/cc $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -E "/Users/GG46195x/Documents/School/Operating Systems/filesystem/filesystem/FileSystemProject/main.c" > CMakeFiles/filesystem.dir/FileSystemProject/main.c.i

CMakeFiles/filesystem.dir/FileSystemProject/main.c.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling C source to assembly CMakeFiles/filesystem.dir/FileSystemProject/main.c.s"
	/Library/Developer/CommandLineTools/usr/bin/cc $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -S "/Users/GG46195x/Documents/School/Operating Systems/filesystem/filesystem/FileSystemProject/main.c" -o CMakeFiles/filesystem.dir/FileSystemProject/main.c.s

CMakeFiles/filesystem.dir/FileSystemProject/softwaredisk.c.o: CMakeFiles/filesystem.dir/flags.make
CMakeFiles/filesystem.dir/FileSystemProject/softwaredisk.c.o: ../FileSystemProject/softwaredisk.c
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --progress-dir="/Users/GG46195x/Documents/School/Operating Systems/filesystem/filesystem/cmake-build-debug/CMakeFiles" --progress-num=$(CMAKE_PROGRESS_2) "Building C object CMakeFiles/filesystem.dir/FileSystemProject/softwaredisk.c.o"
	/Library/Developer/CommandLineTools/usr/bin/cc $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -o CMakeFiles/filesystem.dir/FileSystemProject/softwaredisk.c.o   -c "/Users/GG46195x/Documents/School/Operating Systems/filesystem/filesystem/FileSystemProject/softwaredisk.c"

CMakeFiles/filesystem.dir/FileSystemProject/softwaredisk.c.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing C source to CMakeFiles/filesystem.dir/FileSystemProject/softwaredisk.c.i"
	/Library/Developer/CommandLineTools/usr/bin/cc $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -E "/Users/GG46195x/Documents/School/Operating Systems/filesystem/filesystem/FileSystemProject/softwaredisk.c" > CMakeFiles/filesystem.dir/FileSystemProject/softwaredisk.c.i

CMakeFiles/filesystem.dir/FileSystemProject/softwaredisk.c.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling C source to assembly CMakeFiles/filesystem.dir/FileSystemProject/softwaredisk.c.s"
	/Library/Developer/CommandLineTools/usr/bin/cc $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -S "/Users/GG46195x/Documents/School/Operating Systems/filesystem/filesystem/FileSystemProject/softwaredisk.c" -o CMakeFiles/filesystem.dir/FileSystemProject/softwaredisk.c.s

# Object files for target filesystem
filesystem_OBJECTS = \
"CMakeFiles/filesystem.dir/FileSystemProject/main.c.o" \
"CMakeFiles/filesystem.dir/FileSystemProject/softwaredisk.c.o"

# External object files for target filesystem
filesystem_EXTERNAL_OBJECTS =

filesystem: CMakeFiles/filesystem.dir/FileSystemProject/main.c.o
filesystem: CMakeFiles/filesystem.dir/FileSystemProject/softwaredisk.c.o
filesystem: CMakeFiles/filesystem.dir/build.make
filesystem: CMakeFiles/filesystem.dir/link.txt
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --bold --progress-dir="/Users/GG46195x/Documents/School/Operating Systems/filesystem/filesystem/cmake-build-debug/CMakeFiles" --progress-num=$(CMAKE_PROGRESS_3) "Linking C executable filesystem"
	$(CMAKE_COMMAND) -E cmake_link_script CMakeFiles/filesystem.dir/link.txt --verbose=$(VERBOSE)

# Rule to build all files generated by this target.
CMakeFiles/filesystem.dir/build: filesystem

.PHONY : CMakeFiles/filesystem.dir/build

CMakeFiles/filesystem.dir/clean:
	$(CMAKE_COMMAND) -P CMakeFiles/filesystem.dir/cmake_clean.cmake
.PHONY : CMakeFiles/filesystem.dir/clean

CMakeFiles/filesystem.dir/depend:
	cd "/Users/GG46195x/Documents/School/Operating Systems/filesystem/filesystem/cmake-build-debug" && $(CMAKE_COMMAND) -E cmake_depends "Unix Makefiles" "/Users/GG46195x/Documents/School/Operating Systems/filesystem/filesystem" "/Users/GG46195x/Documents/School/Operating Systems/filesystem/filesystem" "/Users/GG46195x/Documents/School/Operating Systems/filesystem/filesystem/cmake-build-debug" "/Users/GG46195x/Documents/School/Operating Systems/filesystem/filesystem/cmake-build-debug" "/Users/GG46195x/Documents/School/Operating Systems/filesystem/filesystem/cmake-build-debug/CMakeFiles/filesystem.dir/DependInfo.cmake" --color=$(COLOR)
.PHONY : CMakeFiles/filesystem.dir/depend
