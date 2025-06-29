#include "memory/memory_block.h"

namespace rglz {
	namespace memory {
		MemoryBlock::MemoryBlock(std::size_t size) 
			: m_memory(new std::uint8_t[size])
			, m_size(size) 
		{}
	}
}
