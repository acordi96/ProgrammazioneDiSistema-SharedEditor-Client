# CMAKE generated file: DO NOT EDIT!
# Generated by "Unix Makefiles" Generator, CMake Version 3.16

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
CMAKE_COMMAND = /snap/clion/111/bin/cmake/linux/bin/cmake

# The command to remove a file.
RM = /snap/clion/111/bin/cmake/linux/bin/cmake -E remove -f

# Escaping for special characters.
EQUALS = =

# The top-level source directory on which CMake was run.
CMAKE_SOURCE_DIR = /home/cordi/prog_prova_git

# The top-level build directory on which CMake was run.
CMAKE_BINARY_DIR = /home/cordi/prog_prova_git/cmake-build-debug

# Utility rule file for prova_autogen.

# Include the progress variables for this target.
include CMakeFiles/prova_autogen.dir/progress.make

CMakeFiles/prova_autogen:
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --blue --bold --progress-dir=/home/cordi/prog_prova_git/cmake-build-debug/CMakeFiles --progress-num=$(CMAKE_PROGRESS_1) "Automatic MOC and UIC for target prova"
	/snap/clion/111/bin/cmake/linux/bin/cmake -E cmake_autogen /home/cordi/prog_prova_git/cmake-build-debug/CMakeFiles/prova_autogen.dir/AutogenInfo.json Debug

prova_autogen: CMakeFiles/prova_autogen
prova_autogen: CMakeFiles/prova_autogen.dir/build.make

.PHONY : prova_autogen

# Rule to build all files generated by this target.
CMakeFiles/prova_autogen.dir/build: prova_autogen

.PHONY : CMakeFiles/prova_autogen.dir/build

CMakeFiles/prova_autogen.dir/clean:
	$(CMAKE_COMMAND) -P CMakeFiles/prova_autogen.dir/cmake_clean.cmake
.PHONY : CMakeFiles/prova_autogen.dir/clean

CMakeFiles/prova_autogen.dir/depend:
	cd /home/cordi/prog_prova_git/cmake-build-debug && $(CMAKE_COMMAND) -E cmake_depends "Unix Makefiles" /home/cordi/prog_prova_git /home/cordi/prog_prova_git /home/cordi/prog_prova_git/cmake-build-debug /home/cordi/prog_prova_git/cmake-build-debug /home/cordi/prog_prova_git/cmake-build-debug/CMakeFiles/prova_autogen.dir/DependInfo.cmake --color=$(COLOR)
.PHONY : CMakeFiles/prova_autogen.dir/depend

