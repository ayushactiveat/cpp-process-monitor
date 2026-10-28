# Linux Process Monitor (C++17)

A command-line process monitor for Linux written in C++17. Instead of relying on external libraries, it reads directly from the `/proc` virtual filesystem to grab live process stats, calculates CPU usage by comparing tick samples over time, and tracks actual memory usage.

### What it does

- Reads `/proc` directly to find running processes and their stats.
- Calculates CPU % by comparing the delta between two successive samples (utime/stime).
- Handles processes dying mid-read (skips them gracefully instead of crashing).
- Parses memory usage (RSS) from `/proc/[pid]/stat`.
- Includes a GoogleTest suite to test the parsing logic against fake `/proc` data.
  
I loaded the CPU with a `yes > /dev/null` loop to test the delta sampling logic:
<img width="467" height="362" alt="Screenshot 2026-10-28 083542" src="https://github.com/user-attachments/assets/015a5271-cf1e-48a2-a6d4-7b5d65db8ead" />

### Structure

- `include/Process.hpp`: The struct holding process data.
- `include/Parser.hpp` & `src/Parser.cpp`: The parser that reads `/proc` and handles missing files/permissions.
- `src/main.cpp`: The main loop. It iterates through processes, sorts by CPU, and prints the table.
- `tests/`: Unit tests.

### Build and Run

You'll need `g++` and `cmake` installed.

```bash
mkdir build && cd build
cmake ..
make
./monitor
