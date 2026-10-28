# Linux Process Monitor (C++17)

A small command-line process monitor for Linux. It reads the `/proc`
filesystem directly (no external libraries) and shows the top 15
processes by CPU usage, refreshed every second.

## How it works
- Scans the numeric directories in `/proc` and reads each process's
  `comm` and `stat` files.
- CPU %: compares `utime + stime` between two samples and divides by the
  real elapsed time (measured with `steady_clock`).
- Memory: RSS from `/proc/[pid]/stat` (reported in pages), converted to KB
  using the system page size.
- Names with spaces (e.g. `Web Content`) are handled by parsing the stat
  line after the last `)`.
- Processes that exit mid-read, or whose files can't be opened, are
  skipped instead of crashing the monitor.

## Structure
- `include/Process.hpp`: struct holding per-process data
- `include/Parser.hpp`, `src/Parser.cpp`: file reading, stat parsing, CPU calculation
- `src/main.cpp`: main loop, sorting and table output
- `tests/`: GoogleTest unit tests (sample stat lines and CPU maths)

## Build, run, test
Needs g++ and CMake. GoogleTest is downloaded on the first configure.

    mkdir build && cd build
    cmake ..
    make
    ./monitor                  # Ctrl+C to quit
    ctest --output-on-failure  # run the tests

## Limitations
- Linux only (I tested it on <your setup, e.g. Ubuntu on WSL2>).
- CPU % is per core, so a multi-threaded process can exceed 100%.
- No filtering or sorting options yet.
