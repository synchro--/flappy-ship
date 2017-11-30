CXXFLAGS = -std=c++11 -g
LDFLAGS = -lm -lSDL2 -lSDL2_ttf -lSDL2_image -lGLU -lGL
MAKEFLAGS=-j4
SRCS = $(wildcard *.cxx)
OBJS = $(patsubst %.cxx,%.o,$(SRCS))
BNAME = start_game

CXX ?= c++

CCACHE_EXISTS := $(shell ccache -V)
ifdef CCACHE_EXISTS
    CC := ccache $(CC)
    CXX := ccache $(CXX)
endif

.PHONY: all clean

all: $(BNAME)

$(BNAME): $(OBJS)
	$(CXX) -o $(BNAME) $(OBJS) $(LDFLAGS)

%.o: %.cxx
	$(CXX) $(CXXFLAGS) -c -o $@ $<
	
clean:
	rm -f $(BNAME) $(OBJS)
