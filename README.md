# Linux Process Monitor (C++17)

A high-performance, command-line process monitor for Linux built from scratch in C++17. This tool parses the `/proc` virtual filesystem to extract real-time process telemetry, calculate CPU utilization via delta-sampling, and track memory usage.

## 🚀 Features

- **Real-Time Telemetry**: Continuously monitors and displays process data, updating every second.
- **CPU Delta-Sampling**: Calculates accurate CPU utilization percentages by comparing successive CPU tick samples (utime/stime) against the system's clock ticks.
- **OS-Level Resilience**: Handles race conditions and edge cases, such as processes terminating mid-read, preventing crashes.
- **Memory Profiling**: Extracts Resident Set Size (RSS) from `/proc/[pid]/stat` to display actual physical memory usage.
- **Test-Driven Development**: Includes a unit testing suite built with **GoogleTest** to validate parsing logic using mocked `/proc` data.
- *Live output demonstrating CPU delta-sampling under heavy system load.*
- <img width="467" height="362" alt="Screenshot 2026-10-28 083542" src="https://github.com/user-attachments/assets/015a5271-cf1e-48a2-a6d4-7b5d65db8ead" />


## 🛠️ Architecture

- `include/Process.hpp`: A data structure holding process telemetry (PID, Name, CPU%, Memory).
- `include/Parser.hpp` & `src/Parser.cpp`: Parses raw text from `/proc` files, safely handling missing files or permission denied errors.
- `src/main.cpp`: The core engine that iterates through `/proc`, sorts processes by highest CPU usage, and renders the terminal UI.
- `tests/`: GoogleTest suite.

## 💻 Build and Run

This project uses **CMake** for the build pipeline. You will need `g++` and `cmake` installed.

```bash
# 1. Create a build directory
mkdir build && cd build

# 2. Configure and compile the project
cmake ..
make

# 3. Run the process monitor (Press Ctrl+C to stop)
./monitor

# 4. Run the unit tests
ctest --output-on-failure
