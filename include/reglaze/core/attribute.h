#ifndef RGLZ_ATTRIBUTE_H
#define RGLZ_ATTRIBUTE_H

#include "base.h"
#include "containers/fixed_string.h"
#include <variant>

namespace rglz {
	enum class AttributeType {
		Boolean,
		Double,
		String,
		Null
	};
	
	class Attribute {
	public:
		using Null = std::nullptr_t;
		using Value = std::variant<bool, double, rglz::FixedString<RGLZ_PREFERENCES_ATTRIBUTE_RVALUE_MAX_LENGTH>, Null>;

	public:
		Attribute();
		explicit Attribute(std::string_view name, bool value);
		explicit Attribute(std::string_view name, double value);
		explicit Attribute(std::string_view name, const char* value);
		explicit Attribute(std::string_view name, std::string_view value);
		explicit Attribute(std::string_view name, Null);

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

		inline bool& boolean_value() {
			RGLZ_ASSERT(std::holds_alternative<bool>(m_value));
			return std::get<bool>(m_value);
		}

		inline double& double_value() {
			RGLZ_ASSERT(std::holds_alternative<double>(m_value));
			return std::get<double>(m_value);
		}

		inline FixedString<RGLZ_PREFERENCES_ATTRIBUTE_RVALUE_MAX_LENGTH>& string_value() {
			RGLZ_ASSERT(std::holds_alternative<FixedString<RGLZ_PREFERENCES_ATTRIBUTE_RVALUE_MAX_LENGTH>>(m_value));

			return std::get<FixedString<RGLZ_PREFERENCES_ATTRIBUTE_RVALUE_MAX_LENGTH>>(m_value);
		}

		inline bool boolean_value() const {
			RGLZ_ASSERT(std::holds_alternative<bool>(m_value));
			return std::get<bool>(m_value);
		}

		inline double double_value() const {
			RGLZ_ASSERT(std::holds_alternative<double>(m_value));
			return std::get<double>(m_value);
		}

		inline const FixedString<RGLZ_PREFERENCES_ATTRIBUTE_RVALUE_MAX_LENGTH>& string_value() const {
			RGLZ_ASSERT(std::holds_alternative<FixedString<RGLZ_PREFERENCES_ATTRIBUTE_RVALUE_MAX_LENGTH>>(m_value));

			return std::get<FixedString<RGLZ_PREFERENCES_ATTRIBUTE_RVALUE_MAX_LENGTH>>(m_value);
		}

		inline bool is_null() const {
			return std::holds_alternative<std::nullptr_t>(m_value);
		}

		inline Attribute& operator= (bool value) {
			m_value = value;

			return *this;
		}
		inline Attribute& operator= (double value) {
			m_value = value;

			return *this;
		}
		inline Attribute& operator= (const char* value) {
			m_value = value;

			return *this;
		}

		inline void set_null() {

			m_value = nullptr;
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

}	

template<ESR_REGISTRY_PARAMS>
ESR_REGISTER_PROC_W("Logger", rglz::Attribute, attr, stream,
	ESR_PACK({
		stream << "[ AttributeName: \"" << attr.name() << "\", ";
		switch (attr.type()) {
			case rglz::AttributeType::Boolean: {
				stream << "AttributeValue: " << attr.boolean_value() << ", AttributeType: Boolean";
			} break;
			case rglz::AttributeType::Double: {
				stream << "AttributeValue: " << attr.double_value() << ", AttributeType: Double";
			} break;
			case rglz::AttributeType::String: {
				stream << "AttributeValue: \"" << attr.string_value() << '\"' << ", AttributeType: String";
			} break;
			case rglz::AttributeType::Null: {
				stream << "AttributeValue: Null, AttributeType: Null";
			} break;
		}
		stream << " ]";
	});
);

#endif	// RGLZ_ATTRIBUTE_H
