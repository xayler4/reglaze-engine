#ifndef RGLZ_MEMORY_MEMORY_MANAGER_H
#define RGLZ_MEMORY_MEMORY_MANAGER_H

#include "base.h"
#include "memory/memory_block.h"
#include "memory/stack_allocator.h"

int main(int argc, const char** argv);

namespace rglz {
	class Engine;

	namespace memory {
		class MemoryManager {
		public:
			MemoryManager() = delete;

			inline static StackAllocator& stack_allocator() {
				return s_stack_allocator;
			}

		private:
			static void startup(MemoryBlock& memory_block);
			static void shutdown();

		private:
			static StackAllocator s_stack_allocator;

			friend int ::main(int argc, const char** argv);
		};
	}
}

#endif 	// RGLZ_MEMORY_MEMORY_MANAGER_H
