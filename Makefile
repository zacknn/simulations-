# Variables
CXX = g++
CXXFLAGS = -std=c++17 -Wall
LIBS = -lsfml-graphics -lsfml-window -lsfml-system
TARGET = physics_sim

# Get all .cpp files in the directory
SRCS = $(wildcard *.cpp)
OBJS = $(SRCS:.cpp=.o)

# Default rule
all: $(TARGET)

$(TARGET): $(OBJS)
	$(CXX) $(OBJS) -o $(TARGET) $(LIBS)

# Compile a single .cpp file
%.o: %.cpp
	$(CXX) $(CXXFLAGS) -c $< -o $@

# Rule to compile a specific file by name
file:
	$(CXX) $(CXXFLAGS) -c $(file).cpp -o $(file).o

# Clean rule to remove compiled files
clean:
	rm -f $(TARGET) $(OBJS)

# Rule to run the program
run: all
	./$(TARGET)