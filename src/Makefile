UNAME := $(shell uname)

ifeq ($(UNAME),Darwin)
  HOMEBREW_PREFIX ?= $(shell brew --prefix 2>/dev/null || echo /opt/homebrew)
  GLEW_HOME ?= $(HOMEBREW_PREFIX)/opt/glew
  CXXFLAGS = -std=c++11 -g -I$(GLEW_HOME)/include -I$(HOMEBREW_PREFIX)/include
  LDFLAGS = -L$(GLEW_HOME)/lib -L$(HOMEBREW_PREFIX)/lib -lGLEW -framework OpenGL -lSDL2 -lSDL2_image -lSDL2_ttf
else
  CXXFLAGS = -std=c++11 -g
  LDFLAGS = -lGLEW -lGLU -lGL -lSDL2 -lSDL2_image -lSDL2_ttf
endif
MAKEFLAGS=-j4
SRCS = $(wildcard *.cxx)
OBJS = $(patsubst %.cxx,%.o,$(SRCS))
BNAME = start_game

CXX ?= c++

#CCACHE_EXISTS := $(shell ccache -V)
#ifdef CCACHE_EXISTS
#    CC := ccache $(CC)
#    CXX := ccache $(CXX)
#endif

.PHONY: all clean

all: $(BNAME)

$(BNAME): $(OBJS)
	$(CXX) -o $(BNAME) $(OBJS) $(LDFLAGS)

%.o: %.cxx
	$(CXX) $(CXXFLAGS) -c -o $@ $<
	
clean:
	rm -f $(BNAME) $(OBJS)
