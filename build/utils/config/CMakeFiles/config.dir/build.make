# CMAKE generated file: DO NOT EDIT!
# Generated by "Unix Makefiles" Generator, CMake Version 3.26

# Delete rule output on recipe failure.
.DELETE_ON_ERROR:

#=============================================================================
# Special targets provided by cmake.

# Disable implicit rules so canonical targets will work.
.SUFFIXES:

# Disable VCS-based implicit rules.
% : %,v

# Disable VCS-based implicit rules.
% : RCS/%

# Disable VCS-based implicit rules.
% : RCS/%,v

# Disable VCS-based implicit rules.
% : SCCS/s.%

# Disable VCS-based implicit rules.
% : s.%

.SUFFIXES: .hpux_make_needs_suffix_list

# Command-line flag to silence nested $(MAKE).
$(VERBOSE)MAKESILENT = -s

#Suppress display of executed commands.
$(VERBOSE).SILENT:

# A target that is always out of date.
cmake_force:
.PHONY : cmake_force

#=============================================================================
# Set environment variables for the build.

# The shell in which to execute make rules.
SHELL = /bin/sh

# The CMake executable.
CMAKE_COMMAND = /usr/local/bin/cmake

# The command to remove a file.
RM = /usr/local/bin/cmake -E rm -f

# Escaping for special characters.
EQUALS = =

# The top-level source directory on which CMake was run.
CMAKE_SOURCE_DIR = /work

# The top-level build directory on which CMake was run.
CMAKE_BINARY_DIR = /work/build

# Include any dependencies generated for this target.
include utils/config/CMakeFiles/config.dir/depend.make
# Include any dependencies generated by the compiler for this target.
include utils/config/CMakeFiles/config.dir/compiler_depend.make

# Include the progress variables for this target.
include utils/config/CMakeFiles/config.dir/progress.make

# Include the compile flags for this target's objects.
include utils/config/CMakeFiles/config.dir/flags.make

utils/config/CMakeFiles/config.dir/config.cpp.o: utils/config/CMakeFiles/config.dir/flags.make
utils/config/CMakeFiles/config.dir/config.cpp.o: /work/utils/config/config.cpp
utils/config/CMakeFiles/config.dir/config.cpp.o: utils/config/CMakeFiles/config.dir/compiler_depend.ts
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --progress-dir=/work/build/CMakeFiles --progress-num=$(CMAKE_PROGRESS_1) "Building CXX object utils/config/CMakeFiles/config.dir/config.cpp.o"
	cd /work/build/utils/config && /usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -MD -MT utils/config/CMakeFiles/config.dir/config.cpp.o -MF CMakeFiles/config.dir/config.cpp.o.d -o CMakeFiles/config.dir/config.cpp.o -c /work/utils/config/config.cpp

utils/config/CMakeFiles/config.dir/config.cpp.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing CXX source to CMakeFiles/config.dir/config.cpp.i"
	cd /work/build/utils/config && /usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -E /work/utils/config/config.cpp > CMakeFiles/config.dir/config.cpp.i

utils/config/CMakeFiles/config.dir/config.cpp.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling CXX source to assembly CMakeFiles/config.dir/config.cpp.s"
	cd /work/build/utils/config && /usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -S /work/utils/config/config.cpp -o CMakeFiles/config.dir/config.cpp.s

utils/config/CMakeFiles/config.dir/config_develop.cpp.o: utils/config/CMakeFiles/config.dir/flags.make
utils/config/CMakeFiles/config.dir/config_develop.cpp.o: /work/utils/config/config_develop.cpp
utils/config/CMakeFiles/config.dir/config_develop.cpp.o: utils/config/CMakeFiles/config.dir/compiler_depend.ts
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --progress-dir=/work/build/CMakeFiles --progress-num=$(CMAKE_PROGRESS_2) "Building CXX object utils/config/CMakeFiles/config.dir/config_develop.cpp.o"
	cd /work/build/utils/config && /usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -MD -MT utils/config/CMakeFiles/config.dir/config_develop.cpp.o -MF CMakeFiles/config.dir/config_develop.cpp.o.d -o CMakeFiles/config.dir/config_develop.cpp.o -c /work/utils/config/config_develop.cpp

utils/config/CMakeFiles/config.dir/config_develop.cpp.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing CXX source to CMakeFiles/config.dir/config_develop.cpp.i"
	cd /work/build/utils/config && /usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -E /work/utils/config/config_develop.cpp > CMakeFiles/config.dir/config_develop.cpp.i

utils/config/CMakeFiles/config.dir/config_develop.cpp.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling CXX source to assembly CMakeFiles/config.dir/config_develop.cpp.s"
	cd /work/build/utils/config && /usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -S /work/utils/config/config_develop.cpp -o CMakeFiles/config.dir/config_develop.cpp.s

# Object files for target config
config_OBJECTS = \
"CMakeFiles/config.dir/config.cpp.o" \
"CMakeFiles/config.dir/config_develop.cpp.o"

# External object files for target config
config_EXTERNAL_OBJECTS =

utils/config/libconfig.a: utils/config/CMakeFiles/config.dir/config.cpp.o
utils/config/libconfig.a: utils/config/CMakeFiles/config.dir/config_develop.cpp.o
utils/config/libconfig.a: utils/config/CMakeFiles/config.dir/build.make
utils/config/libconfig.a: utils/config/CMakeFiles/config.dir/link.txt
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --bold --progress-dir=/work/build/CMakeFiles --progress-num=$(CMAKE_PROGRESS_3) "Linking CXX static library libconfig.a"
	cd /work/build/utils/config && $(CMAKE_COMMAND) -P CMakeFiles/config.dir/cmake_clean_target.cmake
	cd /work/build/utils/config && $(CMAKE_COMMAND) -E cmake_link_script CMakeFiles/config.dir/link.txt --verbose=$(VERBOSE)

# Rule to build all files generated by this target.
utils/config/CMakeFiles/config.dir/build: utils/config/libconfig.a
.PHONY : utils/config/CMakeFiles/config.dir/build

utils/config/CMakeFiles/config.dir/clean:
	cd /work/build/utils/config && $(CMAKE_COMMAND) -P CMakeFiles/config.dir/cmake_clean.cmake
.PHONY : utils/config/CMakeFiles/config.dir/clean

utils/config/CMakeFiles/config.dir/depend:
	cd /work/build && $(CMAKE_COMMAND) -E cmake_depends "Unix Makefiles" /work /work/utils/config /work/build /work/build/utils/config /work/build/utils/config/CMakeFiles/config.dir/DependInfo.cmake --color=$(COLOR)
.PHONY : utils/config/CMakeFiles/config.dir/depend

