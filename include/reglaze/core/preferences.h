#ifndef RGLZ_PREFERENCES_H
#define RGLZ_PREFERENCES_H

#include "base.h"
#include "core/engine.h"
#include "containers/fixed_vector.h"
#include "containers/fixed_string.h"
#include <fstream>
#include <array>
#include <optional>

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
		None
	};

	class Attribute {
	public:
		Attribute();
		Attribute(std::string_view name, bool value);
		Attribute(std::string_view name, double value);
		Attribute(std::string_view name, std::string_view value);

		inline AttributeType type() const {
			return m_type;
		}

		inline std::string_view name() const {
			return m_name;
		}

		inline std::optional<bool> boolean_value() const {
			if (m_type == AttributeType::Boolean) {
				return m_value.b;
			}
			return {};
		}

		inline std::optional<double> double_value() const {
			if (m_type == AttributeType::Double) {
				return m_value.i;
			}
			return {};
		}

		inline std::optional<std::string_view> string_value() const {
			if (m_type == AttributeType::String) {
				return m_value.s;
			}
			return {};
		}

	private:
		union Value { 
			bool b;
			double i;
			rglz::FixedString<RGLZ_PREFERENCES_ATTRIBUTE_RVALUE_MAX_LENGTH> s;
		};

	private:
		AttributeType m_type;
		FixedString<RGLZ_PREFERENCES_ATTRIBUTE_NAME_MAX_LENGTH> m_name;
		Value m_value;
	};

	class Preferences {
	public:
		Preferences(std::string_view file_name);

	private:
		[[nodiscard]]
		std::optional<std::pair<FixedString<RGLZ_PREFERENCES_ATTRIBUTE_NAME_MAX_LENGTH>, FixedString<RGLZ_PREFERENCES_ATTRIBUTE_RVALUE_MAX_LENGTH>>> read_line(std::ifstream& file, std::string_view file_name, std::uint32_t& line_number);

		[[nodiscard]]
		std::optional<Attribute> parse_attribute(std::string_view attribute_name, std::string_view attribute_value_string);

	private:
		FixedVector<Attribute> m_attributes;
	};
}

template<ESR_REGISTRY_PARAMS>
ESR_REGISTER_PROC_W("Logger", rglz::Attribute, attr, stream,
	ESR_PACK({
		stream << "[ AttributeName: \'" << attr.name() << "\', ";
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
			case rglz::AttributeType::None: {
				stream << "AttributeValue: None, AttributeType: None";
			} break;
		}
		stream << " ]";
	});
);


#endif	// RGLZ_PREFERENCES_H
