CXXFLAGS = -std=gnu++11 
LDFLAGS = -lm -lSDL2 -lSDL2_ttf -lSDL2_image -lGLU -lGL
SRCS = $(wildcard *.cxx)
OBJS = $(patsubst %.cxx,%.o,$(SRCS))
BNAME = start_game

CXX ?= c++

.PHONY: all clean

all: $(BNAME)

$(BNAME): $(OBJS)
	$(CXX) -o $(BNAME) $(OBJS) $(LDFLAGS)

%.o: %.cxx
	$(CXX) $(CXXFLAGS) -c -o $@ $<
	
clean:
	rm -f $(BNAME) $(OBJS)
