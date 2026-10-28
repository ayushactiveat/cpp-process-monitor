# Linux Process Monitor (C++17)

![CI](https://github.com/ayushactiveat/cpp-process-monitor/actions/workflows/ci.yml/badge.svg)

A small command-line process monitor for Linux. It reads the `/proc`
filesystem directly (no external libraries) and shows the top 15 processes
by CPU usage, refreshed every second.

## How it works

- Scans the numeric directories in `/proc` and reads each process's `comm`
  and `stat` files.
- **CPU %**: compares `utime + stime` between two samples and divides by the
  real elapsed time between them (measured with `steady_clock`).
- **Memory**: RSS from `/proc/[pid]/stat`, which is counted in pages, converted
  to KB using the system page size.
- Process names with spaces (like `Web Content`) are handled by parsing the
  stat line after the last `)`.
- Processes that exit mid-read, or whose files can't be opened (permissions),
  are skipped instead of crashing the monitor. Truncated or garbled stat
  lines are parsed as zeros rather than throwing.

## Structure

- `include/Process.hpp`: struct holding per-process data
- `include/Parser.hpp`, `src/Parser.cpp`: file reading, stat parsing and the
  CPU calculation
- `src/main.cpp`: scanning loop, sorting and table output
- `tests/test_parser.cpp`: GoogleTest unit tests

## Build, run and test

You need g++ and CMake (3.14 or newer). GoogleTest is downloaded on the
first configure, so that step needs internet access.

    mkdir build && cd build
    cmake ..
    make
    ./monitor                   # Ctrl+C to quit
    ctest --output-on-failure   # run the tests

The tests use hand-written sample `/proc/[pid]/stat` lines (including names
with spaces, parentheses in the name, and truncated lines), plus checks for
the CPU calculation and for reading missing or empty files.

## Limitations

- Linux only. Developed and tested on Ubuntu under WSL2.
- CPU % is per core, so a multi-threaded process can go above 100%.
- The first screen shows 0% for everything because CPU needs two samples.
- No filtering or sort options yet.
