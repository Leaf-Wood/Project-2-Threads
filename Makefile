CXX = g++
CXXFLAGS = -g -Wall -O2

TARGET = mt-collatz

all: $(TARGET)

$(TARGET): mt-collatz.cpp
	$(CXX) $(CXXFLAGS) mt-collatz.cpp -o $(TARGET) -pthread

experiment:
	bash run-experiments.sh

spreadsheet:
	powershell.exe -NoProfile -ExecutionPolicy Bypass -File make-excel.ps1

clean:
	rm -f $(TARGET) $(TARGET).exe
	rm -rf results/windows results/linux

.PHONY: all experiment spreadsheet clean
