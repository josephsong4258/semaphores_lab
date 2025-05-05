# No idea what I'm doing - I'll be using this source as a reference to making this
# https://web.stanford.edu/class/archive/cs/cs107/cs107.1174/guide_make.html

# Didn't realize there were assigned readings for this topic
# https://www.cs.colby.edu/maxwell/courses/tutorials/maketutor/
# https://makefiletutorial.com/#targets

# A makefile determines which parts of the program need to be recompiled

# This makefile is based off this resource - # https://www.cs.colby.edu/maxwell/courses/tutorials/maketutor/
# First step, set the default C compiler
CC := gcc
IDIR := .
ODIR := obj

# CFLAGS variable sets compiler flags for gcc
# -g gives debug information, -O0 makes sure the code doesn't get automatically optimized?
CFLAGS := -g -O0 -I$(IDIR)

# Don't know which libraries we're using yet so I'll leave blank
LIBS := -lrt -pthread

# Maps all header files to DEPS
_DEPS := dungeon_info.h dungeon_settings.h
DEPS := $(patsubst %,$(IDIR)/%,$(_DEPS))

# Maps object files to OBJ
SRCS := game.c barbarian.c wizard.c rogue.c
_OBJ := $(SRCS:.c=.o)
OBJ := $(patsubst %,$(ODIR)/%,$(_OBJ))

# Builds executables
all: game barbarian wizard rogue

$(ODIR):
	mkdir -p $(ODIR)

# compiles .c to object file
# $< is the “first prerequisite” (the .c source file)
# $@ is the “target” (the obj/whatever.o file)
$(ODIR)/%.o: %.c $(DEPS) | $(ODIR)
	$(CC) $(CFLAGS) -c $< -o $@

#links game executable to dungeon.o
game: $(ODIR)/game.o dungeon.o
	$(CC) -o $@ $^ $(CFLAGS) $(LIBS)

#Make character executables
barbarian: $(ODIR)/barbarian.o
	$(CC) -o $@ $^ $(CFLAGS) $(LIBS)

wizard: $(ODIR)/wizard.o
	$(CC) -o $@ $^ $(CFLAGS) $(LIBS)

rogue: $(ODIR)/rogue.o
	$(CC) -o $@ $^ $(CFLAGS) $(LIBS)

# helps remove all .o files and executables so we can make fresh runs in
clean:
	rm -rf $(ODIR)/*.o game barbarian wizard rogue

.PHONY: all clean