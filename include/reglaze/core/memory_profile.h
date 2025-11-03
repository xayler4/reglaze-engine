#ifndef RGLZ_MEMORY_PROFILE_H
#define RGLZ_MEMORY_PROFILE_H

namespace rglz {
	struct MemoryProfile {
		std::size_t allocation_size = 32000;
		double stack_allocator_allocation_size_percent = 70;
		double single_frame_allocator_allocation_size_percent = 30;
		std::optional<std::size_t> stack_allocator_max_allocation_size;
		std::optional<std::size_t> single_frame_allocator_max_allocation_size;
	};
}

#endif
