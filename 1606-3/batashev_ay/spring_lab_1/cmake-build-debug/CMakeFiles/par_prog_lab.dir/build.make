# CMAKE generated file: DO NOT EDIT!
# Generated by "Unix Makefiles" Generator, CMake Version 3.13

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
CMAKE_COMMAND = "/Users/aleksandrbatasev/Library/Application Support/JetBrains/Toolbox/apps/CLion/ch-1/191.5849.15/CLion 2019.1 EAP.app/Contents/bin/cmake/mac/bin/cmake"

# The command to remove a file.
RM = "/Users/aleksandrbatasev/Library/Application Support/JetBrains/Toolbox/apps/CLion/ch-1/191.5849.15/CLion 2019.1 EAP.app/Contents/bin/cmake/mac/bin/cmake" -E remove -f

# Escaping for special characters.
EQUALS = =

# The top-level source directory on which CMake was run.
CMAKE_SOURCE_DIR = /Users/aleksandrbatasev/CLionProjects/par_prog_lab

# The top-level build directory on which CMake was run.
CMAKE_BINARY_DIR = /Users/aleksandrbatasev/CLionProjects/par_prog_lab/cmake-build-debug

# Include any dependencies generated for this target.
include CMakeFiles/par_prog_lab.dir/depend.make

# Include the progress variables for this target.
include CMakeFiles/par_prog_lab.dir/progress.make

# Include the compile flags for this target's objects.
include CMakeFiles/par_prog_lab.dir/flags.make

CMakeFiles/par_prog_lab.dir/main.cpp.o: CMakeFiles/par_prog_lab.dir/flags.make
CMakeFiles/par_prog_lab.dir/main.cpp.o: ../main.cpp
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --progress-dir=/Users/aleksandrbatasev/CLionProjects/par_prog_lab/cmake-build-debug/CMakeFiles --progress-num=$(CMAKE_PROGRESS_1) "Building CXX object CMakeFiles/par_prog_lab.dir/main.cpp.o"
	/Applications/Xcode.app/Contents/Developer/Toolchains/XcodeDefault.xctoolchain/usr/bin/c++  $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -o CMakeFiles/par_prog_lab.dir/main.cpp.o -c /Users/aleksandrbatasev/CLionProjects/par_prog_lab/main.cpp

CMakeFiles/par_prog_lab.dir/main.cpp.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing CXX source to CMakeFiles/par_prog_lab.dir/main.cpp.i"
	/Applications/Xcode.app/Contents/Developer/Toolchains/XcodeDefault.xctoolchain/usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -E /Users/aleksandrbatasev/CLionProjects/par_prog_lab/main.cpp > CMakeFiles/par_prog_lab.dir/main.cpp.i

CMakeFiles/par_prog_lab.dir/main.cpp.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling CXX source to assembly CMakeFiles/par_prog_lab.dir/main.cpp.s"
	/Applications/Xcode.app/Contents/Developer/Toolchains/XcodeDefault.xctoolchain/usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -S /Users/aleksandrbatasev/CLionProjects/par_prog_lab/main.cpp -o CMakeFiles/par_prog_lab.dir/main.cpp.s

# Object files for target par_prog_lab
par_prog_lab_OBJECTS = \
"CMakeFiles/par_prog_lab.dir/main.cpp.o"

# External object files for target par_prog_lab
par_prog_lab_EXTERNAL_OBJECTS =

par_prog_lab: CMakeFiles/par_prog_lab.dir/main.cpp.o
par_prog_lab: CMakeFiles/par_prog_lab.dir/build.make
par_prog_lab: CMakeFiles/par_prog_lab.dir/link.txt
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --bold --progress-dir=/Users/aleksandrbatasev/CLionProjects/par_prog_lab/cmake-build-debug/CMakeFiles --progress-num=$(CMAKE_PROGRESS_2) "Linking CXX executable par_prog_lab"
	$(CMAKE_COMMAND) -E cmake_link_script CMakeFiles/par_prog_lab.dir/link.txt --verbose=$(VERBOSE)

# Rule to build all files generated by this target.
CMakeFiles/par_prog_lab.dir/build: par_prog_lab

.PHONY : CMakeFiles/par_prog_lab.dir/build

CMakeFiles/par_prog_lab.dir/clean:
	$(CMAKE_COMMAND) -P CMakeFiles/par_prog_lab.dir/cmake_clean.cmake
.PHONY : CMakeFiles/par_prog_lab.dir/clean

CMakeFiles/par_prog_lab.dir/depend:
	cd /Users/aleksandrbatasev/CLionProjects/par_prog_lab/cmake-build-debug && $(CMAKE_COMMAND) -E cmake_depends "Unix Makefiles" /Users/aleksandrbatasev/CLionProjects/par_prog_lab /Users/aleksandrbatasev/CLionProjects/par_prog_lab /Users/aleksandrbatasev/CLionProjects/par_prog_lab/cmake-build-debug /Users/aleksandrbatasev/CLionProjects/par_prog_lab/cmake-build-debug /Users/aleksandrbatasev/CLionProjects/par_prog_lab/cmake-build-debug/CMakeFiles/par_prog_lab.dir/DependInfo.cmake --color=$(COLOR)
.PHONY : CMakeFiles/par_prog_lab.dir/depend

