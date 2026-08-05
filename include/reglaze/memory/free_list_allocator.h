#ifndef RGLZ_FREE_LIST_ALLOCATOR_H
#define RGLZ_FREE_LIST_ALLOCATOR_H

#include "base.h"
#include <mutex>

namespace rglz {
	namespace memory {
		class FreeListAllocator {
		public:
			struct Footer {
				size_t size_and_free;

				enum Flag {
					FreeBitmask = size_t{1} << (sizeof(size_t) * 8 - 1)
				};

				inline size_t size() const {
					return size_and_free & ~FreeBitmask;
				}

				inline bool is_free() const {
					return size_and_free & FreeBitmask;
				}

				inline void set_size(std::size_t size) {
					RGLZ_ENGINE_ASSERT((size & FreeBitmask) == 0);

					size_and_free = (size | (size_and_free & FreeBitmask));
				}

				inline void free() {
					size_and_free |= FreeBitmask;
				}

				inline void unfree() {
					size_and_free &= ~FreeBitmask;
				}
			};

			struct FreeBlock : public Footer {
				FreeBlock* prev;
				FreeBlock* next;
			};

		public:
			FreeListAllocator(std::uint8_t* memory, std::size_t size, const FixedString<32> name);

			void* allocate(size_t size);
			void* reallocate(void* user_ptr, size_t size);
			void deallocate(void* usert_ptr);

		private:
			FreeBlock* free_block_create(void* memory, std::size_t region_size);
			void free_block_merge(FreeBlock* free_block1, FreeBlock* free_block2);
			FreeBlock* free_block_rneighbour(FreeBlock* free_block);
			FreeBlock* free_block_lneighbour(FreeBlock* free_block);

			void insert_free_block(FreeBlock* node);
			void unlink_free_block(FreeBlock* node);

		private:
			static std::recursive_mutex s_mutex;
			const uint8_t* m_start_ptr;
			const uint8_t* m_end_ptr;
			FreeBlock* m_free_list;
			FixedString<> m_name;
		};
	}
}

#endif	// RGLZ_FREE_LIST_ALLOCATOR_H
