#include "memory/stack_allocator.h"

namespace rglz {
	namespace memory {
		StackAllocator::Marker::Marker(std::uint8_t* ptr) 
			: m_ptr(ptr)
		{}

		StackAllocator::StackAllocator(std::uint8_t* memory, std::size_t size)
			: m_memory(memory)
			, m_marker(memory)
			, m_size(size) 
		{}

		StackAllocator::Marker StackAllocator::allocate(std::size_t size, std::size_t alignment) {
			std::size_t alloc_size = m_size - (m_marker - m_memory);
			std::cout << size << " "<< alignment << " " << m_size << " " << m_size - (m_marker - m_memory) << "\n";
			void* offsetted_memory = m_marker;
			uint8_t* ptr = static_cast<std::uint8_t*>(std::align(alignment, size, offsetted_memory, alloc_size));

			RGLZ_ENGINE_ALWAYS_ASSERT_MSG(ptr, "StackAllocator: Out of stack memory!");

			Marker previous_marker(ptr);
			m_marker = Marker(ptr + size);

			return previous_marker;
		}
	}	
}
