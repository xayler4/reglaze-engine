#ifndef RGLZ_MEMORY_MEMORY_MANAGER_H
#define RGLZ_MEMORY_MEMORY_MANAGER_H

#include "pch.h"
#include "memory/memory_block.h"
#include "memory/stack_allocator.h"

int main(int argc, const char** argv);

namespace rglz {
	class Engine;

	namespace memory {
		class MemoryManager {
		public:
			MemoryManager() = delete;

			static void startup(MemoryBlock& memory_block, const std::uint8_t stack_allocator_size_percentage, const std::uint8_t single_frame_allocator_size_percentage);
			static void update();
			static void shutdown();

			inline static StackAllocator& stack_allocator() {
				return s_stack_allocator;
			}

			inline static StackAllocator& single_frame_allocator() {
				return s_single_frame_allocator;
			}

		private:
			static StackAllocator s_stack_allocator;
			static StackAllocator s_single_frame_allocator;
		};
	}
}

#endif 	// RGLZ_MEMORY_MEMORY_MANAGER_H
