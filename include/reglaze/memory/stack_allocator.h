#ifndef RGLZ_MEMORY_STACK_ALLOCATOR_H
#define RGLZ_MEMORY_STACK_ALLOCATOR_H

#include "base.h"
#include <cstdint>

namespace rglz {
	namespace memory {
		class StackAllocator {
		public:
			class Marker {
			public:
				Marker(std::uint8_t* ptr);

				inline operator std::uint8_t*() const {
					return m_ptr;
				}

			private:
				std::uint8_t* m_ptr;

				friend class StackAllocator;
			};

		public:
			StackAllocator(std::uint8_t* memory, std::size_t size);
			
			template<typename T = std::uint8_t>
			StackAllocator::Marker allocate(std::size_t n) {
				return allocate(n*sizeof(T), alignof(T));
			}

			StackAllocator::Marker allocate(std::size_t size, std::size_t alignment);

			inline void reset_at_marker(Marker marker) {
				m_marker = marker;
			}

			inline void clear() {
				m_marker = Marker(m_memory);
			}

			inline Marker marker() const {
				return m_marker;
			}

			inline std::size_t size() const {
				return m_size;
			}

		private:
			std::uint8_t* m_memory;
			Marker m_marker;
			std::size_t m_size;
		};
	}
}

#endif	// RGLZ_MEMORY_STACK_ALLOCATOR_H
