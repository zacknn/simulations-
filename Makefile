# Variables
CXX = g++
CXXFLAGS = -std=c++17 -Wall
LIBS = -lsfml-graphics -lsfml-window -lsfml-system

# Get all .cpp files in the directory
SRCS = $(wildcard *.cpp)
BINARIES = $(SRCS:.cpp=)

# Default rule: build all executables with the same basename as each .cpp file
all: $(BINARIES)

# Build an executable from a single source file
%: %.cpp
	$(CXX) $(CXXFLAGS) $< -o $@ $(LIBS)

# Clean rule to remove generated executables
clean:
	rm -f $(BINARIES)
