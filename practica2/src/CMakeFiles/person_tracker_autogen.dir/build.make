# CMAKE generated file: DO NOT EDIT!
# Generated by "Unix Makefiles" Generator, CMake Version 3.24

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
CMAKE_SOURCE_DIR = /home/antonio/robocomp/components/grupo4/practica2

# The top-level build directory on which CMake was run.
CMAKE_BINARY_DIR = /home/antonio/robocomp/components/grupo4/practica2

# Utility rule file for person_tracker_autogen.

# Include any custom commands dependencies for this target.
include src/CMakeFiles/person_tracker_autogen.dir/compiler_depend.make

# Include the progress variables for this target.
include src/CMakeFiles/person_tracker_autogen.dir/progress.make

src/CMakeFiles/person_tracker_autogen:
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --blue --bold --progress-dir=/home/antonio/robocomp/components/grupo4/practica2/CMakeFiles --progress-num=$(CMAKE_PROGRESS_1) "Automatic MOC and UIC for target person_tracker"
	cd /home/antonio/robocomp/components/grupo4/practica2/src && /usr/local/bin/cmake -E cmake_autogen /home/antonio/robocomp/components/grupo4/practica2/src/CMakeFiles/person_tracker_autogen.dir/AutogenInfo.json ""

person_tracker_autogen: src/CMakeFiles/person_tracker_autogen
person_tracker_autogen: src/CMakeFiles/person_tracker_autogen.dir/build.make
.PHONY : person_tracker_autogen

# Rule to build all files generated by this target.
src/CMakeFiles/person_tracker_autogen.dir/build: person_tracker_autogen
.PHONY : src/CMakeFiles/person_tracker_autogen.dir/build

src/CMakeFiles/person_tracker_autogen.dir/clean:
	cd /home/antonio/robocomp/components/grupo4/practica2/src && $(CMAKE_COMMAND) -P CMakeFiles/person_tracker_autogen.dir/cmake_clean.cmake
.PHONY : src/CMakeFiles/person_tracker_autogen.dir/clean

src/CMakeFiles/person_tracker_autogen.dir/depend:
	cd /home/antonio/robocomp/components/grupo4/practica2 && $(CMAKE_COMMAND) -E cmake_depends "Unix Makefiles" /home/antonio/robocomp/components/grupo4/practica2 /home/antonio/robocomp/components/grupo4/practica2/src /home/antonio/robocomp/components/grupo4/practica2 /home/antonio/robocomp/components/grupo4/practica2/src /home/antonio/robocomp/components/grupo4/practica2/src/CMakeFiles/person_tracker_autogen.dir/DependInfo.cmake --color=$(COLOR)
.PHONY : src/CMakeFiles/person_tracker_autogen.dir/depend

