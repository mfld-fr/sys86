// SYS86 project
// Source configuration

// Kernel code & data segments
#define KERNEL_CS 0x1000
#define KERNEL_DS 0x1000

// Interrupt stack size
#define INT_STACK_SIZE 256

// Serial I/O queues size
// Must be 2^N
#define SERIAL_QUEUE_SIZE 16

// Enable the timed traces
// #define CONFIG_TRACE

// Enable the heap dump
// #define HEAP_DEBUG
