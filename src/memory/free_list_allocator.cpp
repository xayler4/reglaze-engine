#include "pch.h"
#include "memory/free_list_allocator.h"

namespace rglz {
	namespace memory {
		std::recursive_mutex FreeListAllocator::s_mutex;

		static const uint8_t alignment = alignof(std::max_align_t);

		static constexpr std::size_t align_up(std::size_t size, std::size_t alignment) {
			return (size + alignment - 1) & ~(alignment - 1);
		}

		static constexpr std::size_t kHeaderSize = align_up(sizeof(FreeListAllocator::FreeBlock), alignment);
		static constexpr std::size_t kFooterSize = align_up(sizeof(FreeListAllocator::Footer), alignment);

		FreeListAllocator::FreeListAllocator(std::uint8_t* memory, std::size_t size, const FixedString<32> name)
			: m_start_ptr(memory)
			, m_end_ptr(memory + size)
			, m_free_list(nullptr)
		{
			m_name += "Free List Allocator \"";
			m_name += name;
			m_name += "\": Out of heap memory!";

			uintptr_t mem_addr = reinterpret_cast<uintptr_t>(memory);
			m_start_ptr = reinterpret_cast<uint8_t*>((mem_addr + alignment - 1) & ~(alignment - 1));

			if (memory != nullptr && m_end_ptr > m_start_ptr) {
				m_free_list = free_block_create(const_cast<uint8_t*>(m_start_ptr), m_end_ptr - m_start_ptr);
			}
		}

		void* FreeListAllocator::allocate(size_t size) {
			std::lock_guard<std::recursive_mutex> lock(s_mutex);
			std::size_t aligned_payload_size = align_up(size, alignment);
			FreeBlock* free_block = m_free_list;

			while (free_block != nullptr) {
				if (free_block->size() >= aligned_payload_size) {
					std::size_t free_payload_size = free_block->size();
					std::size_t remaining_bytes = free_payload_size - aligned_payload_size;

					unlink_free_block(free_block);

					if (remaining_bytes >= kHeaderSize + alignment + kFooterSize) {
						uint8_t* new_block_addr = reinterpret_cast<uint8_t*>(free_block) 
												  + kHeaderSize 
												  + aligned_payload_size 
												  + kFooterSize;

						FreeBlock* new_free_block = reinterpret_cast<FreeBlock*>(new_block_addr);
						std::size_t new_payload_size = remaining_bytes - kHeaderSize - kFooterSize;
						
						new_free_block->set_size(new_payload_size);
						new_free_block->free();
						
						Footer* new_footer = reinterpret_cast<Footer*>(new_block_addr + kHeaderSize + new_payload_size);
						new_footer->size_and_free = new_free_block->size_and_free;

						insert_free_block(new_free_block);

						free_block->set_size(aligned_payload_size);
					}

					free_block->unfree();
					uint8_t* user_ptr = reinterpret_cast<uint8_t*>(free_block) + kHeaderSize;
					
					Footer* footer = reinterpret_cast<Footer*>(user_ptr + free_block->size());
					footer->size_and_free = free_block->size_and_free;

					return user_ptr;
				}
				free_block = free_block->next;
			}
			return nullptr;
		}

		void* FreeListAllocator::reallocate(void* user_ptr, size_t size) {
			std::lock_guard<std::recursive_mutex> lock(s_mutex);
			if (user_ptr == nullptr) {
				return allocate(size);
			}

			if (size == 0) {
				deallocate(user_ptr);
				return nullptr;
			}

			size = (size + kFooterSize + alignment - 1) & ~(alignment - 1);
			size -= kFooterSize;

			FreeBlock* block = reinterpret_cast<FreeBlock*>(static_cast<uint8_t*>(user_ptr) - kHeaderSize);
			if (size < block->size()) {
				return user_ptr;
			}

			FreeBlock* rneighbour = free_block_rneighbour(block);
			if (rneighbour != nullptr) {
				if (rneighbour->is_free() && ((reinterpret_cast<uint8_t*>(rneighbour) + rneighbour->size()) - static_cast<uint8_t*>(user_ptr)) >= size) {
					free_block_merge(block, rneighbour);
					return user_ptr;
				}
			}

			void* out_ptr = allocate(size);
			if (out_ptr != nullptr) {
				memcpy(out_ptr, user_ptr, block->size());
				deallocate(user_ptr);
			}
			return out_ptr;

		}

	void FreeListAllocator::deallocate(void* user_ptr) {
		std::lock_guard<std::recursive_mutex> lock(s_mutex);
		if (user_ptr == nullptr) return;

		FreeBlock* block = reinterpret_cast<FreeBlock*>(static_cast<uint8_t*>(user_ptr) - kHeaderSize);
		block->free();

		Footer* footer = reinterpret_cast<Footer*>(static_cast<uint8_t*>(user_ptr) + block->size());
		footer->size_and_free = block->size_and_free;

		FreeBlock* rneighbour = free_block_rneighbour(block);
		if (rneighbour != nullptr && rneighbour->is_free()) {
			unlink_free_block(rneighbour);
			std::size_t merged_payload_size = block->size() + kFooterSize + kHeaderSize + rneighbour->size();
			block->set_size(merged_payload_size);
			
			Footer* merged_footer = reinterpret_cast<Footer*>(reinterpret_cast<uint8_t*>(block) + kHeaderSize + merged_payload_size);
			merged_footer->size_and_free = block->size_and_free;
		}

		FreeBlock* lneighbour = free_block_lneighbour(block);
		if (lneighbour != nullptr && lneighbour->is_free()) {
			unlink_free_block(lneighbour);
			std::size_t merged_payload_size = lneighbour->size() + kFooterSize + kHeaderSize + block->size();
			lneighbour->set_size(merged_payload_size);

			Footer* merged_footer = reinterpret_cast<Footer*>(reinterpret_cast<uint8_t*>(lneighbour) + kHeaderSize + merged_payload_size);
			merged_footer->size_and_free = lneighbour->size_and_free;

			block = lneighbour;
		}

		insert_free_block(block);
	}

	void FreeListAllocator::unlink_free_block(FreeBlock* node) {
		if (node->prev != nullptr) {
			node->prev->next = node->next;
		} else {
			m_free_list = node->next;
		}
		if (node->next != nullptr) {
			node->next->prev = node->prev;
		}
		node->next = nullptr;
		node->prev = nullptr;
	}

	void FreeListAllocator::insert_free_block(FreeBlock* node) {
		node->next = m_free_list;
		node->prev = nullptr;
		if (m_free_list != nullptr) {
			m_free_list->prev = node;
		}
		m_free_list = node;
	}

	FreeListAllocator::FreeBlock* FreeListAllocator::free_block_rneighbour(FreeBlock* block) {
		uint8_t* rneighbour_addr = reinterpret_cast<uint8_t*>(block) + kHeaderSize + block->size() + kFooterSize;
		if (rneighbour_addr >= m_end_ptr) return nullptr;
		return reinterpret_cast<FreeBlock*>(rneighbour_addr);
	}

	FreeListAllocator::FreeBlock* FreeListAllocator::free_block_lneighbour(FreeBlock* block) {
		uint8_t* block_addr = reinterpret_cast<uint8_t*>(block);
		if (block_addr <= m_start_ptr) return nullptr;

		Footer* lfooter = reinterpret_cast<Footer*>(block_addr - kFooterSize);
		uint8_t* lblock_addr = block_addr - kFooterSize - lfooter->size() - kHeaderSize;
		return reinterpret_cast<FreeBlock*>(lblock_addr);
	}

		FreeListAllocator::FreeBlock* FreeListAllocator::free_block_create(void* memory, std::size_t region_size) {
			RGLZ_ENGINE_ASSERT(region_size >= kHeaderSize + alignment + kFooterSize);

			std::size_t free_size = region_size - kHeaderSize - kFooterSize;
			void* offsetted_memory = static_cast<std::uint8_t*>(memory) + kHeaderSize;
			
			uint8_t* user_ptr = static_cast<std::uint8_t*>(std::align(alignment, 1, offsetted_memory, free_size));
			RGLZ_ENGINE_ASSERT(user_ptr != nullptr);

			FreeBlock* out_free_block = reinterpret_cast<FreeBlock*>(user_ptr - kHeaderSize);
			out_free_block->next = nullptr;
			out_free_block->prev = nullptr;
			out_free_block->set_size(region_size - (user_ptr - static_cast<std::uint8_t*>(memory)) - kFooterSize);
			out_free_block->free();

			Footer* footer = reinterpret_cast<Footer*>(user_ptr + out_free_block->size());
			footer->size_and_free = out_free_block->size_and_free;

			return out_free_block;
		}


		void FreeListAllocator::free_block_merge(FreeBlock* free_block1, FreeBlock* free_block2) {
			uint8_t* user_ptr1 = reinterpret_cast<uint8_t*>(free_block1) + kHeaderSize;
			uint8_t* user_ptr2 = reinterpret_cast<uint8_t*>(free_block2) + kHeaderSize;

			if (free_block2->prev != nullptr) {
				free_block2->prev->next = free_block2->next;
			} else {
				m_free_list = free_block2->next;
			}
			
			if (free_block2->next != nullptr) {
				free_block2->next->prev = free_block2->prev;
			}

			free_block1->set_size((user_ptr2 + free_block2->size()) - user_ptr1);

			Footer* footer = reinterpret_cast<Footer*>(user_ptr1 + free_block1->size());
			footer->size_and_free = free_block1->size_and_free;
		}

	}
}
