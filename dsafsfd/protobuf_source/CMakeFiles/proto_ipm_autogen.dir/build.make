# CMAKE generated file: DO NOT EDIT!
# Generated by "Unix Makefiles" Generator, CMake Version 3.18

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
CMAKE_COMMAND = /usr/bin/cmake

# The command to remove a file.
RM = /usr/bin/cmake -E rm -f

# Escaping for special characters.
EQUALS = =

# The top-level source directory on which CMake was run.
CMAKE_SOURCE_DIR = /home/michal/mff/bakalarka/bp_program

# The top-level build directory on which CMake was run.
CMAKE_BINARY_DIR = /home/michal/mff/bakalarka/bp_program/dsafsfd

# Utility rule file for proto_ipm_autogen.

# Include the progress variables for this target.
include protobuf_source/CMakeFiles/proto_ipm_autogen.dir/progress.make

protobuf_source/CMakeFiles/proto_ipm_autogen: protobuf_source/ip_map.pb.cc
protobuf_source/CMakeFiles/proto_ipm_autogen: protobuf_source/ip_map.pb.h
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --blue --bold --progress-dir=/home/michal/mff/bakalarka/bp_program/dsafsfd/CMakeFiles --progress-num=$(CMAKE_PROGRESS_1) "Automatic MOC and UIC for target proto_ipm"
	cd /home/michal/mff/bakalarka/bp_program/dsafsfd/protobuf_source && /usr/bin/cmake -E cmake_autogen /home/michal/mff/bakalarka/bp_program/dsafsfd/protobuf_source/CMakeFiles/proto_ipm_autogen.dir/AutogenInfo.json ""

protobuf_source/ip_map.pb.h: ../protobuf_source/ip_map.proto
protobuf_source/ip_map.pb.h: /usr/local/bin/protoc
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --blue --bold --progress-dir=/home/michal/mff/bakalarka/bp_program/dsafsfd/CMakeFiles --progress-num=$(CMAKE_PROGRESS_2) "Running cpp protocol buffer compiler on ip_map.proto"
	cd /home/michal/mff/bakalarka/bp_program/dsafsfd/protobuf_source && /usr/local/bin/protoc --cpp_out /home/michal/mff/bakalarka/bp_program/dsafsfd/protobuf_source -I /home/michal/mff/bakalarka/bp_program/protobuf_source /home/michal/mff/bakalarka/bp_program/protobuf_source/ip_map.proto

protobuf_source/ip_map.pb.cc: protobuf_source/ip_map.pb.h
	@$(CMAKE_COMMAND) -E touch_nocreate protobuf_source/ip_map.pb.cc

proto_ipm_autogen: protobuf_source/CMakeFiles/proto_ipm_autogen
proto_ipm_autogen: protobuf_source/ip_map.pb.h
proto_ipm_autogen: protobuf_source/ip_map.pb.cc
proto_ipm_autogen: protobuf_source/CMakeFiles/proto_ipm_autogen.dir/build.make

.PHONY : proto_ipm_autogen

# Rule to build all files generated by this target.
protobuf_source/CMakeFiles/proto_ipm_autogen.dir/build: proto_ipm_autogen

.PHONY : protobuf_source/CMakeFiles/proto_ipm_autogen.dir/build

protobuf_source/CMakeFiles/proto_ipm_autogen.dir/clean:
	cd /home/michal/mff/bakalarka/bp_program/dsafsfd/protobuf_source && $(CMAKE_COMMAND) -P CMakeFiles/proto_ipm_autogen.dir/cmake_clean.cmake
.PHONY : protobuf_source/CMakeFiles/proto_ipm_autogen.dir/clean

protobuf_source/CMakeFiles/proto_ipm_autogen.dir/depend:
	cd /home/michal/mff/bakalarka/bp_program/dsafsfd && $(CMAKE_COMMAND) -E cmake_depends "Unix Makefiles" /home/michal/mff/bakalarka/bp_program /home/michal/mff/bakalarka/bp_program/protobuf_source /home/michal/mff/bakalarka/bp_program/dsafsfd /home/michal/mff/bakalarka/bp_program/dsafsfd/protobuf_source /home/michal/mff/bakalarka/bp_program/dsafsfd/protobuf_source/CMakeFiles/proto_ipm_autogen.dir/DependInfo.cmake --color=$(COLOR)
.PHONY : protobuf_source/CMakeFiles/proto_ipm_autogen.dir/depend
