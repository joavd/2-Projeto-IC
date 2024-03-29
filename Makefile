###
### First part of a Makefile:
### Define required variables
###

### Variables used by implicit rules:
### https://www.gnu.org/software/make/manual/html_node/Implicit-Variables.html

# C compiler to use
CC=gcc
# Compiler flags
CFLAGS=-Wl,-allow-multiple-definition -Wl,--subsystem,windows -std=c99 -g
# Linker library location flags (e.g. -L)
LDFLAGS= 
# Linker library flags (e.g. -l)
LDLIBS= -Iexternal -lraylib -lglfw3 -lopengl32 -lgdi32 -lopenal32 -lwinmm


### Regular variables

# Name of program
PROGRAM=jogo

###
### Second part of a Makefile:
### Rules to build our example
###

# Rule to generate executable program - by default first target in the makefile
# is generated when "make" is executed. Because no explicit recipe is given,
# the "make" program will use the implicit rule for linking all object files
# into an executable program.
$(PROGRAM): $(PROGRAM).o showworld_raylib.o agent.o world.o ini.o

# Rule to generate program object. Again, because no explicit recipe is given,
# "make" will use an implicit rule for properly compiling $(PROGRAM).o.
$(PROGRAM).o: $(PROGRAM).c showworld.h

# Rule to generate showworld_simple.o object. Again, because no explicit recipe
# is given, "make" will use an implicit rule for properly compiling
# showworld_simple.o.
showworld_raylib.o: showworld_raylib.c showworld.h

agent.o: agent.c agent.h

world.o: world.c world.h
	
ini.o: ini.c ini.h

# The following rule cleans all compiled (objects) and linked (executable
# program) files. Because "clean" is not actually the name of a file, it is
# good practice to specify it as a "phony" target (avoiding conflict with a
# file of the same name and improving performance). In this case, an explicit
# recipe is given, so make will not try to determine an implicit one.
.PHONY: clean


clean:
	rm -f $(PROGRAM) *.o
