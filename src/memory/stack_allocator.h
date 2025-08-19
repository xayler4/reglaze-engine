#ifndef RGLZ_MEMORY_STACK_ALLOCATOR_H
#define RGLZ_MEMORY_STACK_ALLOCATOR_H

#include "pch.h"

namespace rglz {
	namespace memory {
		class StackAllocator {
		public:
			class Marker {
			public:
				inline operator std::uint8_t*() const {
					return m_ptr;
				}

			private:
				Marker(std::uint8_t* ptr) 
					: m_ptr(ptr) 
				{}

			private:
				std::uint8_t* m_ptr;

				friend class StackAllocator;
			};

		public:
			StackAllocator(std::uint8_t* memory, std::size_t size) 
				: m_memory(memory)
				, m_marker(memory)
				, m_size(size) 
			{}
			
			template<typename T = std::uint8_t>
			T* allocate(std::size_t n) {
				RGLZ_ENGINE_ASSERT((m_marker + n) - m_memory <= m_size);
				Marker previous_marker(m_marker);
				m_marker = Marker(previous_marker + n);

				return previous_marker;
			}

			void reset_at_marker(Marker marker) {
				m_marker = marker;
			}

			void clear() {
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
