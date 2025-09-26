#ifndef RGLZ_CONTAINERS_FIXED_VECTOR_H
#define RGLZ_CONTAINERS_FIXED_VECTOR_H

#include "base.h"
#include "easer/easer.h"
#include <array>

namespace rglz {
	template<typename T, std::size_t UCapacity = 128>
	class FixedVector {
	public:
		using value_type = T;
		using size_type = std::size_t;
		using difference_type = std::ptrdiff_t;
		using reference = value_type&;
		using const_reference = const value_type&;
		using pointer = value_type*;
		using const_pointer = const value_type*;
		using iterator = std::array<T, UCapacity>::iterator;
		using const_iterator = std::array<T, UCapacity>::const_iterator;
		using reverse_iterator = std::reverse_iterator<iterator>;
		using const_reverse_iterator = std::reverse_iterator<const_iterator>;

	public:
		FixedVector()
			: m_size(0)
		{}

		FixedVector(std::initializer_list<T> list)
			: m_size(list.size())
		{
			RGLZ_ENGINE_ASSERT(m_size < UCapacity);

			std::copy(list.begin(), list.end(), m_data.begin());

		}

		template<std::size_t USize>
		FixedVector(const std::array<T, USize>& data)
			: m_size(data.size())
		{
			RGLZ_ENGINE_ASSERT(m_size < UCapacity);

			std::copy(data.begin(), data.end(), m_data.begin());
		}

		FixedVector(std::size_t size)
			: m_size(size) 
		{
			RGLZ_ENGINE_ASSERT(m_size < UCapacity);
		}

		void extend(const T& data) {
			RGLZ_ENGINE_ASSERT(m_size + 1 < UCapacity);

			m_data[m_size] = data;
			m_size ++;
		} 

		void extend(std::initializer_list<T> list) {
			RGLZ_ENGINE_ASSERT(m_size + list.size() < UCapacity);
			
			std::copy(list.begin(), list.end(), end());
			m_size += list.size();
		}

		template<std::size_t USize>
		void extend(const std::array<T, USize>& data) {
			RGLZ_ENGINE_ASSERT(m_size + data.size() < UCapacity);

			std::copy(data.begin(), data.end(), end());
			m_size += data.size();
		} 

		template<std::size_t USize>
		void extend(const std::array<T, USize>& data, std::size_t size) {
			RGLZ_ENGINE_ASSERT(m_size <= size && size >= (data.size() + m_size));

			std::copy(data.begin(), data.end(), end());
			m_size = size;
		} 

		void extend(std::size_t size) {
			m_size = size;

			RGLZ_ENGINE_ASSERT(m_size < size);
		}

		inline std::size_t size() const {
			return m_size;
		}

		inline std::size_t capacity() const {
			return UCapacity;
		}

		inline T* data() {
			return m_data.data();
		}

		inline T& operator[](std::size_t index) {
			RGLZ_ENGINE_ASSERT(index < m_size);

			return m_data[index];
		}

		inline const T& operator[](std::size_t index) const {
			RGLZ_ENGINE_ASSERT(index < m_size);

			return m_data[index];
		}

		inline iterator begin() {
			return m_data.begin(); 
		}
    	inline iterator end() {
			return m_data.begin() + m_size; 
		}

		inline const_iterator begin() const {
			return cbegin(); 
		}
    	inline const_iterator end() const {
			return cend(); 
		}

		inline const_iterator cbegin() const {
			return m_data.cbegin(); 
		}
    	inline const_iterator cend() const {
			return m_data.cbegin() + m_size; 
		}

		inline reverse_iterator rbegin() {
			return m_data.rbegin() + (UCapacity - m_size);
		}
    	inline reverse_iterator rend() {
			return m_data.rend();
		}

		inline const_reverse_iterator rbegin() const {
			return crbegin();
		}
    	inline const_reverse_iterator rend() const {
			return crend();
		}

		inline const_reverse_iterator crbegin() const {
			return m_data.crbegin() + (UCapacity - m_size);
		}
    	inline const_reverse_iterator crend() const {
			return m_data.crend();
		}

	private:
		std::array<T, UCapacity> m_data;
		std::size_t m_size;
	};
}

#endif	// RGLZ_CONTAINERS_FIXED_VECTOR_H
