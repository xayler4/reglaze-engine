#ifndef RGLZ_MEMORY_MEMORY_BLOCK_H
#define RGLZ_MEMORY_MEMORY_BLOCK_H

#include "base.h"
#include <memory>
#include <cstdint>

namespace rglz {
	namespace memory {

		class MemoryBlock {
		public:
			MemoryBlock(std::size_t size);

			inline std::uint8_t* memory() {
				return m_memory.get();
			}

			inline std::size_t size() const {
				return m_size;
			}

		private:
			std::unique_ptr<std::uint8_t[]> m_memory;
			std::size_t m_size;
		};
	}
}

#endif	// RGLZ_MEMORY_MEMORY_BLOCK_H
