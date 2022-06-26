# CMAKE generated file: DO NOT EDIT!
# Generated by "Unix Makefiles" Generator, CMake Version 3.23

# Default target executed when no arguments are given to make.
default_target: all
.PHONY : default_target

# Allow only one "make -f Makefile2" at a time, but pass parallelism.
.NOTPARALLEL:

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
CMAKE_COMMAND = /opt/homebrew/Cellar/cmake/3.23.2/bin/cmake

# The command to remove a file.
RM = /opt/homebrew/Cellar/cmake/3.23.2/bin/cmake -E rm -f

# Escaping for special characters.
EQUALS = =

# The top-level source directory on which CMake was run.
CMAKE_SOURCE_DIR = /Users/casperwong/Documents/GitHub/Chess

# The top-level build directory on which CMake was run.
CMAKE_BINARY_DIR = /Users/casperwong/Documents/GitHub/Chess

#=============================================================================
# Targets provided globally by CMake.

# Special rule for the target edit_cache
edit_cache:
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --cyan "Running CMake cache editor..."
	/opt/homebrew/Cellar/cmake/3.23.2/bin/ccmake -S$(CMAKE_SOURCE_DIR) -B$(CMAKE_BINARY_DIR)
.PHONY : edit_cache

# Special rule for the target edit_cache
edit_cache/fast: edit_cache
.PHONY : edit_cache/fast

# Special rule for the target rebuild_cache
rebuild_cache:
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --cyan "Running CMake to regenerate build system..."
	/opt/homebrew/Cellar/cmake/3.23.2/bin/cmake --regenerate-during-build -S$(CMAKE_SOURCE_DIR) -B$(CMAKE_BINARY_DIR)
.PHONY : rebuild_cache

# Special rule for the target rebuild_cache
rebuild_cache/fast: rebuild_cache
.PHONY : rebuild_cache/fast

# The main all target
all: cmake_check_build_system
	$(CMAKE_COMMAND) -E cmake_progress_start /Users/casperwong/Documents/GitHub/Chess/CMakeFiles /Users/casperwong/Documents/GitHub/Chess//CMakeFiles/progress.marks
	$(MAKE) $(MAKESILENT) -f CMakeFiles/Makefile2 all
	$(CMAKE_COMMAND) -E cmake_progress_start /Users/casperwong/Documents/GitHub/Chess/CMakeFiles 0
.PHONY : all

# The main clean target
clean:
	$(MAKE) $(MAKESILENT) -f CMakeFiles/Makefile2 clean
.PHONY : clean

# The main clean target
clean/fast: clean
.PHONY : clean/fast

# Prepare targets for installation.
preinstall: all
	$(MAKE) $(MAKESILENT) -f CMakeFiles/Makefile2 preinstall
.PHONY : preinstall

# Prepare targets for installation.
preinstall/fast:
	$(MAKE) $(MAKESILENT) -f CMakeFiles/Makefile2 preinstall
.PHONY : preinstall/fast

# clear depends
depend:
	$(CMAKE_COMMAND) -S$(CMAKE_SOURCE_DIR) -B$(CMAKE_BINARY_DIR) --check-build-system CMakeFiles/Makefile.cmake 1
.PHONY : depend

#=============================================================================
# Target rules for targets named Chess

# Build rule for target.
Chess: cmake_check_build_system
	$(MAKE) $(MAKESILENT) -f CMakeFiles/Makefile2 Chess
.PHONY : Chess

# fast build rule for target.
Chess/fast:
	$(MAKE) $(MAKESILENT) -f CMakeFiles/Chess.dir/build.make CMakeFiles/Chess.dir/build
.PHONY : Chess/fast

lichess_bot_refactored/ChessEngine.o: lichess_bot_refactored/ChessEngine.c.o
.PHONY : lichess_bot_refactored/ChessEngine.o

# target to build an object file
lichess_bot_refactored/ChessEngine.c.o:
	$(MAKE) $(MAKESILENT) -f CMakeFiles/Chess.dir/build.make CMakeFiles/Chess.dir/lichess_bot_refactored/ChessEngine.c.o
.PHONY : lichess_bot_refactored/ChessEngine.c.o

lichess_bot_refactored/ChessEngine.i: lichess_bot_refactored/ChessEngine.c.i
.PHONY : lichess_bot_refactored/ChessEngine.i

# target to preprocess a source file
lichess_bot_refactored/ChessEngine.c.i:
	$(MAKE) $(MAKESILENT) -f CMakeFiles/Chess.dir/build.make CMakeFiles/Chess.dir/lichess_bot_refactored/ChessEngine.c.i
.PHONY : lichess_bot_refactored/ChessEngine.c.i

lichess_bot_refactored/ChessEngine.s: lichess_bot_refactored/ChessEngine.c.s
.PHONY : lichess_bot_refactored/ChessEngine.s

# target to generate assembly for a file
lichess_bot_refactored/ChessEngine.c.s:
	$(MAKE) $(MAKESILENT) -f CMakeFiles/Chess.dir/build.make CMakeFiles/Chess.dir/lichess_bot_refactored/ChessEngine.c.s
.PHONY : lichess_bot_refactored/ChessEngine.c.s

lichess_bot_refactored/board_manipulations.o: lichess_bot_refactored/board_manipulations.c.o
.PHONY : lichess_bot_refactored/board_manipulations.o

# target to build an object file
lichess_bot_refactored/board_manipulations.c.o:
	$(MAKE) $(MAKESILENT) -f CMakeFiles/Chess.dir/build.make CMakeFiles/Chess.dir/lichess_bot_refactored/board_manipulations.c.o
.PHONY : lichess_bot_refactored/board_manipulations.c.o

lichess_bot_refactored/board_manipulations.i: lichess_bot_refactored/board_manipulations.c.i
.PHONY : lichess_bot_refactored/board_manipulations.i

# target to preprocess a source file
lichess_bot_refactored/board_manipulations.c.i:
	$(MAKE) $(MAKESILENT) -f CMakeFiles/Chess.dir/build.make CMakeFiles/Chess.dir/lichess_bot_refactored/board_manipulations.c.i
.PHONY : lichess_bot_refactored/board_manipulations.c.i

lichess_bot_refactored/board_manipulations.s: lichess_bot_refactored/board_manipulations.c.s
.PHONY : lichess_bot_refactored/board_manipulations.s

# target to generate assembly for a file
lichess_bot_refactored/board_manipulations.c.s:
	$(MAKE) $(MAKESILENT) -f CMakeFiles/Chess.dir/build.make CMakeFiles/Chess.dir/lichess_bot_refactored/board_manipulations.c.s
.PHONY : lichess_bot_refactored/board_manipulations.c.s

lichess_bot_refactored/dev_tools.o: lichess_bot_refactored/dev_tools.c.o
.PHONY : lichess_bot_refactored/dev_tools.o

# target to build an object file
lichess_bot_refactored/dev_tools.c.o:
	$(MAKE) $(MAKESILENT) -f CMakeFiles/Chess.dir/build.make CMakeFiles/Chess.dir/lichess_bot_refactored/dev_tools.c.o
.PHONY : lichess_bot_refactored/dev_tools.c.o

lichess_bot_refactored/dev_tools.i: lichess_bot_refactored/dev_tools.c.i
.PHONY : lichess_bot_refactored/dev_tools.i

# target to preprocess a source file
lichess_bot_refactored/dev_tools.c.i:
	$(MAKE) $(MAKESILENT) -f CMakeFiles/Chess.dir/build.make CMakeFiles/Chess.dir/lichess_bot_refactored/dev_tools.c.i
.PHONY : lichess_bot_refactored/dev_tools.c.i

lichess_bot_refactored/dev_tools.s: lichess_bot_refactored/dev_tools.c.s
.PHONY : lichess_bot_refactored/dev_tools.s

# target to generate assembly for a file
lichess_bot_refactored/dev_tools.c.s:
	$(MAKE) $(MAKESILENT) -f CMakeFiles/Chess.dir/build.make CMakeFiles/Chess.dir/lichess_bot_refactored/dev_tools.c.s
.PHONY : lichess_bot_refactored/dev_tools.c.s

# Help Target
help:
	@echo "The following are some of the valid targets for this Makefile:"
	@echo "... all (the default if no target is provided)"
	@echo "... clean"
	@echo "... depend"
	@echo "... edit_cache"
	@echo "... rebuild_cache"
	@echo "... Chess"
	@echo "... lichess_bot_refactored/ChessEngine.o"
	@echo "... lichess_bot_refactored/ChessEngine.i"
	@echo "... lichess_bot_refactored/ChessEngine.s"
	@echo "... lichess_bot_refactored/board_manipulations.o"
	@echo "... lichess_bot_refactored/board_manipulations.i"
	@echo "... lichess_bot_refactored/board_manipulations.s"
	@echo "... lichess_bot_refactored/dev_tools.o"
	@echo "... lichess_bot_refactored/dev_tools.i"
	@echo "... lichess_bot_refactored/dev_tools.s"
.PHONY : help



#=============================================================================
# Special targets to cleanup operation of make.

# Special rule to run CMake to check the build system integrity.
# No rule that depends on this can have commands that come from listfiles
# because they might be regenerated.
cmake_check_build_system:
	$(CMAKE_COMMAND) -S$(CMAKE_SOURCE_DIR) -B$(CMAKE_BINARY_DIR) --check-build-system CMakeFiles/Makefile.cmake 0
.PHONY : cmake_check_build_system
