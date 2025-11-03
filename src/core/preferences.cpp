#include "pch.h"
#include "core/preferences.h"
#include <cctype>
#include <charconv>

namespace rglz {
	Preferences::Preferences(std::string_view file_name, const FixedVector<Attribute, RGLZ_PREFERENCES_MAX_ATTRIBUTES>& default_attributes) 
		: m_rebuild(true)
		, m_file_name(file_name)
		, m_default_attributes(default_attributes)
		, m_current_attributes()
	{
		{
			std::fstream(m_file_name.data(), std::fstream::app);
		}
		std::ifstream file(m_file_name.data(), std::ios::binary);
		// esr::WriteStream<PreferencesStream> m_ofile_stream(m_ofile);
		// esr::ReadStream<PreferencesStream> m_ifile_stream(m_ifile);

		if (!file.is_open()) {
			RGLZ_ENGINE_LOG_ERROR("Failed to open or create preferences file \"", m_file_name, '\"');
			return;
		}
		std::uint32_t line_number = 1;
		do {
			auto token_pair = read_line(file, m_file_name, line_number);
			if (token_pair.has_value()) {
				auto attribute = parse_attribute(token_pair->first, token_pair->second);
				if (attribute.has_value()) {
					if (m_current_attributes[attribute->name()] != nullptr) {
						RGLZ_ENGINE_LOG_ERROR("Attribute \"", attribute->name(), "\" in preferences file \"", m_file_name, "\" was already defined. Ignoring all occurences except the first one.");
					}
					else {
						m_current_attributes.insert(attribute->name(), *attribute);
					}
				}
			}
		}
		while (!file.eof());

		rebuild();
	}

	Attribute* Preferences::write_attribute(std::string_view attribute_name, bool value) {
		Attribute* attribute = m_current_attributes[attribute_name];
		RGLZ_ASSERT(attribute != nullptr);
		*(attribute) = value;
		m_rebuild = true;

		return attribute;
	}

	Attribute* Preferences::write_attribute(std::string_view attribute_name, double value) {
		Attribute* attribute = m_current_attributes[attribute_name];
		RGLZ_ASSERT(attribute != nullptr);
		*(attribute) = value;
		m_rebuild = true;

		return attribute;
	}


	Attribute* Preferences::write_attribute(std::string_view attribute_name, const char* value) {
		Attribute* attribute = m_current_attributes[attribute_name];
		RGLZ_ASSERT(attribute != nullptr);
		*(attribute) = value;
		m_rebuild = true;

		return attribute;
	}

	Attribute* Preferences::write_attribute(std::string_view attribute_name, std::string_view value) {
		Attribute* attribute = m_current_attributes[attribute_name];
		RGLZ_ASSERT(attribute != nullptr);
		*(attribute) = value;
		m_rebuild = true;

		return attribute;
	}

	Attribute* Preferences::write_attribute(std::string_view attribute_name) {
		Attribute* attribute = m_current_attributes[attribute_name];
		RGLZ_ASSERT(attribute != nullptr);
		attribute->set_null();
		m_rebuild = true;

		return attribute;
	}

	std::optional<std::pair<FixedString<RGLZ_PREFERENCES_ATTRIBUTE_NAME_MAX_LENGTH>, FixedString<RGLZ_PREFERENCES_ATTRIBUTE_RVALUE_MAX_LENGTH>>> Preferences::read_line(std::ifstream& file, std::string_view m_file_name, std::uint32_t& line_number) {
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

								RGLZ_ENGINE_LOG_ERROR("in \"", m_file_name, "\" at line ", line_number, ": AttributeName \'", attribute_name, "...\' length has exceeded the max length of ", RGLZ_PREFERENCES_ATTRIBUTE_NAME_MAX_LENGTH, " characters");
							}
							else if (is_space_in_attribute_name) {
								is_invalid_line = true;
								RGLZ_ENGINE_LOG_ERROR("in \"", m_file_name, "\" at line ", line_number, ": Unallowed spaces in AttributeName \'", attribute_name, "...\'");
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

								RGLZ_ENGINE_LOG_ERROR("in \"", m_file_name, "\" at line ", line_number, ": AttributeValue \"", attribute_value_string, "...\" of attribute \"", attribute_name, "\" length has exceeded the max length of ", RGLZ_PREFERENCES_ATTRIBUTE_RVALUE_MAX_LENGTH, " characters");
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
			double result;
			if (std::from_chars(attribute_value_string.data(), attribute_value_string.data() + attribute_value_string.size(), result).ec == std::errc()) {
				return Attribute(attribute_name, result);
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
				bool value = attribute.boolean_value();
				if (value) {
					file << "True";
				}
				else {
					file << "False";
				}
			} break;
			case AttributeType::Double: {
				file << std::fixed << attribute.double_value();
			} break;
			case AttributeType::String: {
				file << "\"" << static_cast<std::string_view>(attribute.string_value()) << "\"";
			} break;
			case AttributeType::Null: {
				file << "Null";
			} break;
		}

		file << std::endl;
	}

	void Preferences::rebuild() {
		if (!m_rebuild) {
			return;
		}

		m_rebuild = false;
		std::ofstream file(m_file_name.data());
		auto undeclared_attributes = m_current_attributes;

		std::size_t n_added_attributes = 0;
		for (auto& att : m_default_attributes) {
			auto parsed_att = m_current_attributes[att.name()];
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

		RGLZ_ENGINE_LOG_TRACE_GET(attributes_log_builder, m_current_attributes.size(), " parsed attributes in \"", m_file_name, "\"");
		if (m_current_attributes.size()) {
			RGLZ_ENGINE_LOG_TRACE_TO(attributes_log_builder, ":");
		}
		for (auto& att : m_default_attributes) {
			auto* parsed_att = m_current_attributes[att.name()];
			if (parsed_att != nullptr) {
				RGLZ_ENGINE_LOG_TRACE_TO(attributes_log_builder, alignln, "  ", *parsed_att);
			}
		}

		RGLZ_ENGINE_LOG_TRACE_TO(attributes_log_builder, alignln, n_added_attributes, " added attributes in \"", m_file_name, "\"");
		if (n_added_attributes) {
			RGLZ_ENGINE_LOG_TRACE_TO(attributes_log_builder, ":");
		}
		for (auto& att : m_default_attributes) {
			if (m_current_attributes[att.name()] == nullptr) {
				RGLZ_ENGINE_LOG_TRACE_TO(attributes_log_builder, alignln, "  ", att);
				m_current_attributes.insert(att.name(), att);
			}
		}
	}
}
