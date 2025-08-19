#include "pch.h"
#include "memory/memory_manager.h"

namespace rglz {
	namespace memory {
		StackAllocator MemoryManager::s_stack_allocator(nullptr, 0);
		StackAllocator MemoryManager::s_single_frame_allocator(nullptr, 0);
		
		void MemoryManager::startup(MemoryBlock& memory_block, const std::uint8_t stack_allocator_size_percentage, const std::uint8_t single_frame_allocator_size_percentage) {
			assert((stack_allocator_size_percentage + single_frame_allocator_size_percentage) == 100);
			std::uint8_t* stack_allocator_data = memory_block.data();
			std::size_t stack_allocator_size = memory_block.size() / 100 * stack_allocator_size_percentage;
			std::uint8_t* single_frame_allocator_data = memory_block.data() + stack_allocator_size;
			std::size_t single_frame_allocator_size = memory_block.size() - (single_frame_allocator_data - memory_block.data());

			s_stack_allocator = StackAllocator(stack_allocator_data, stack_allocator_size);
			s_single_frame_allocator = StackAllocator(single_frame_allocator_data, single_frame_allocator_size);
		}

		void MemoryManager::update() {
			s_single_frame_allocator.clear();
		}

		void MemoryManager::shutdown() {
		}
	}
}
