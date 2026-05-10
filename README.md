# Linux System Call Tracer: Kernel-Level Execution Monitor

A custom, high-precision C utility designed to track, monitor, and log interactions between user-space programs and the Linux Kernel. It acts as a lightweight `strace` clone to intercept system calls in real-time.

## 🛠️ Tech Stack

* **Language:** C (Standard C99/C11)
* **Environment:** 64-bit Linux (Ubuntu)
* **Core APIs:** POSIX (`<sys/ptrace.h>`, `<sys/user.h>`, `<sys/wait.h>`)
* **Build Tool:** GNU Make (`Makefile`)

## ✨ Key Features

* **Dual Tracking Modes:** Spawn and track a new child process, or securely attach to an existing external process via its PID.
* **High-Precision Timing:** Utilizes `clock_gettime(CLOCK_MONOTONIC)` to calculate exact nanosecond-level execution times for every system call.
* **Name Translation:** Automatically translates raw system call IDs (e.g., `59`) into human-readable string names (e.g., `execve`).
* **Return Value Interception:** Captures the `rax` register on syscall exit to securely record the kernel operation's success or failure status.
* **Modular Architecture:** Cleanly separated codebase with dedicated files for command routing, file logging, dictionary mapping, and the core `ptrace` engine.

## 🚀 How to Run Locally

1. Clone the repository.
2. Compile the project using Make: `make clean && make`
3. Track a newly spawned command: `sudo ./main <command>` (e.g., `sudo ./main ls`)
4. Track an active background process (requires root): `sudo ./main -p <PID>`
5. View the formatted execution timeline in your terminal or check the automatically generated `syscall_log.log` file.
