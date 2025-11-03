#ifndef RGLZ_PREFERENCES_H
#define RGLZ_PREFERENCES_H

#include "base.h"
#include "core/attribute.h"
#include "containers/fixed_vector.h"
#include "containers/fixed_string.h"
#include <fstream>
#include <array>
#include <optional>
#include <string_view>

namespace rglz {
	// class PreferencesStream : public esr::Stream<std::ios> {
	// public:
	// 	static consteval std::string_view get_channel() {
	// 		return "Preferences";
	// 	}
	// };
	class Preferences {
	public:
		Preferences(std::string_view file_name, const FixedVector<Attribute, RGLZ_PREFERENCES_MAX_ATTRIBUTES>& default_attributes);

		void rebuild();

		Attribute* write_attribute(std::string_view attribute_name, bool value);

		Attribute* write_attribute(std::string_view attribute_name, double value);

		Attribute* write_attribute(std::string_view attribute_name, const char* value);

		Attribute* write_attribute(std::string_view attribute_name, std::string_view value);

		Attribute* write_attribute(std::string_view attribute_name);


		inline const Attribute* operator[] (std::string_view attribute_name) const {
			return m_current_attributes[attribute_name];
		}

		inline FixedHashMap<FixedString<RGLZ_PREFERENCES_ATTRIBUTE_NAME_MAX_LENGTH>, Attribute, RGLZ_PREFERENCES_MAX_ATTRIBUTES>::Iterator begin() {
			return m_current_attributes.begin();
		}

		inline FixedHashMap<FixedString<RGLZ_PREFERENCES_ATTRIBUTE_NAME_MAX_LENGTH>, Attribute, RGLZ_PREFERENCES_MAX_ATTRIBUTES>::Iterator end() {
			return m_current_attributes.end();
		}

		inline FixedHashMap<FixedString<RGLZ_PREFERENCES_ATTRIBUTE_NAME_MAX_LENGTH>, Attribute, RGLZ_PREFERENCES_MAX_ATTRIBUTES>::ConstIterator begin() const {
			return m_current_attributes.begin();
		}

		inline FixedHashMap<FixedString<RGLZ_PREFERENCES_ATTRIBUTE_NAME_MAX_LENGTH>, Attribute, RGLZ_PREFERENCES_MAX_ATTRIBUTES>::ConstIterator end() const {
			return m_current_attributes.end();
		}

		inline FixedHashMap<FixedString<RGLZ_PREFERENCES_ATTRIBUTE_NAME_MAX_LENGTH>, Attribute, RGLZ_PREFERENCES_MAX_ATTRIBUTES>::ConstIterator cbegin() const {
			return m_current_attributes.cbegin();
		}

		inline FixedHashMap<FixedString<RGLZ_PREFERENCES_ATTRIBUTE_NAME_MAX_LENGTH>, Attribute, RGLZ_PREFERENCES_MAX_ATTRIBUTES>::ConstIterator cend() const {
			return m_current_attributes.cend();
		}

	private:
		[[nodiscard]]
		std::optional<std::pair<FixedString<RGLZ_PREFERENCES_ATTRIBUTE_NAME_MAX_LENGTH>, FixedString<RGLZ_PREFERENCES_ATTRIBUTE_RVALUE_MAX_LENGTH>>> read_line(std::ifstream& file, std::string_view file_name, std::uint32_t& line_number);

		[[nodiscard]]
		std::optional<Attribute> parse_attribute(std::string_view attribute_name, std::string_view attribute_value_string);

		void write_attribute(Attribute& attribute, std::ofstream& file);

	private:
		bool m_rebuild;
		FixedString<RGLZ_FILE_PATH_MAX_LENGTH> m_file_name;
		FixedVector<Attribute, RGLZ_PREFERENCES_MAX_ATTRIBUTES> m_default_attributes;
		FixedHashMap<FixedString<RGLZ_PREFERENCES_ATTRIBUTE_NAME_MAX_LENGTH>, Attribute, RGLZ_PREFERENCES_MAX_ATTRIBUTES> m_current_attributes;
	};
}

#endif	// RGLZ_PREFERENCES_H
