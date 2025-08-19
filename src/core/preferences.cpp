#include "pch.h"
#include "core/preferences.h"
#include <cctype>
#include <stdlib.h>

namespace rglz {
	Attribute::Attribute() 
		: m_type(AttributeType::None)
		, m_value{0}
	{}

	Attribute::Attribute(std::string_view name, bool value) 
		: m_type(AttributeType::Boolean)
		, m_name(name)
		, m_value{.b = value}
	{}

	Attribute::Attribute(std::string_view name, double value)
		: m_type(AttributeType::Double)
		, m_name(name)
		, m_value{.i = value} 
	{}

	Attribute::Attribute(std::string_view name, std::string_view value) 
		: m_type(AttributeType::String)
		, m_name(name)
		, m_value{.s = value} 
	{}

	Preferences::Preferences(std::string_view file_path) {
		// std::ofstream m_ofile(file_name.data());
		std::ifstream file(file_path.data(), std::ios::binary);
		// esr::WriteStream<PreferencesStream> m_ofile_stream(m_ofile);
		// esr::ReadStream<PreferencesStream> m_ifile_stream(m_ifile);

		if (!file.is_open()) {
			RGLZ_ENGINE_LOG_ERROR("Failed to open preferences file \'", file_path, '\'');
			return;
		}
		std::uint32_t line_number = 1;
		do {
			auto token_pair = read_line(file, file_path, line_number);
			if (token_pair.has_value()) {
				auto attribute = parse_attribute(token_pair->first, token_pair->second);
				if (attribute.has_value()) {
					m_attributes.extend(*attribute);
				}
			}
		}
		while (!file.eof());

		RGLZ_ENGINE_LOG_TRACE_GET(log_builder, m_attributes.size(), " Parsed Attributes in \"", file_path, "\":");
		for (auto& attr : m_attributes) {
			RGLZ_ENGINE_LOG_TRACE_TO(log_builder, alignln, "  ", attr);
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
		else if (isdigit(attribute_value_string[0])) {
			char* end_ptr;
			if (attribute_value_string.length() == 1 && attribute_value_string[0] == '0') {
				return Attribute(attribute_name, 0.0);
			}
			else if (double result = strtod(attribute_value_string.data(), &end_ptr)) {
				if (errno != ERANGE && result != 0) {
					return Attribute(attribute_name, result);
				}
			}
		}
		else if (attribute_value_string[0] == '\"' && attribute_value_string[attribute_value_string.length() - 1] == '\"') {
			std::string_view str(attribute_value_string.data() + 1, attribute_value_string.length() - 2);
			return Attribute(attribute_name, str);
		}

		return {};
	}
}
