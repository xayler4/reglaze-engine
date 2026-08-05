#include "pch.h"
#include "memory/memory_manager.h"

namespace rglz {
	namespace memory {
		StackAllocator MemoryManager::s_core_allocator(nullptr, 0, "Core Allocator");
		StackAllocator MemoryManager::s_scene_stack_allocator(nullptr, 0, "Scene Stack Allocator");
		StackAllocator MemoryManager::s_single_frame_allocator(nullptr, 0, "Single Frame Allocator");
		FreeListAllocator MemoryManager::s_free_list_allocator(nullptr, 0, "Free List Allocator");
		
		void MemoryManager::startup(MemoryBlock& memory_block, const MemoryProfile& memory_profile, std::string_view name) {
			std::size_t allocation_size = memory_profile.allocation_size;
			double stack_allocator_allocation_size_percent = memory_profile.core_stack_allocator_allocation_size_percent;
			double scene_stack_allocator_allocation_size_percent = memory_profile.scene_stack_allocator_allocation_size_percent;
			double free_list_allocator_allocation_size_percent = memory_profile.free_list_allocator_allocation_size_percent;
			double single_frame_allocator_allocation_size_percent = memory_profile.single_frame_allocator_allocation_size_percent;

			std::optional<std::size_t> stack_allocator_max_allocation_size = memory_profile.stack_allocator_max_allocation_size;
			std::optional<std::size_t> scene_stack_allocator_max_allocation_size = memory_profile.scene_stack_allocator_max_allocation_size;
			std::optional<std::size_t> free_list_allocator_max_allocation_size = memory_profile.free_list_allocator_max_allocation_size;
			std::optional<std::size_t> single_frame_allocator_max_allocation_size = memory_profile.single_frame_allocator_max_allocation_size;
			const float epsilon = 0.001;

			double sum_of_percentages = 
				stack_allocator_allocation_size_percent + 
				scene_stack_allocator_allocation_size_percent +
				free_list_allocator_allocation_size_percent +
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

			std::uint8_t* scene_stack_allocator_data = stack_allocator_data + stack_size;
			std::size_t scene_stack_size = memory_block.size() / 100 * scene_stack_allocator_allocation_size_percent;
			if (scene_stack_allocator_max_allocation_size.has_value()) {
				scene_stack_size = std::clamp(
					static_cast<std::size_t>(scene_stack_size), 
					static_cast<std::size_t>(0),
					static_cast<std::size_t>(scene_stack_allocator_max_allocation_size.value())
				);
			}

			std::uint8_t* single_frame_allocator_data = scene_stack_allocator_data + scene_stack_size;
			std::size_t single_frame_size = memory_block.size() / 100 * single_frame_allocator_allocation_size_percent;
			if (single_frame_allocator_max_allocation_size.has_value()) {
				single_frame_size = std::clamp(
					static_cast<std::size_t>(single_frame_size),
					static_cast<std::size_t>(0),
					static_cast<std::size_t>(single_frame_allocator_max_allocation_size.value())
				);
			}

			std::uint8_t* free_list_allocator_data = single_frame_allocator_data + single_frame_size;
			std::size_t free_list_size = memory_block.size() - (free_list_allocator_data - memory_block.data());
			if (free_list_allocator_max_allocation_size.has_value()) {
				free_list_size = std::clamp(
					static_cast<std::size_t>(free_list_size),
					static_cast<std::size_t>(0),
					static_cast<std::size_t>(free_list_allocator_max_allocation_size.value())
				);
			}

			s_core_allocator = StackAllocator(stack_allocator_data, stack_size, "Core Allocator");
			s_scene_stack_allocator = StackAllocator(scene_stack_allocator_data, scene_stack_size, "Scene Stack Allocator");
			s_single_frame_allocator = StackAllocator(single_frame_allocator_data, single_frame_size, "Single Frame Allocator");
			s_free_list_allocator = FreeListAllocator(free_list_allocator_data, free_list_size, "Free List Allocator");
			
			RGLZ_ENGINE_LOG_TRACE(name, " - Total allocated size: ", allocation_size, alignln, name, " - Core stack allocator allocated size: ", stack_size, alignln, name, " - Scene stack allocator allocated size: ", scene_stack_size, alignln, name, name, " - Free list allocator allocated size: ", free_list_size, alignln, " - Single frame allocator allocated size: ", single_frame_size);
		}

		void MemoryManager::update() {
			s_single_frame_allocator.clear();
		}

		void MemoryManager::shutdown() {
		}
	}
}
