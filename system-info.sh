#!/bin/bash

echo "=== SYSTEM SPECIFICATIONS ==="
echo "Host Name: $(hostname)"
echo "CPU Model: $(lscpu | grep 'Model name' | cut -d: -f2 | xargs)"
echo "Sockets:   $(lscpu | grep 'Socket(s):' | cut -d: -f2 | xargs)"
echo "Cores/Socket: $(lscpu | grep 'Core(s) per socket:' | cut -d: -f2 | xargs)"
echo "Total CPU Cores: $(nproc)"
echo "Total Memory: $(free -h | awk '/^Mem:/ {print $2}')"