# Project: flappyShip

CXX  = g++
CC   = gcc
BIN  = flappyShip

OBJ  = main.o env.o elments.o log.o point.o mesh.o game.o window.o 
LINKOBJ  = main.o env.o elments.o log.o point.o mesh.o game.o window.o
LIBS = -L/usr/X11R6 -lGL -lGLU -lSDL2_image -lSDL2 -lm
INCS = -I. -I/usr/X11R6/include
CXXINCS=-std=c++14
CXXFLAGS = $(CXXINCS)
CFLAGS = $(INCS)
RM = rm -f

all: $(BIN)


clean:
	${RM} $(OBJ) $(BIN)

run: $(BIN)
	@./$(BIN)

$(BIN): $(OBJ)
	$(CXX) $(LINKOBJ) -o $(BIN) $(LIBS)

main.o: main.cxx
	$(CXX) -c main.cxx -o main.o $(CXXFLAGS)

env.o: env.cxx
	$(CXX) -c env.cxx -o env.o $(CXXFLAGS)

elements.o: elements.cxx
	$(CXX) -c elements.cxx -o elements.o $(CXXFLAGS)

log.o: log.cxx
	$(CXX) -c log.cxx -o log.o $(CXXFLAGS)

point.o: point.cxx
	$(CXX) -c point.cxx -o point.o $(CXXFLAGS)

mesh.o: mesh.cxx
	$(CXX) -c mesh.cxx -o mesh.o $(CXXFLAGS)

window.o: smart_window.cxx
	$(CXX) -c smart_window.cxx -o window.o $(CXXFLAGS)

game.o: game.cxx
	$(CXX) -c game.cxx -o game.o $(CXXFLAGS)
