#ifndef COLORS_H
#define COLORS_H

// --- Standard Terminal Colors ---
#define COLOR_RESET   "\033[0m"

// Metadata (PIDs, Timestamps)
#define COLOR_DIM     "\033[2m"
#define COLOR_CYAN    "\033[1;36m"
#define COLOR_BLUE    "\033[0;34m"

// Core Data (Syscall names)
#define COLOR_GREEN   "\033[0;32m"
#define COLOR_BOLD_GREEN "\033[1;32m"

// Results and Return Values
#define COLOR_YELLOW  "\033[0;33m"
#define COLOR_BOLD_YELLOW "\033[1;33m"

// Errors and Signals
#define COLOR_RED     "\033[0;31m"
#define COLOR_BOLD_RED "\033[1;31m"

// Highlights
#define COLOR_MAGENTA "\033[0;35m"

#define COLOR_GREY "\033[0;37m"
#define COLOR_ACK     "\033[1;36m"

#endif // COLORS_H
