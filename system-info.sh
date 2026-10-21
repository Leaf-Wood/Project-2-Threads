#!/bin/bash

set -e
export LC_ALL=C

# Windows MINGW64 uses PowerShell for hardware and OS information so that is what we use
case "$(uname -s)" in
    MINGW*|MSYS*|CYGWIN*)
        powershell.exe -NoProfile -Command '
        $ErrorActionPreference = "Stop"
        Get-CimInstance Win32_Processor | Format-List Name,NumberOfCores,NumberOfLogicalProcessors
        Get-CimInstance Win32_ComputerSystem | Format-List TotalPhysicalMemory
        Get-CimInstance Win32_OperatingSystem | Format-List Caption,Version
        '
        g++ --version
        exit 0
        ;;
esac

echo "=== SYSTEM SPECIFICATIONS ==="
echo "Host Name: $(hostname)"
echo "CPU Model: $(lscpu | grep 'Model name' | cut -d: -f2 | xargs)"
echo "Sockets:   $(lscpu | grep 'Socket(s):' | cut -d: -f2 | xargs)"
echo "Cores/Socket: $(lscpu | grep 'Core(s) per socket:' | cut -d: -f2 | xargs)"
echo "Total Physical Cores: $(lscpu -p=Socket,Core | grep -v '^#' | sort -u | wc -l)"
echo "Available Logical Processors: $(nproc)"
echo "Total Memory: $(free -h | awk '/^Mem:/ {print $2}')"
echo "Operating System: $(uname -s -r)"
echo "Compiler: $(g++ --version | sed -n '1p')"
