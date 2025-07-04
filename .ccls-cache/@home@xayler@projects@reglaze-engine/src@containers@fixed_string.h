#ifndef RGLZ_CONTAINERS_FIXED_STRING
#define RGLZ_CONTAINERS_FIXED_STRING

#include "base.h"
#include <string_view>
#include <cstring>
#include <assert.h>

namespace rglz {
	namespace cnt {
		template<std::size_t ULength = 128>
		class FixedString {
		public:
			FixedString(std::string_view str) : m_length(str.length()){
				assert(m_length < ULength);

				std::memcpy(m_data.data(), str.data(), m_length);
			}

			FixedString(std::string_view str, std::size_t length) : m_length(length) {
				assert(m_length < ULength && m_length >= str.length());

				std::memcpy(m_data.data(), str.data(), m_length);
			}

			FixedString(std::string_view str1, std::string_view str2) : m_length(str1.length() + str2.length()){
				assert(m_length < ULength);

				std::memcpy(m_data.data(), str1.data(), str1.length());
				std::memcpy(m_data.data() + str1.length(), str2.data(), str2.length());
			}

			FixedString(std::size_t length) : m_length(length) {
				assert(m_length < ULength);
			}

			inline std::size_t length() const {
				return ULength;
			}

			inline char* data() {
				return m_data.data();
			}

			inline char& operator[](std::size_t index) {
				assert(index < m_length);

				return m_data[index];
			}

			inline const char& operator[](std::size_t index) const {
				assert(index < m_length);

				return m_data[index];
			}

			inline FixedString<ULength>& operator += (std::string_view str) {
				assert((m_length + str.length()) < ULength);

				std::memcpy(m_data.data() + m_length, str.data(), str.length());

				return *this;
			}

			inline FixedString<ULength> operator + (std::string_view str) {
				assert((m_length + str.length()) < ULength);

				return FixedString<ULength>(std::string_view(m_data.data(), m_length), str);
			}

			inline FixedString<ULength> operator = (std::string_view str) {
				return FixedString<ULength>(str);
			}

			inline operator std::string_view() const {
				return std::string_view(m_data.data(), m_length);
			}

		private:
			std::array<char, ULength> m_data;
			std::size_t m_length;
		};
	} 
}

#endif	// RGLZ_CONTAINERS_FIXED_STRING
