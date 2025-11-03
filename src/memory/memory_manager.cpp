#include "pch.h"
#include "memory/memory_manager.h"

namespace rglz {
	namespace memory {
		StackAllocator MemoryManager::s_stack_allocator(nullptr, 0);
		StackAllocator MemoryManager::s_single_frame_allocator(nullptr, 0);
		
		void MemoryManager::startup(MemoryBlock& memory_block, const MemoryProfile& memory_profile) {
			std::size_t allocation_size = memory_profile.allocation_size;
			double stack_allocator_allocation_size_percent = memory_profile.stack_allocator_allocation_size_percent;
			double single_frame_allocator_allocation_size_percent = memory_profile.single_frame_allocator_allocation_size_percent;
			std::optional<std::size_t> stack_allocator_max_allocation_size = memory_profile.stack_allocator_max_allocation_size;
			std::optional<std::size_t> single_frame_allocator_max_allocation_size = memory_profile.single_frame_allocator_max_allocation_size;
			const double epsilon = 0.001;

			double sum_of_percentages = 
				stack_allocator_allocation_size_percent + 
				single_frame_allocator_allocation_size_percent;

			bool is_sum_of_percentages_100 = (
				std::fabs(sum_of_percentages - 100) <= epsilon
			);

			assert(is_sum_of_percentages_100);

			std::uint8_t* stack_allocator_data = memory_block.data();

			std::size_t stack_size = memory_block.size() / 100 * stack_allocator_allocation_size_percent;
			if (stack_allocator_max_allocation_size.has_value()) {
				stack_size = std::clamp(
					static_cast<std::size_t>(stack_size), 
					static_cast<std::size_t>(0),
					static_cast<std::size_t>(stack_allocator_max_allocation_size.value())
				);
			}

			std::uint8_t* single_frame_allocator_data = memory_block.data() + stack_size;

			std::size_t single_frame_size = memory_block.size() - (single_frame_allocator_data - memory_block.data());
			if (single_frame_allocator_max_allocation_size.has_value()) {
				single_frame_size = std::clamp(
					static_cast<std::size_t>(single_frame_size),
					static_cast<std::size_t>(0),
					static_cast<std::size_t>(single_frame_allocator_max_allocation_size.value())
				);
			}

			s_stack_allocator = StackAllocator(stack_allocator_data, stack_size);
			s_single_frame_allocator = StackAllocator(single_frame_allocator_data, single_frame_size);
			
			RGLZ_ENGINE_LOG_TRACE("Total Allocated Size: ", allocation_size, alignln, "Stack Allocator Allocated Size: ", stack_size, alignln, "Single Frame Allocator Allocated Size: ", single_frame_size);
		}

		void MemoryManager::update() {
			s_single_frame_allocator.clear();
		}

		void MemoryManager::shutdown() {
		}
	}
}
