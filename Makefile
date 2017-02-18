# Project: flappyShip

CXX  = g++
CC   = gcc
BIN  = flappyShip

OBJ  = main.o car.o mesh.o utils.o gameState.o
LINKOBJ  = main.o car.o mesh.o utils.o gameState.o
LIBS = -L/usr/X11R6 -lGL -lGLU -lSDL2_image -lSDL2 -lm
INCS = -I. -I/usr/X11R6/include
CXXINCS=-std=c++11
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

car.o: car.cxx
	$(CXX) -c car.cxx -o car.o $(CXXFLAGS)

mesh.o: mesh.cxx
	$(CXX) -c mesh.cxx -o mesh.o $(CXXFLAGS)

utils.o: utils.cxx
	$(CXX) -c utils.cxx -o utils.o $(CXXFLAGS)

gameState.o: gameState.cxx
	$(CXX) -c gameState.cxx -o gameState.o $(CXXFLAGS)
