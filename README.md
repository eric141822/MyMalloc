# Memory Allocator in C

A basic memory allocator written in C. Allocates memory through the `mmap()` and `munmap()` system calls.

It implements `malloc()`, `calloc()`, and `free()`, check the [manual](https://man7.org/linux/man-pages/man3/free.3.html) for details.

For more details, check the manual page (e.g. `man 2 mmap`) for more details.

## Details

The actual `malloc()` in *libc* implements memory allocation through both the `brk()/sbrk()` and `mmap()` system calls interchangably. Checking `man 2 sbrk` on a mac book terminal: *"The brk and sbrk functions are historical curiosities left over from earlier days before the advent of virtual memory management."*.

Generally, `mmap()` is used to allocate a larger chunk of memory and it can be more efficient with `brk()` to implement this allocator. Here, `mmap()` is used both because of learning purporses, and the fact that it is in general, safer than `brk()` (brk/sbrk are not thread-safe).