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

# Include any dependencies generated for this target.
include protobuf_source/CMakeFiles/proto_msg.dir/depend.make

# Include the progress variables for this target.
include protobuf_source/CMakeFiles/proto_msg.dir/progress.make

# Include the compile flags for this target's objects.
include protobuf_source/CMakeFiles/proto_msg.dir/flags.make

protobuf_source/messages.pb.h: ../protobuf_source/messages.proto
protobuf_source/messages.pb.h: /usr/local/bin/protoc
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --blue --bold --progress-dir=/home/michal/mff/bakalarka/bp_program/dsafsfd/CMakeFiles --progress-num=$(CMAKE_PROGRESS_1) "Running cpp protocol buffer compiler on messages.proto"
	cd /home/michal/mff/bakalarka/bp_program/dsafsfd/protobuf_source && /usr/local/bin/protoc --cpp_out /home/michal/mff/bakalarka/bp_program/dsafsfd/protobuf_source -I /home/michal/mff/bakalarka/bp_program/protobuf_source /home/michal/mff/bakalarka/bp_program/protobuf_source/messages.proto

protobuf_source/messages.pb.cc: protobuf_source/messages.pb.h
	@$(CMAKE_COMMAND) -E touch_nocreate protobuf_source/messages.pb.cc

protobuf_source/CMakeFiles/proto_msg.dir/proto_msg_autogen/mocs_compilation.cpp.o: protobuf_source/CMakeFiles/proto_msg.dir/flags.make
protobuf_source/CMakeFiles/proto_msg.dir/proto_msg_autogen/mocs_compilation.cpp.o: protobuf_source/proto_msg_autogen/mocs_compilation.cpp
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --progress-dir=/home/michal/mff/bakalarka/bp_program/dsafsfd/CMakeFiles --progress-num=$(CMAKE_PROGRESS_2) "Building CXX object protobuf_source/CMakeFiles/proto_msg.dir/proto_msg_autogen/mocs_compilation.cpp.o"
	cd /home/michal/mff/bakalarka/bp_program/dsafsfd/protobuf_source && /usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -o CMakeFiles/proto_msg.dir/proto_msg_autogen/mocs_compilation.cpp.o -c /home/michal/mff/bakalarka/bp_program/dsafsfd/protobuf_source/proto_msg_autogen/mocs_compilation.cpp

protobuf_source/CMakeFiles/proto_msg.dir/proto_msg_autogen/mocs_compilation.cpp.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing CXX source to CMakeFiles/proto_msg.dir/proto_msg_autogen/mocs_compilation.cpp.i"
	cd /home/michal/mff/bakalarka/bp_program/dsafsfd/protobuf_source && /usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -E /home/michal/mff/bakalarka/bp_program/dsafsfd/protobuf_source/proto_msg_autogen/mocs_compilation.cpp > CMakeFiles/proto_msg.dir/proto_msg_autogen/mocs_compilation.cpp.i

protobuf_source/CMakeFiles/proto_msg.dir/proto_msg_autogen/mocs_compilation.cpp.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling CXX source to assembly CMakeFiles/proto_msg.dir/proto_msg_autogen/mocs_compilation.cpp.s"
	cd /home/michal/mff/bakalarka/bp_program/dsafsfd/protobuf_source && /usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -S /home/michal/mff/bakalarka/bp_program/dsafsfd/protobuf_source/proto_msg_autogen/mocs_compilation.cpp -o CMakeFiles/proto_msg.dir/proto_msg_autogen/mocs_compilation.cpp.s

protobuf_source/CMakeFiles/proto_msg.dir/messages.pb.cc.o: protobuf_source/CMakeFiles/proto_msg.dir/flags.make
protobuf_source/CMakeFiles/proto_msg.dir/messages.pb.cc.o: protobuf_source/messages.pb.cc
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --progress-dir=/home/michal/mff/bakalarka/bp_program/dsafsfd/CMakeFiles --progress-num=$(CMAKE_PROGRESS_3) "Building CXX object protobuf_source/CMakeFiles/proto_msg.dir/messages.pb.cc.o"
	cd /home/michal/mff/bakalarka/bp_program/dsafsfd/protobuf_source && /usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -o CMakeFiles/proto_msg.dir/messages.pb.cc.o -c /home/michal/mff/bakalarka/bp_program/dsafsfd/protobuf_source/messages.pb.cc

protobuf_source/CMakeFiles/proto_msg.dir/messages.pb.cc.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing CXX source to CMakeFiles/proto_msg.dir/messages.pb.cc.i"
	cd /home/michal/mff/bakalarka/bp_program/dsafsfd/protobuf_source && /usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -E /home/michal/mff/bakalarka/bp_program/dsafsfd/protobuf_source/messages.pb.cc > CMakeFiles/proto_msg.dir/messages.pb.cc.i

protobuf_source/CMakeFiles/proto_msg.dir/messages.pb.cc.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling CXX source to assembly CMakeFiles/proto_msg.dir/messages.pb.cc.s"
	cd /home/michal/mff/bakalarka/bp_program/dsafsfd/protobuf_source && /usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -S /home/michal/mff/bakalarka/bp_program/dsafsfd/protobuf_source/messages.pb.cc -o CMakeFiles/proto_msg.dir/messages.pb.cc.s

# Object files for target proto_msg
proto_msg_OBJECTS = \
"CMakeFiles/proto_msg.dir/proto_msg_autogen/mocs_compilation.cpp.o" \
"CMakeFiles/proto_msg.dir/messages.pb.cc.o"

# External object files for target proto_msg
proto_msg_EXTERNAL_OBJECTS =

protobuf_source/libproto_msg.a: protobuf_source/CMakeFiles/proto_msg.dir/proto_msg_autogen/mocs_compilation.cpp.o
protobuf_source/libproto_msg.a: protobuf_source/CMakeFiles/proto_msg.dir/messages.pb.cc.o
protobuf_source/libproto_msg.a: protobuf_source/CMakeFiles/proto_msg.dir/build.make
protobuf_source/libproto_msg.a: protobuf_source/CMakeFiles/proto_msg.dir/link.txt
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --bold --progress-dir=/home/michal/mff/bakalarka/bp_program/dsafsfd/CMakeFiles --progress-num=$(CMAKE_PROGRESS_4) "Linking CXX static library libproto_msg.a"
	cd /home/michal/mff/bakalarka/bp_program/dsafsfd/protobuf_source && $(CMAKE_COMMAND) -P CMakeFiles/proto_msg.dir/cmake_clean_target.cmake
	cd /home/michal/mff/bakalarka/bp_program/dsafsfd/protobuf_source && $(CMAKE_COMMAND) -E cmake_link_script CMakeFiles/proto_msg.dir/link.txt --verbose=$(VERBOSE)

# Rule to build all files generated by this target.
protobuf_source/CMakeFiles/proto_msg.dir/build: protobuf_source/libproto_msg.a

.PHONY : protobuf_source/CMakeFiles/proto_msg.dir/build

protobuf_source/CMakeFiles/proto_msg.dir/clean:
	cd /home/michal/mff/bakalarka/bp_program/dsafsfd/protobuf_source && $(CMAKE_COMMAND) -P CMakeFiles/proto_msg.dir/cmake_clean.cmake
.PHONY : protobuf_source/CMakeFiles/proto_msg.dir/clean

protobuf_source/CMakeFiles/proto_msg.dir/depend: protobuf_source/messages.pb.h
protobuf_source/CMakeFiles/proto_msg.dir/depend: protobuf_source/messages.pb.cc
	cd /home/michal/mff/bakalarka/bp_program/dsafsfd && $(CMAKE_COMMAND) -E cmake_depends "Unix Makefiles" /home/michal/mff/bakalarka/bp_program /home/michal/mff/bakalarka/bp_program/protobuf_source /home/michal/mff/bakalarka/bp_program/dsafsfd /home/michal/mff/bakalarka/bp_program/dsafsfd/protobuf_source /home/michal/mff/bakalarka/bp_program/dsafsfd/protobuf_source/CMakeFiles/proto_msg.dir/DependInfo.cmake --color=$(COLOR)
.PHONY : protobuf_source/CMakeFiles/proto_msg.dir/depend
