#pragma once

#include <stdint.h>

// Stack arena
struct Arena
{
    uint64_t pos;
    void* memory_base;
};

Arena* arena_alloc(uint64_t capacity);
void arena_release(Arena* arena);

void* arena_push(Arena* arena, uint64_t size);
void* arena_push_zero(Arena* arena, uint64_t size);

void arena_pop(Arena* arena, uint64_t size);
void arena_set_pos_back(Arena* arena, uint64_t size);
void arena_clear(Arena* arena);
