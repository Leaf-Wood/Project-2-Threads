CXX = g++
CXXFLAGS = -g -Wall -O2

TARGET = mt-collatz

all: $(TARGET)

$(TARGET): mt-collatz.cpp
	$(CXX) $(CXXFLAGS) mt-collatz.cpp -o $(TARGET) -pthread

clean:
	rm -f $(TARGET)

.PHONY: all clean