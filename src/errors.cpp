#include "pch.h"

namespace rglz {
	void report_engine_assertion_failure(std::string_view expr, std::string_view msg, std::uint32_t line, std::string_view file_name) {
		rglz::Engine::log(rglz::LoggerSeverity::Fatal) << "Assertion failed at line " << line << " of file " << file_name << alignln << "MSG: " << msg << alignln << "EXPR: "<< expr;
		throw FatalError{};
	}

	void report_engine_assertion_failure(std::string_view expr, std::uint32_t line, std::string_view file_name) {
		rglz::Engine::log(rglz::LoggerSeverity::Fatal) << "Assertion failed at line " << line << " of file " << file_name << alignln << "EXPR: "<< expr;
		throw FatalError{};
	}

	void report_assertion_failure(std::string_view expr, std::string_view msg, std::uint32_t line, std::string_view file_name) {
		rglz::Engine::app_logger().log(rglz::LoggerSeverity::Fatal) << "Assertion failed at line " << line << " of file " << file_name << alignln << "MSG: " << msg << alignln << "EXPR: "<< expr;
		throw FatalError{};
	}

	void report_assertion_failure(std::string_view expr, std::uint32_t line, std::string_view file_name) {
		rglz::Engine::app_logger().log(rglz::LoggerSeverity::Fatal) << "Assertion failed at line " << line << " of file " << file_name << alignln << "EXPR: "<< expr;
		throw FatalError{};
	}
}
