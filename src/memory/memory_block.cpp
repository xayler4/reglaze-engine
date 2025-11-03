#include "pch.h"
#include "memory/memory_block.h"

namespace rglz {
	namespace memory {
		MemoryBlock::MemoryBlock(std::size_t size)
			: m_bad_alloc(false)
			, m_size(0)
		{
			try {
				m_memory = new std::uint8_t[size];

				m_size = size;
			}
			catch(const std::bad_alloc& e) {
				m_bad_alloc = true;
			}
		}

		MemoryBlock::MemoryBlock(MemoryBlock&& memory_block) 
			: m_size(memory_block.m_size)
			, m_bad_alloc(memory_block.m_bad_alloc)
		{
			memory_block.m_memory = nullptr;
		}

		MemoryBlock::~MemoryBlock() {
			if (m_memory != nullptr && !m_bad_alloc) {
				delete[] m_memory;
			}
		}

		void MemoryBlock::realloc(std::size_t size) {
			if (m_memory != nullptr && !m_bad_alloc) {
				delete[] m_memory;
			}

			m_bad_alloc = false;

			try {
				m_memory = new std::uint8_t[size];

				m_size = size;
			}
			catch(const std::bad_alloc& e) {
				m_bad_alloc = true;
			}
		}
	}
}
