#pragma once

#include <stdint.h>
#include <stddef.h>

#define Bytes(n)      (n)
#define Kilobytes(n)  (n << 10)
#define Megabytes(n)  (n << 20)
#define Gigabytes(n)  (((U64)n) << 30)
#define Terabytes(n)  (((U64)n) << 40)

// Linear Allocator
// Credits to gingerBill and RyanFleury
struct Arena
{
  unsigned char* mem_base;
  size_t mem_length;
  size_t curr_offset;
};

Arena* arena_alloc(size_t capacity);
void arena_release(Arena* arena);

void* arena_push(Arena* arena, size_t size);
void* arena_push_align(Arena* arena, size_t size, size_t align);

#define ARRAY_PUSH(flat_array, count) &flat_array[count++];

