#ifndef RGLZ_CONTAINERS_FIXED_STRING
#define RGLZ_CONTAINERS_FIXED_STRING

#include "base.h"
#include <string_view>
#include <cstring>

namespace rglz {
	template<std::size_t UMaxLength = 128>
	class FixedString {
	public:
		using Type = std::array<char, UMaxLength>;

	public:
		FixedString(std::string_view str) : m_length(str.length()){
			RGLZ_ENGINE_ASSERT(m_length < UMaxLength);

			std::memcpy(m_data.data(), str.data(), m_length);
		}

		FixedString(std::string_view str, std::size_t length) : m_length(length) {
			RGLZ_ENGINE_ASSERT(m_length < UMaxLength && m_length >= str.length());

			std::memcpy(m_data.data(), str.data(), m_length);
		}

		FixedString(std::string_view str1, std::string_view str2) : m_length(str1.length() + str2.length()){
			RGLZ_ENGINE_ASSERT(m_length < UMaxLength);

			std::memcpy(m_data.data(), str1.data(), str1.length());
			std::memcpy(m_data.data() + str1.length(), str2.data(), str2.length());
		}

		FixedString(std::size_t length) : m_length(length) {
			RGLZ_ENGINE_ASSERT(m_length < UMaxLength);
		}

		inline std::size_t length() const {
			return m_length;
		}

		inline std::size_t max_length() const {
			return UMaxLength;
		}

		inline char* data() {
			return m_data.data();
		}

		inline char& operator[](std::size_t index) {
			RGLZ_ENGINE_ASSERT(index < m_length);

			return m_data[index];
		}

		inline const char& operator[](std::size_t index) const {
			RGLZ_ENGINE_ASSERT(index < m_length);

			return m_data[index];
		}

		inline FixedString<UMaxLength>& operator += (std::string_view str) {
			RGLZ_ENGINE_ASSERT((m_length + str.length()) < UMaxLength);

			std::memcpy(m_data.data() + m_length, str.data(), str.length());

			return *this;
		}

		inline FixedString<UMaxLength> operator + (std::string_view str) {
			RGLZ_ENGINE_ASSERT((m_length + str.length()) < UMaxLength);

			return FixedString<UMaxLength>(std::string_view(m_data.data(), m_length), str);
		}

		inline FixedString<UMaxLength> operator = (std::string_view str) {
			return FixedString<UMaxLength>(str);
		}

		inline operator std::string_view() {
			return std::string_view(m_data.data(), m_length);
		}

		inline Type::iterator begin() {
			return m_data.begin(); 
		}
    	inline Type::iterator end() {
			return m_data.begin() + m_length; 
		}

		inline Type::const_iterator begin() const {
			return cbegin(); 
		}
    	inline Type::const_iterator end() const {
			return cend(); 
		}

		inline Type::const_iterator cbegin() const {
			return m_data.cbegin(); 
		}
    	inline Type::const_iterator cend() const {
			return m_data.cbegin() + m_length; 
		}

		inline Type::reverse_iterator rbegin() {
			return m_data.rbegin() + (UMaxLength - m_length);
		}
    	inline Type::reverse_iterator rend() {
			return m_data.rend();
		}

		inline Type::const_reverse_iterator rbegin() const {
			return crbegin();
		}
    	inline Type::const_reverse_iterator rend() const {
			return crend();
		}

		inline Type::const_reverse_iterator crbegin() const {
			return m_data.crbegin() + (UMaxLength - m_length);
		}
    	inline Type::const_reverse_iterator crend() const {
			return m_data.crend();
		}

	private:
		Type m_data;
		std::size_t m_length;
	};
}

#endif	// RGLZ_CONTAINERS_FIXED_STRING
