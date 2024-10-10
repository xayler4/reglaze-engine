#include "memory/memory_block.h"

namespace rglz {
	namespace memory {
		MemoryBlock::MemoryBlock(std::size_t size) 
			: m_memory(static_cast<std::uint8_t*> (::operator new(size)))
			, m_size(size) 
		{}
	}
}
