CXXFLAGS = -std=gnu++11 -O2 '-DGAME_NAME="Space Runners"'
LDFLAGS = -lm -lSDL2 -lSDL2_ttf -lSDL2_image -lGLU -lGL
SRCS = $(wildcard *.cc)
OBJS = $(patsubst %.cc,%.o,$(SRCS))
BNAME = glgame

CXX ?= c++

.PHONY: all clean

all: $(BNAME)

$(BNAME): $(OBJS)
	$(CXX) -o $(BNAME) $(OBJS) $(LDFLAGS)

%.o: %.cc
	$(CXX) $(CXXFLAGS) -c -o $@ $<
	
clean:
	rm -f $(BNAME) $(OBJS)
