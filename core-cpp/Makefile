CXX = g++
CXXFLAGS = -std=c++17 -Wall -Wextra -Iinclude

SRCS = src/Graph.cpp src/MinHeap.cpp src/main.cpp
OBJS = $(SRCS:.cpp=.o)
TARGET = campus_nav.exe

all: $(TARGET)

$(TARGET): $(OBJS)
	$(CXX) $(CXXFLAGS) -o $@ $^

src/%.o: src/%.cpp
	$(CXX) $(CXXFLAGS) -c $< -o $@

clean:
	rm -f src/*.o $(TARGET)

.PHONY: all clean

