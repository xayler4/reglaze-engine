#ifndef RGLZ_MEMORY_MEMORY_BLOCK_H
#define RGLZ_MEMORY_MEMORY_BLOCK_H

#include "pch.h"

namespace rglz {
	namespace memory {

		class MemoryBlock {
		public:
			MemoryBlock(std::size_t size);
			MemoryBlock(MemoryBlock&& memory_block);
			~MemoryBlock();

			inline std::uint8_t* data() {
				return m_memory;
			}

			inline std::size_t size() const {
				return m_size;
			}

			inline bool is_bad_alloc() const {
				return m_bad_alloc;
			}

			MemoryBlock(const MemoryBlock& memory_block) = delete;

		private:
			std::uint8_t* m_memory;
			std::size_t m_size;
			bool m_bad_alloc;
		};
	}
}

#endif	// RGLZ_MEMORY_MEMORY_BLOCK_H
