#include "memory/memory_manager.h"

namespace rglz {
	namespace memory {
		StackAllocator MemoryManager::s_stack_allocator(nullptr, 0);
		
		void MemoryManager::startup(MemoryBlock& memory_block) {
			s_stack_allocator = StackAllocator(memory_block.memory(), memory_block.size());
		}

		void MemoryManager::shutdown() {
		}
	}
}
