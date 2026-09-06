#include "memory/memory_manager.h"

#define STB_IMAGE_IMPLEMENTATION

#define STBI_MALLOC(s) rglz::memory::MemoryManager::free_list_allocator().allocate(s)
#define STBI_REALLOC(p, s) rglz::memory::MemoryManager::free_list_allocator().reallocate(p, s)
#define STBI_FREE(p) rglz::memory::MemoryManager::free_list_allocator().deallocate(p)

#include <external/stb_image.h>
