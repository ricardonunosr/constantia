#include "memory.h"
#include <windows.h>

Arena* arena_alloc(uint64_t capacity)
{
  Arena* arena = (Arena*)VirtualAlloc(0,capacity, MEM_COMMIT | MEM_RESERVE, PAGE_READWRITE );
  arena->pos = sizeof(arena);
  arena->memory_base = arena + sizeof(arena);
  return arena;
}

void arena_release(Arena* arena)
{
  // TODO
}

void* arena_push(Arena* arena, uint64_t size)
{
  void* memory;
  uint64_t pos = arena->pos;
  memory = ((uint8_t*)arena->memory_base) + pos;
  arena->pos += size;
  return memory;
}

void* arena_push_zero(Arena*, uint64_t size)
{
  // TODO
  return nullptr;
}
