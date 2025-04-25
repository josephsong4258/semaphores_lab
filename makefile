# No idea what I'm doing - I'll be using this source as a reference to making this
# https://web.stanford.edu/class/archive/cs/cs107/cs107.1174/guide_make.html

# A makefile manages the build of the project through the C source files

# First step, set the default C compiler
CC = gcc

# CFLAGS variable sets compiler flags for gcc
# -g gives debug information, -O0 makes sure the code doesn't get automatically optimized?
CFLAGS = -g -O0
