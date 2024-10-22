# Example 8
EXE=hw5

# Main target
all: $(EXE)

#  Msys/MinGW (for Windows)
ifeq "$(OS)" "Windows_NT"
CFLG=-O3 -Wall -DUSEGLEW `sdl2-config --cflags`
LIBS=-lfreeglut -lglew32 -lglu32 -lopengl32 -lm `sdl2-config --libs` -lSDL2_mixer
CLEAN=rm -f *.exe *.o *.a

#  OSX
else ifeq "$(shell uname)" "Darwin"
RES=$(shell uname -r|sed -E 's/(.).*/\1/'|tr 12 21)
LDFLAGS  :=$(shell sdl2-config --libs)
CXXFLAGS :=$(shell sdl2-config --cflags)
CFLG=-O3 -Wall -Wno-deprecated-declarations $(CXXFLAGS) -DRES=$(RES)
# CFLG=-O3 -Wall -Wno-deprecated-declarations -DRES=$(RES) `sdl2-config --cflags`
LIBS=-framework GLUT -framework OpenGL `sdl2-config --libs` -lSDL2_mixer
CLEAN=rm -f $(EXE) *.o *.a

#  Linux/Unix/Solaris
else
CFLG=-O3 -Wall `sdl2-config --cflags`
LIBS=-lglut -lGLU -lGL -lm `sdl2-config --libs` -lSDL2_mixer
CLEAN=rm -f $(EXE) *.o *.a
endif

# Compile rules
.c.o:
	gcc -c $(CFLG)  $<

.cpp.o:
	g++ -c $(CFLG)  $<

#  Link
hw5: hw5.o  
	gcc $(CFLG) -o $@ $^  $(LIBS)

#  Clean
clean:
	$(CLEAN)
