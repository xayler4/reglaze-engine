#ifndef RGLZ_FIXED_HASH_MAP_H
#define RGLZ_FIXED_HASH_MAP_H

#include "base.h"
#include "containers/fixed_string.h"
#include <string_view>

namespace rglz {
	std::size_t to_hash(std::string_view str);

	template<typename TKey, std::size_t UCapacity>
	struct Hash {
		static std::size_t from(const TKey& key) {
			return rglz::to_hash(key) % UCapacity;
		}
	};

	template<typename TKey, typename TValue, std::size_t UCapacity = 128, typename THash = Hash<TKey, UCapacity>>
	class FixedHashMap {
	public:

		struct Bucket {
			TKey key;
			TValue value;
		};

		using value_type = Bucket;
		using size_type = std::size_t;
		using difference_type = std::ptrdiff_t;
		using reference = value_type&;
		using const_reference = const value_type&;
		using pointer = value_type*;
		using const_pointer = const value_type*;

		class Iterator {
		public:
			Iterator(std::optional<Bucket>* current, std::optional<Bucket>* end) 
				: m_ptr(current)
				, m_end(end)
			{
				advance_to_valid();
			}

			void advance_to_valid() {
				while (m_ptr != m_end) {
 					if (m_ptr->has_value()) {
						return;
					}
					else {
						++m_ptr;
					}
				}
			}
	
	        reference operator*() const {
	            return m_ptr->value();
	        }
	
	        pointer operator->() const {
	            return &m_ptr->value();
	        }
	
	        Iterator& operator++() {
	            ++m_ptr;
	            advance_to_valid();
	            return *this;
	        }
	
	        bool operator==(const Iterator& it) const {
	            return m_ptr == it.m_ptr;
	        }
	
	        bool operator!=(const Iterator& it) const {
	            return m_ptr != it.m_ptr;
	        }
	
		private:
			std::optional<Bucket>* m_ptr; 
			std::optional<Bucket>* m_end; 
		};

		class ConstIterator {
		public:
			ConstIterator(const std::optional<Bucket>* current, const std::optional<Bucket>* end) 
				: m_ptr(current)
				, m_end(end)
			{
				advance_to_valid();
			}

			void advance_to_valid() {
				while (m_ptr != m_end) {
 					if (m_ptr->has_value()) {
						return;
					}
					else {
						++m_ptr;
					}
				}
			}
	
	        const reference operator*() const {
	            return m_ptr->value();
	        }
	
	        const pointer operator->() const {
	            return &m_ptr->value();
	        }
	
	        ConstIterator& operator++() {
	            ++m_ptr;
	            advance_to_valid();
	            return *this;
	        }
	
	        bool operator==(const ConstIterator& it) const {
	            return m_ptr == it.m_ptr;
	        }
	
	        bool operator!=(const ConstIterator& it) const {
	            return (m_ptr != it.m_ptr);
	        }
	
		private:
			const std::optional<Bucket>* m_ptr; 
			const std::optional<Bucket>* m_end; 
		};

		using iterator = Iterator;
		using const_iterator = ConstIterator;

	public:
		FixedHashMap()
			: m_size(0)
		{}

		void insert(const TKey& key, const TValue& value) {
			RGLZ_ASSERT(m_size != UCapacity);

			std::size_t initial_index = THash::from(key);
			for (std::size_t i = initial_index; i < UCapacity; ++i) {
				if (!m_data[i].has_value()) {
					m_data[i] = std::move(Bucket{std::move(key), std::move(value)});
					++m_size;
					return;
				}	
			}

			for (std::size_t i = initial_index; i-- != 0; ) {
				if (!m_data[i].has_value()) {
					m_data[i] = std::move(Bucket{std::move(key), std::move(value)});
					++m_size;
					return;
				}	
			}
		}

		bool remove(const TKey& key) {
			std::size_t initial_index = THash::from(key);

			for (std::size_t i = initial_index; i < UCapacity; ++i) {
				if (m_data[i].has_value()) {
					if (m_data[i].value().key == key) {
						m_data[i].reset();
						--m_size;
						return true;
					}
				}	
			}

			for (std::size_t i = initial_index; i-- != 0; ) {
				if (m_data[i].has_value()) {
					if (m_data[i].value().key == key) {
						m_data[i].reset();
						--m_size;
						return true;
					}
				}	
			}

			return false;
		}

		inline const TValue* operator[] (const TKey& key) const {
			std::size_t initial_index = THash::from(key);

			for (std::size_t i = initial_index; i < UCapacity; ++i) {
				if (m_data[i].has_value()) {
					const auto& entry = m_data[i].value();
					if (entry.key == key) {
						return &entry.value;
					}
				}	
			}

			for (std::size_t i = initial_index; i-- != 0; ) {
				if (m_data[i].has_value()) {
					const auto& entry = m_data[i].value();
					if (entry.key == key) {
						return &entry.value;
					}
				}	
			}

			return nullptr;
		}

		inline TValue* operator[] (const TKey& key) {
			std::size_t initial_index = THash::from(key);

			for (std::size_t i = initial_index; i < UCapacity; ++i) {
				if (m_data[i].has_value()) {
					auto& entry = m_data[i].value();
					if (entry.key == key) {
						return &entry.value;
					}
				}	
			}

			for (std::size_t i = initial_index; i-- != 0;) {
				if (m_data[i].has_value()) {
					auto& entry = m_data[i].value();
					if (entry.key == key) {
						return &entry.value;
					}
				}	
			}
			return nullptr;
		}

		inline std::size_t size() const {
			return m_size;
		}

		inline std::size_t capacity() const {
			return UCapacity;
		}

		inline std::optional<Bucket>* data() const {
			return m_data.data();
		}

		inline iterator begin() {
			return Iterator(m_data.data(), m_data.data() + UCapacity);
		}

		inline iterator end() {
			return Iterator(m_data.data() + UCapacity, m_data.data() + UCapacity);
		}

		inline const_iterator begin() const {
			return ConstIterator(m_data.data(), m_data.data() + UCapacity);
		}

		inline const_iterator end() const {
			return ConstIterator(m_data.data() + UCapacity, m_data.data() + UCapacity);
		}

	private:
		std::array<std::optional<Bucket>, UCapacity> m_data;
		std::size_t m_size;
	};
}

#endif	// RGLZ_FIXED_HASH_MAP_H
