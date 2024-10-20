# Memory Allocator in C

A basic memory allocator written in C. Allocates memory through the `sbrk()' system calls.

It implements `malloc()`, `calloc()`, `realloc()` and `free()`, check the [manual](https://man7.org/linux/man-pages/man3/free.3.html) for details on the native implementation of these functions.

## mmap() v.s. brk()/sbrk()

The actual `malloc()` in *libc* implements memory allocation through both the `brk()/sbrk()` and `mmap()` system calls interchangably. Checking `man 2 sbrk` on a mac book terminal: *"The brk and sbrk functions are historical curiosities left over from earlier days before the advent of virtual memory management."*.

In general, `mmap()` is safer than `sbrk()` (brk/sbrk are not thread-safe). Normally `mmap()` is used to allocate a larger chunk of memory, and it can be more efficient to use the `sbrk()` system call to implement this allocator due to it's less complex nature. For simplicity, this allocator uses `sbrk()`.

## Thread safety

While this allocator attempts to be thread-safe with a mutex, it is not guaranteed to be so. This is due to the fact that the `sbrk()` system call itself is not thread-safe. So a thread can call `sbrk()` and change the heap size while another thread is in the middle of a `mymalloc()` call. This can lead to memory corruption.

This allocator is written for fun only and is far from "correct" and "perfect"!
