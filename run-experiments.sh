#!/bin/bash

# Error checking
set -euo pipefail
cd -- "$(dirname -- "$0")"

# Compile fresh executable
make -B || exit 1

N=${1:-10000000}
TRIALS=10
if ! [[ "$N" =~ ^[1-9][0-9]{0,9}$ ]] || [ "$N" -gt 2147483647 ]; then
    echo "N must be an integer from 1 to 2147483647." >&2
    exit 1
fi

# Will keep Windows and Linux runs separate
case "$(uname -s)" in
    MINGW*|MSYS*|CYGWIN*) MACHINE=windows ;;
    Linux*) MACHINE=linux ;;
    *) echo "Run on Windows MINGW64 or Linux." >&2; exit 1 ;;
esac
RESULTS="results/$MACHINE"
mkdir -p "$RESULTS"
OUTPUT="$RESULTS/results.csv"

# Record system specifications
{ date -Iseconds; echo "Experiment: N=$N, T=1..8, $TRIALS trials per mode"; bash system-info.sh; } > "$RESULTS/system-specs.txt"

# Save the histogram and the three N=500000 checks
./mt-collatz $N 1 > "$RESULTS/histogram.csv" 2> "$RESULTS/histogram-time.csv"
./mt-collatz 500000 1 -nolock > "$RESULTS/hist1.txt" 2> "$RESULTS/hist1-time.csv"
./mt-collatz 500000 5 -nolock > "$RESULTS/hist2.txt" 2> "$RESULTS/hist2-time.csv"
./mt-collatz 500000 5 > "$RESULTS/hist3.txt" 2> "$RESULTS/hist3-time.csv"

echo "N,T,Time,Mode" > "$OUTPUT"
echo "N=$N"

# Loop through thread counts 1 to 8
for T in {1..8}; do
    echo "Running T=$T (Locked)..."
    for run in $(seq 1 $TRIALS); do
        TIMING=$(./mt-collatz $N $T 2>&1 > /dev/null)
        echo "$TIMING,Locked" >> "$OUTPUT"
    done

    echo "Running T=$T (No Lock)..."
    for run in $(seq 1 $TRIALS); do
        TIMING=$(./mt-collatz $N $T -nolock 2>&1 > /dev/null)
        echo "$TIMING,NoLock" >> "$OUTPUT"
    done
done

# Calculate the average for each thread count and mode
{
    echo "T,Mode,AverageSeconds,Trials"
    awk -F, 'NR>1 {k=$2 "," $4; sum[k]+=$3; count[k]++}
    END {
        for (k in sum)
            printf "%s,%.6f,%d\n", k, sum[k]/count[k], count[k]
    }' "$OUTPUT" | sort -t, -k1,1n -k2,2
} > "$RESULTS/averages.csv"

# Compare full histogram contents using Bash; diff is not required
{
    for FILE in hist2 hist3; do
        if [[ "$(< "$RESULTS/hist1.txt")" == "$(< "$RESULTS/$FILE.txt")" ]]; then
            echo "hist1.txt and $FILE.txt match."
        else
            echo "hist1.txt and $FILE.txt differ."
        fi
    done
    echo "Separate check timings (N,T,seconds):"
    cat "$RESULTS/hist1-time.csv" "$RESULTS/hist2-time.csv" "$RESULTS/hist3-time.csv"
} > "$RESULTS/comparisons.txt"

echo "=== Histogram checks ==="
cat "$RESULTS/comparisons.txt"
echo "=== Average runtimes (seconds) ==="
cat "$RESULTS/averages.csv"
echo "Experiments complete. Results saved to $RESULTS"
if [ "$MACHINE" = windows ]; then
    powershell.exe -NoProfile -ExecutionPolicy Bypass -File make-excel.ps1 "$RESULTS"
else
    echo "For Excel charts, copy results/linux to Windows and run: make spreadsheet"
fi
