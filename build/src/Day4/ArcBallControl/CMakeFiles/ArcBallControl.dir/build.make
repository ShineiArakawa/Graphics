# CMAKE generated file: DO NOT EDIT!
# Generated by "Unix Makefiles" Generator, CMake Version 3.22

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
CMAKE_COMMAND = /usr/local/Cellar/cmake/3.22.2/bin/cmake

# The command to remove a file.
RM = /usr/local/Cellar/cmake/3.22.2/bin/cmake -E rm -f

# Escaping for special characters.
EQUALS = =

# The top-level source directory on which CMake was run.
CMAKE_SOURCE_DIR = /Users/shin/Github/Graphics

# The top-level build directory on which CMake was run.
CMAKE_BINARY_DIR = /Users/shin/Github/Graphics/build

# Include any dependencies generated for this target.
include src/Day4/ArcBallControl/CMakeFiles/ArcBallControl.dir/depend.make
# Include any dependencies generated by the compiler for this target.
include src/Day4/ArcBallControl/CMakeFiles/ArcBallControl.dir/compiler_depend.make

# Include the progress variables for this target.
include src/Day4/ArcBallControl/CMakeFiles/ArcBallControl.dir/progress.make

# Include the compile flags for this target's objects.
include src/Day4/ArcBallControl/CMakeFiles/ArcBallControl.dir/flags.make

src/Day4/ArcBallControl/CMakeFiles/ArcBallControl.dir/main.cpp.o: src/Day4/ArcBallControl/CMakeFiles/ArcBallControl.dir/flags.make
src/Day4/ArcBallControl/CMakeFiles/ArcBallControl.dir/main.cpp.o: ../src/Day4/ArcBallControl/main.cpp
src/Day4/ArcBallControl/CMakeFiles/ArcBallControl.dir/main.cpp.o: src/Day4/ArcBallControl/CMakeFiles/ArcBallControl.dir/compiler_depend.ts
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --progress-dir=/Users/shin/Github/Graphics/build/CMakeFiles --progress-num=$(CMAKE_PROGRESS_1) "Building CXX object src/Day4/ArcBallControl/CMakeFiles/ArcBallControl.dir/main.cpp.o"
	cd /Users/shin/Github/Graphics/build/src/Day4/ArcBallControl && /usr/bin/clang++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -MD -MT src/Day4/ArcBallControl/CMakeFiles/ArcBallControl.dir/main.cpp.o -MF CMakeFiles/ArcBallControl.dir/main.cpp.o.d -o CMakeFiles/ArcBallControl.dir/main.cpp.o -c /Users/shin/Github/Graphics/src/Day4/ArcBallControl/main.cpp

src/Day4/ArcBallControl/CMakeFiles/ArcBallControl.dir/main.cpp.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing CXX source to CMakeFiles/ArcBallControl.dir/main.cpp.i"
	cd /Users/shin/Github/Graphics/build/src/Day4/ArcBallControl && /usr/bin/clang++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -E /Users/shin/Github/Graphics/src/Day4/ArcBallControl/main.cpp > CMakeFiles/ArcBallControl.dir/main.cpp.i

src/Day4/ArcBallControl/CMakeFiles/ArcBallControl.dir/main.cpp.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling CXX source to assembly CMakeFiles/ArcBallControl.dir/main.cpp.s"
	cd /Users/shin/Github/Graphics/build/src/Day4/ArcBallControl && /usr/bin/clang++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -S /Users/shin/Github/Graphics/src/Day4/ArcBallControl/main.cpp -o CMakeFiles/ArcBallControl.dir/main.cpp.s

# Object files for target ArcBallControl
ArcBallControl_OBJECTS = \
"CMakeFiles/ArcBallControl.dir/main.cpp.o"

# External object files for target ArcBallControl
ArcBallControl_EXTERNAL_OBJECTS =

src/Day4/ArcBallControl/ArcBallControl: src/Day4/ArcBallControl/CMakeFiles/ArcBallControl.dir/main.cpp.o
src/Day4/ArcBallControl/ArcBallControl: src/Day4/ArcBallControl/CMakeFiles/ArcBallControl.dir/build.make
src/Day4/ArcBallControl/ArcBallControl: /usr/local/lib/libglfw.3.3.dylib
src/Day4/ArcBallControl/ArcBallControl: src/Day4/ArcBallControl/CMakeFiles/ArcBallControl.dir/link.txt
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --bold --progress-dir=/Users/shin/Github/Graphics/build/CMakeFiles --progress-num=$(CMAKE_PROGRESS_2) "Linking CXX executable ArcBallControl"
	cd /Users/shin/Github/Graphics/build/src/Day4/ArcBallControl && $(CMAKE_COMMAND) -E cmake_link_script CMakeFiles/ArcBallControl.dir/link.txt --verbose=$(VERBOSE)
	cd /Users/shin/Github/Graphics/build/src/Day4/ArcBallControl && /usr/local/Cellar/cmake/3.22.2/bin/cmake -E copy /Users/shin/Github/Graphics/src/Day4/ArcBallControl"/render.vert" /Users/shin/Github/Graphics/build/src/Day4/ArcBallControl
	cd /Users/shin/Github/Graphics/build/src/Day4/ArcBallControl && /usr/local/Cellar/cmake/3.22.2/bin/cmake -E copy /Users/shin/Github/Graphics/src/Day4/ArcBallControl"/render.frag" /Users/shin/Github/Graphics/build/src/Day4/ArcBallControl

# Rule to build all files generated by this target.
src/Day4/ArcBallControl/CMakeFiles/ArcBallControl.dir/build: src/Day4/ArcBallControl/ArcBallControl
.PHONY : src/Day4/ArcBallControl/CMakeFiles/ArcBallControl.dir/build

src/Day4/ArcBallControl/CMakeFiles/ArcBallControl.dir/clean:
	cd /Users/shin/Github/Graphics/build/src/Day4/ArcBallControl && $(CMAKE_COMMAND) -P CMakeFiles/ArcBallControl.dir/cmake_clean.cmake
.PHONY : src/Day4/ArcBallControl/CMakeFiles/ArcBallControl.dir/clean

src/Day4/ArcBallControl/CMakeFiles/ArcBallControl.dir/depend:
	cd /Users/shin/Github/Graphics/build && $(CMAKE_COMMAND) -E cmake_depends "Unix Makefiles" /Users/shin/Github/Graphics /Users/shin/Github/Graphics/src/Day4/ArcBallControl /Users/shin/Github/Graphics/build /Users/shin/Github/Graphics/build/src/Day4/ArcBallControl /Users/shin/Github/Graphics/build/src/Day4/ArcBallControl/CMakeFiles/ArcBallControl.dir/DependInfo.cmake --color=$(COLOR)
.PHONY : src/Day4/ArcBallControl/CMakeFiles/ArcBallControl.dir/depend

