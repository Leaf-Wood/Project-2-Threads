CXX = g++
CXXFLAGS = -g -Wall -O2

TARGET = mt-collatz

all: $(TARGET)

$(TARGET): main.cpp
	$(CXX) $(CXXFLAGS) main.cpp -o $(TARGET) -pthread

clean:
	rm -f $(TARGET)

.PHONY: all clean