#pragma once

#include <stdint.h>

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
