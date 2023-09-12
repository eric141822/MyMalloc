# Memory Allocator in C

A basic memory allocator written in C. Allocates memory through the `mmap()` and `munmap()` system calls.

It implements `malloc()`, `calloc()`, `realloc()` and `free()`, check the [manual](https://man7.org/linux/man-pages/man3/free.3.html) for details on the native implementation of these functions.

For more details on the `mmap()` system call, check the manual page (e.g. `man 2 mmap`) for more details.

## mmap() v.s. brk()/sbrk()

The actual `malloc()` in *libc* implements memory allocation through both the `brk()/sbrk()` and `mmap()` system calls interchangably. Checking `man 2 sbrk` on a mac book terminal: *"The brk and sbrk functions are historical curiosities left over from earlier days before the advent of virtual memory management."*.

In general, `mmap()` is safer than `brk()` (brk/sbrk are not thread-safe). Normally `mmap()` is used to allocate a larger chunk of memory, and it can be more efficient to use the `brk()` system call to implement this allocator due to it's less complex nature. For learning purposes and safety reasons, `mmap()` is used instead of `brk()` for the implementation.  