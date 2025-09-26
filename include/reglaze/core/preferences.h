#ifndef RGLZ_PREFERENCES_H
#define RGLZ_PREFERENCES_H

#include "base.h"
#include "core/engine.h"
#include "containers/fixed_vector.h"
#include "containers/fixed_string.h"
#include <fstream>
#include <array>
#include <optional>
#include <string_view>
#include <variant>

namespace rglz {
	// class PreferencesStream : public esr::Stream<std::ios> {
	// public:
	// 	static consteval std::string_view get_channel() {
	// 		return "Preferences";
	// 	}
	// };

	enum class AttributeType {
		Boolean,
		Double,
		String,
		Null
	};
	
	class Attribute {
	public:
		using Value = std::variant<bool, double, rglz::FixedString<RGLZ_PREFERENCES_ATTRIBUTE_RVALUE_MAX_LENGTH>, std::nullptr_t>;

	public:
		Attribute();
		explicit Attribute(std::string_view name, bool value);
		explicit Attribute(std::string_view name, double value);
		explicit Attribute(std::string_view name, const char* value);
		explicit Attribute(std::string_view name, std::string_view value);
		Attribute(std::string_view name);

		inline AttributeType type() const {
			if (std::holds_alternative<bool>(m_value)) {
				return AttributeType::Boolean;
			}
			else if (std::holds_alternative<double>(m_value)) {
				return AttributeType::Double;
			}
			else if (std::holds_alternative<rglz::FixedString<RGLZ_PREFERENCES_ATTRIBUTE_RVALUE_MAX_LENGTH>>(m_value)) {
				return AttributeType::String;
			}
			else {
				return AttributeType::Null;
			}
		}

		inline std::string_view name() const {
			return m_name;
		}

		inline std::optional<bool> boolean_value() const {
			if (std::holds_alternative<bool>(m_value)) {
				return std::get<bool>(m_value);
			}
			return {};
		}

		inline std::optional<double> double_value() const {
			if (std::holds_alternative<double>(m_value)) {
				return std::get<double>(m_value);
			}
			return {};
		}

		inline std::optional<std::string_view> string_value() const {
			if (std::holds_alternative<FixedString<RGLZ_PREFERENCES_ATTRIBUTE_RVALUE_MAX_LENGTH>>(m_value)) {
				return std::get<FixedString<RGLZ_PREFERENCES_ATTRIBUTE_RVALUE_MAX_LENGTH>>(m_value);
			}
			return {};
		}

		inline bool is_null() const {
			return std::holds_alternative<std::nullptr_t>(m_value);
		}
	private:
		inline bool is_string_valid(std::string_view str) const {
			for (char c : str) {
				if (!(isalnum(c) || isspace(c) || c == '_' || c == '-')) {
					return false;
				}
			}

			return true;
		}

	private:
		FixedString<RGLZ_PREFERENCES_ATTRIBUTE_NAME_MAX_LENGTH> m_name;
		Value m_value;
	};

	class Preferences {
	public:
		Preferences(std::string_view file_name, FixedVector<Attribute> default_attributes);

	private:
		[[nodiscard]]
		std::optional<std::pair<FixedString<RGLZ_PREFERENCES_ATTRIBUTE_NAME_MAX_LENGTH>, FixedString<RGLZ_PREFERENCES_ATTRIBUTE_RVALUE_MAX_LENGTH>>> read_line(std::ifstream& file, std::string_view file_name, std::uint32_t& line_number);

		[[nodiscard]]
		std::optional<Attribute> parse_attribute(std::string_view attribute_name, std::string_view attribute_value_string);

		void write_attribute(Attribute& attribute, std::ofstream& file);

	private:
		FixedVector<Attribute> m_default_attributes;
		FixedHashMap<FixedString<RGLZ_PREFERENCES_ATTRIBUTE_NAME_MAX_LENGTH>, Attribute> m_detected_attributes;
	};
}

template<ESR_REGISTRY_PARAMS>
ESR_REGISTER_PROC_W("Logger", rglz::Attribute, attr, stream,
	ESR_PACK({
		stream << "[ AttributeName: \"" << attr.name() << "\", ";
		switch (attr.type()) {
			case rglz::AttributeType::Boolean: {
				stream << "AttributeValue: " << attr.boolean_value().value() << ", AttributeType: Boolean";
			} break;
			case rglz::AttributeType::Double: {
				stream << "AttributeValue: " << attr.double_value().value() << ", AttributeType: Double";
			} break;
			case rglz::AttributeType::String: {
				stream << "AttributeValue: \"" << attr.string_value().value() << '\"' << ", AttributeType: String";
			} break;
			case rglz::AttributeType::Null: {
				stream << "AttributeValue: Null, AttributeType: Null";
			} break;
		}
		stream << " ]";
	});
);


#endif	// RGLZ_PREFERENCES_H
