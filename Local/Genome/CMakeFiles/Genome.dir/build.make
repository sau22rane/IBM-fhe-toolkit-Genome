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
CMAKE_COMMAND = /usr/bin/cmake

# The command to remove a file.
RM = /usr/bin/cmake -E remove -f

# Escaping for special characters.
EQUALS = =

# The top-level source directory on which CMake was run.
CMAKE_SOURCE_DIR = /opt/IBM/FHE-Workspace/examples/Genome

# The top-level build directory on which CMake was run.
CMAKE_BINARY_DIR = /opt/IBM/FHE-Workspace/examples/Genome

# Include any dependencies generated for this target.
include CMakeFiles/Genome.dir/depend.make

# Include the progress variables for this target.
include CMakeFiles/Genome.dir/progress.make

# Include the compile flags for this target's objects.
include CMakeFiles/Genome.dir/flags.make

CMakeFiles/Genome.dir/Genome.cpp.o: CMakeFiles/Genome.dir/flags.make
CMakeFiles/Genome.dir/Genome.cpp.o: Genome.cpp
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --progress-dir=/opt/IBM/FHE-Workspace/examples/Genome/CMakeFiles --progress-num=$(CMAKE_PROGRESS_1) "Building CXX object CMakeFiles/Genome.dir/Genome.cpp.o"
	/usr/bin/c++  $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -o CMakeFiles/Genome.dir/Genome.cpp.o -c /opt/IBM/FHE-Workspace/examples/Genome/Genome.cpp

CMakeFiles/Genome.dir/Genome.cpp.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing CXX source to CMakeFiles/Genome.dir/Genome.cpp.i"
	/usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -E /opt/IBM/FHE-Workspace/examples/Genome/Genome.cpp > CMakeFiles/Genome.dir/Genome.cpp.i

CMakeFiles/Genome.dir/Genome.cpp.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling CXX source to assembly CMakeFiles/Genome.dir/Genome.cpp.s"
	/usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -S /opt/IBM/FHE-Workspace/examples/Genome/Genome.cpp -o CMakeFiles/Genome.dir/Genome.cpp.s

# Object files for target Genome
Genome_OBJECTS = \
"CMakeFiles/Genome.dir/Genome.cpp.o"

# External object files for target Genome
Genome_EXTERNAL_OBJECTS =

Genome: CMakeFiles/Genome.dir/Genome.cpp.o
Genome: CMakeFiles/Genome.dir/build.make
Genome: /usr/local/lib/libhelib.so.1.0.0
Genome: /usr/local/lib/libntl.so
Genome: /usr/lib/x86_64-linux-gnu/libgmp.so
Genome: CMakeFiles/Genome.dir/link.txt
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --bold --progress-dir=/opt/IBM/FHE-Workspace/examples/Genome/CMakeFiles --progress-num=$(CMAKE_PROGRESS_2) "Linking CXX executable Genome"
	$(CMAKE_COMMAND) -E cmake_link_script CMakeFiles/Genome.dir/link.txt --verbose=$(VERBOSE)

# Rule to build all files generated by this target.
CMakeFiles/Genome.dir/build: Genome

.PHONY : CMakeFiles/Genome.dir/build

CMakeFiles/Genome.dir/clean:
	$(CMAKE_COMMAND) -P CMakeFiles/Genome.dir/cmake_clean.cmake
.PHONY : CMakeFiles/Genome.dir/clean

CMakeFiles/Genome.dir/depend:
	cd /opt/IBM/FHE-Workspace/examples/Genome && $(CMAKE_COMMAND) -E cmake_depends "Unix Makefiles" /opt/IBM/FHE-Workspace/examples/Genome /opt/IBM/FHE-Workspace/examples/Genome /opt/IBM/FHE-Workspace/examples/Genome /opt/IBM/FHE-Workspace/examples/Genome /opt/IBM/FHE-Workspace/examples/Genome/CMakeFiles/Genome.dir/DependInfo.cmake --color=$(COLOR)
.PHONY : CMakeFiles/Genome.dir/depend

