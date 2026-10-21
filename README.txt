COP4634 Project 2 - Multithreaded Collatz
Nathan Garrett and Leaf Wood

CURRENT RESULTS
The included results/windows and results/linux folders contain the measured
Windows and university Linux runs. Their workbooks are results/windows.xlsx
and results/linux.xlsx. report.pdf compares and interprets both runs.

1. WINDOWS
Use MINGW64 with g++, make, and Microsoft Excel installed.
Open the project folder and run:

	make experiment

This compiles, runs the histogram checks and all timing trials, records the
system information, and creates results/windows.xlsx automatically.

2. LINUX
Use Bash with g++ and make installed. Open the project folder and run:

	make experiment

This prints histogram comparisons and average runtimes in the terminal and
saves the data in results/linux.
The Excel helper requires Microsoft Excel on Windows. To make the Linux
workbook, copy results/linux into this project's results folder on Windows
and run:

	make spreadsheet

This automatically uses the Linux data if present; otherwise it uses Windows
data. The workbook is results/linux.xlsx or results/windows.xlsx.

RESULTS AND CLEANUP
The experiment uses fixed N=10,000,000, T=1..8, ten trials each with and without
locks. Separate histogram checks use N=500,000. Check comparisons.txt in the
data folder: hist1 and hist3 should match; hist2 may differ because of races.
Raw data stays inside results/windows or results/linux. Rerunning replaces
that computer's previous data and workbook, so copy them first to keep a run.
After reviewing the workbook, run:

	make clean

This deletes the executable and both raw-data folders. Finished .xlsx files
are kept in results. Keep raw data until you have what you need for the report.

PROGRAM ONLY (either system)
    make
    ./mt-collatz 10 1

Arguments: N, thread count, optional -nolock. The histogram goes to stdout;
N,T,time goes to stderr. The program does not fully validate all inputs;
use positive integers. NoLock can produce incorrect histograms.

AI DISCLOSURE
AI assisted with the Excel helper, README, and report. Measurements came from program runs.