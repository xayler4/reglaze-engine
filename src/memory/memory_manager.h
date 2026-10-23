#ifndef RGLZ_MEMORY_MEMORY_MANAGER_H
#define RGLZ_MEMORY_MEMORY_MANAGER_H

#include "pch.h"
#include "core/memory_profile.h"
#include "memory/memory_block.h"
#include "memory/stack_allocator.h"
#include "memory/free_list_allocator.h"

int main(int argc, const char** argv);

namespace rglz {
	class Engine;

	namespace memory {
		class MemoryManager {
		public:
			MemoryManager() = delete;

			static void startup(MemoryBlock& memory_block, const MemoryProfile& memory_profile, std::string_view name);
			static void update();
			static void shutdown();

			inline static StackAllocator& core_stack_allocator() {
				return s_core_allocator;
			}

			inline static StackAllocator& scene_stack_allocator() {
				return s_scene_stack_allocator;
			}

			inline static StackAllocator& single_frame_allocator() {
				return s_single_frame_allocator;
			}

			inline static FreeListAllocator& free_list_allocator() {
				return s_free_list_allocator;
			}

		private:
			static StackAllocator s_core_allocator;
			static StackAllocator s_single_frame_allocator;
			static StackAllocator s_scene_stack_allocator;
			static FreeListAllocator s_free_list_allocator;
		};
	}
}

#endif 	// RGLZ_MEMORY_MEMORY_MANAGER_H
