#include "core/logger.h"
#include "memory/memory_manager.h"

namespace rglz {
	LogBuffer::LogBuffer()
		: data(m_fixed_data)
		, size(RGLZ_LOGGER_LOG_PREALLOCATED_LENGTH)
	{
	}

	void LogBuffer::reset() {
		data = m_fixed_data;
		size = RGLZ_LOGGER_LOG_PREALLOCATED_LENGTH;
	}

	void LogBuffer::grow(std::size_t required_size_delta) {
		memory::StackAllocator& allocator = memory::MemoryManager::single_frame_allocator();

		std::size_t old_size = size;
		std::size_t new_size = 
			((old_size * 2 - old_size) > required_size_delta) 
				? old_size * 2 
				: old_size + required_size_delta;

		std::uint8_t* tmp_data = data;
		std::uint8_t* new_data = allocator.allocate<std::uint8_t>(new_size);

		std::memcpy(new_data, tmp_data, old_size);

		data = new_data;
		size = new_size;
	}
}
