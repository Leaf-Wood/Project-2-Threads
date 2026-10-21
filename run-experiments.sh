#!/bin/bash

# Compile fresh executable
make clean && make

N=10000000
TRIALS=10
OUTPUT="results.csv"

# Create CSV header
echo "N,T,Time,Mode" > "$OUTPUT"

echo "N=$N"

# Loop through thread counts 1 to 8
for T in {1..8}; do
    echo "Running T=$T (Locked)..."
    for run in $(seq 1 $TRIALS); do
        # Run with locks: redirect stdout (histogram) to /dev/null, append stderr (timing) to CSV
        ./mt-collatz $N $T > /dev/null 2>> "$OUTPUT"
    done

    echo "Running T=$T (No Lock)..."
    for run in $(seq 1 $TRIALS); do
        # Run without locks
        ./mt-collatz $N $T -nolock > /dev/null 2>> "$OUTPUT"
    done
done

echo "Experiments complete. Data saved to $OUTPUT."