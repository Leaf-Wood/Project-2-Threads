CXX = g++
CXXFLAGS = -g -Wall -O2

TARGET = mt-collatz

all: $(TARGET)

$(TARGET): main.cpp
	$(CXX) $(CXXFLAGS) main.cpp -o $(TARGET)

clean:
	rm -f $(TARGET)

.PHONY: all clean