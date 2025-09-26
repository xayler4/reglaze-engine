#include "pch.h"
#include "core/preferences.h"
#include <cctype>
#include <stdlib.h>

namespace rglz {
	Attribute::Attribute() 
		: m_name("")
		, m_value(nullptr)
	{
	}

	Attribute::Attribute(std::string_view name, bool value)
		: m_name(name)
		, m_value(value)

	{
		RGLZ_ASSERT_MSG(is_string_valid(name), "Attribute string must contain only alphanumeric characters or spaces!");
	}

	Attribute::Attribute(std::string_view name, double value)
		: m_name(name)
		, m_value(value)

	{
		RGLZ_ASSERT_MSG(is_string_valid(name), "Attribute string must contain only alphanumeric characters or spaces!");
	}

	Attribute::Attribute(std::string_view name, const char* value)
		: m_name(name)
		, m_value(value)
	{
		RGLZ_ASSERT_MSG(is_string_valid(name) && is_string_valid(value), "Attribute string must contain only alphanumeric characters or spaces!");
	}

	Attribute::Attribute(std::string_view name, std::string_view value)
		: m_name(name)
		, m_value(value)
	{
		RGLZ_ASSERT_MSG(is_string_valid(name) && is_string_valid(value), "Attribute string must contain only alphanumeric characters or spaces");
	}

	Attribute::Attribute(std::string_view name) 
		: m_name(name)
		, m_value(nullptr)
	{
		RGLZ_ASSERT_MSG(is_string_valid(name), "Attribute string must contain only alphanumeric characters or spaces!");
	}

	Preferences::Preferences(std::string_view file_name, FixedVector<Attribute> default_attributes) 
		: m_default_attributes(default_attributes)
	{
		{

			{
				std::fstream(file_name.data(), std::fstream::app);
			}
			std::ifstream file(file_name.data(), std::ios::binary);
			// esr::WriteStream<PreferencesStream> m_ofile_stream(m_ofile);
			// esr::ReadStream<PreferencesStream> m_ifile_stream(m_ifile);

			if (!file.is_open()) {
				RGLZ_ENGINE_LOG_ERROR("Failed to open or create preferences file \"", file_name, '\"');
				return;
			}
			std::uint32_t line_number = 1;
			do {
				auto token_pair = read_line(file, file_name, line_number);
				if (token_pair.has_value()) {
					auto attribute = parse_attribute(token_pair->first, token_pair->second);
					if (attribute.has_value()) {
						if (m_detected_attributes[attribute->name()] != nullptr) {
							RGLZ_ENGINE_LOG_ERROR("Attribute \"", attribute->name(), "\" in preferences file \"", file_name, "\" was already defined. Ignoring all occurences except the first one.");
						}
						else {
							m_detected_attributes.insert(attribute->name(), *attribute);
						}
					}
				}
			}
			while (!file.eof());

		}
		std::ofstream file(file_name.data());
		auto undeclared_attributes = m_detected_attributes;

		std::size_t n_added_attributes = 0;
		for (auto& att : m_default_attributes) {
			auto parsed_att = m_detected_attributes[att.name()];
			if (parsed_att != nullptr) {
				write_attribute(*parsed_att, file);
			}
			else {
				write_attribute(att, file);
				++n_added_attributes;
			}
			undeclared_attributes.remove(att.name());
		}

		for (auto& att : undeclared_attributes) {
			write_attribute(att.value, file);
		}

		RGLZ_ENGINE_LOG_TRACE_GET(parsed_attributes_log_builder, m_detected_attributes.size(), " parsed attributes in \"", file_name, "\"");
		if (m_detected_attributes.size()) {
			RGLZ_ENGINE_LOG_TRACE_TO(parsed_attributes_log_builder, ":");
		}
		for (auto& att : m_default_attributes) {
			auto parsed_att = m_detected_attributes[att.name()];
			if (parsed_att != nullptr) {
				RGLZ_ENGINE_LOG_TRACE_TO(parsed_attributes_log_builder, alignln, "  ", *parsed_att);
			}
		}
		for (auto& att : undeclared_attributes) {
			RGLZ_ENGINE_LOG_TRACE_TO(parsed_attributes_log_builder, alignln, "  ", att.value);
		}

		RGLZ_ENGINE_LOG_TRACE_GET(added_attributes_log_builder, n_added_attributes, " added attributes in \"", file_name, "\"");
		if (n_added_attributes) {
			RGLZ_ENGINE_LOG_TRACE_TO(added_attributes_log_builder, ":");
		}
		for (auto& att : m_default_attributes) {
			if (m_detected_attributes[att.name()] == nullptr) {
				RGLZ_ENGINE_LOG_TRACE_TO(added_attributes_log_builder, alignln, "  ", att);
			}
		}
	}

	std::optional<std::pair<FixedString<RGLZ_PREFERENCES_ATTRIBUTE_NAME_MAX_LENGTH>, FixedString<RGLZ_PREFERENCES_ATTRIBUTE_RVALUE_MAX_LENGTH>>> Preferences::read_line(std::ifstream& file, std::string_view file_name, std::uint32_t& line_number) {
		while (!file.eof()) {
			FixedString<RGLZ_PREFERENCES_ATTRIBUTE_NAME_MAX_LENGTH> attribute_name;
			FixedString<RGLZ_PREFERENCES_ATTRIBUTE_RVALUE_MAX_LENGTH> attribute_value_string;
			bool is_attribute_name_processed = false;
			bool is_attribute_name = false;
			bool is_space_in_attribute_name = false;
			bool is_value_string = false;
			bool is_processed = false;
			bool is_carriage_return = false;
			bool is_invalid_line = false;
			char current_char;

			do {
				// Handle multiplatform endline
				bool is_endline = false;

				if (!is_carriage_return && !file.eof()) {
					file.get(current_char);
				}
				else {
					is_carriage_return = false;
				}

				if (current_char == '\r') {
					if (!file.eof()) {
						char secondary_char;
						file.get(secondary_char);

						if (secondary_char != '\n') {
							is_carriage_return = true;		
							current_char = secondary_char;
						}
					}
					is_endline = true;						
				}

				else if (current_char == '\n') {
					is_endline = true;
				}

				if (is_endline) {
					if (!attribute_name.length() || !attribute_value_string.length()) {
						is_invalid_line = true;
					}
					if (is_invalid_line) {
						attribute_name = FixedString<RGLZ_PREFERENCES_ATTRIBUTE_NAME_MAX_LENGTH>();
						attribute_value_string = FixedString<RGLZ_PREFERENCES_ATTRIBUTE_RVALUE_MAX_LENGTH>();
						is_invalid_line = false;
						is_attribute_name_processed = false;
						is_value_string = false;
						is_space_in_attribute_name = false;
					}
					else {
						is_processed = true;
					}
					++line_number;
				}
				// If invalid line skip character until is_endline gets flagged
				else if (!is_invalid_line) {
					// Process AttributeName
					if (!is_attribute_name_processed) {
						if (current_char == '=') {
							is_attribute_name_processed = true;
						}
						else if (current_char != ' ') {
							if (attribute_name.length() == RGLZ_PREFERENCES_ATTRIBUTE_NAME_MAX_LENGTH) {
								is_invalid_line = true;

								RGLZ_ENGINE_LOG_ERROR("in \"", file_name, "\" at line ", line_number, ": AttributeName \'", attribute_name, "...\' length has exceeded the max length of ", RGLZ_PREFERENCES_ATTRIBUTE_NAME_MAX_LENGTH, " characters");
							}
							else if (is_space_in_attribute_name) {
								is_invalid_line = true;
								RGLZ_ENGINE_LOG_ERROR("in \"", file_name, "\" at line ", line_number, ": Unallowed spaces in AttributeName \'", attribute_name, "...\'");
							}
							else {
								std::string_view current_view(&current_char, 1);
								attribute_name += current_view;
							}
						}
						else {
							is_space_in_attribute_name = true;
						}
					}
					// Process AttributeValue
					else {
						if (current_char == '\"') {
							is_value_string = true;
						}
						if (current_char != ' ' || is_value_string) {
							if (attribute_value_string.length() == RGLZ_PREFERENCES_ATTRIBUTE_RVALUE_MAX_LENGTH) {
								is_invalid_line = true;

								RGLZ_ENGINE_LOG_ERROR("in \"", file_name, "\" at line ", line_number, ": AttributeValue \"", attribute_value_string, "...\" of attribute \"", attribute_name, "\" length has exceeded the max length of ", RGLZ_PREFERENCES_ATTRIBUTE_RVALUE_MAX_LENGTH, " characters");
							}
							else {
								std::string_view current_view(&current_char, 1);
								attribute_value_string += current_view;
							}
						}

					}
				}

			} while(!is_processed && !file.eof());

			if (is_processed) {
				return std::make_pair(attribute_name, attribute_value_string);
			}
		}
		return {};
	}

	std::optional<Attribute> Preferences::parse_attribute(std::string_view attribute_name, std::string_view attribute_value_string) {
		if (attribute_value_string == "True") {
			return Attribute(attribute_name, true);
		}
		else if (attribute_value_string == "False") {
			return Attribute(attribute_name, false);
		}
		else if (attribute_value_string == "Null") {
			return Attribute(attribute_name);
		}
		else if (isdigit(attribute_value_string[0])) {
			char* end_ptr;
			if (attribute_value_string.length() == 1 && attribute_value_string[0] == '0') {
				return Attribute(attribute_name, 0.0);
			}
			else if (double result = strtod(attribute_value_string.data(), &end_ptr)) {
				if (errno != ERANGE && result != 0) {
					return Attribute(attribute_name, result );
				}
			}
		}
		else if (attribute_value_string[0] == '\"' && attribute_value_string[attribute_value_string.length() - 1] == '\"') {
			std::string_view str(attribute_value_string.data() + 1, attribute_value_string.length() - 2);
			return Attribute(attribute_name, str);
		}

		return {};
	}

	void Preferences::write_attribute(Attribute& attribute, std::ofstream& file) {
		file << attribute.name();
		file << " = ";

		switch (attribute.type()) {
			case AttributeType::Boolean: {
				bool value = attribute.boolean_value().value();
				if (value) {
					file << "True";
				}
				else {
					file << "False";
				}
			} break;
			case AttributeType::Double: {
				file << attribute.double_value().value();
			} break;
			case AttributeType::String: {
				file << "\"" << attribute.string_value().value() << "\"";
			} break;
			case AttributeType::Null: {
				file << "Null";
			} break;
		}

		file << std::endl;
	}
}
