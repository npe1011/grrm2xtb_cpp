# Intel Compiler and Options
CXX = icpx
CXXFLAGS = -std=c++17 -O3 -static -I./src

# Target Executable
TARGET = grrm2xtb

# Source Files and Object Files
SRCDIR = src
SOURCES = $(SRCDIR)/main.cpp $(SRCDIR)/grrm.cpp $(SRCDIR)/xtb.cpp $(SRCDIR)/utils.cpp
OBJECTS = $(SOURCES:.cpp=.o)

# Build Rules
all: $(TARGET)

$(TARGET): $(OBJECTS)
	$(CXX) $(CXXFLAGS) -o $@ $(OBJECTS)

%.o: %.cpp
	$(CXX) $(CXXFLAGS) -c $< -o $@

clean:
	rm -f $(TARGET) $(OBJECTS)