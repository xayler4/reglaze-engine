#include "base.h"
#include "core/engine.h"
#include "core/logger.h"
#include <cassert>

namespace rglz {
	void report_engine_assertion_failure(std::string_view expr, std::string_view msg, std::uint32_t line, std::string_view file_name) {
		rglz::Engine::log(rglz::LoggerSeverity::Fatal) << "Assertion failed at line " << line << " of file " << file_name << alignln << "MSG: " << msg << alignln << "EXPR: "<< expr;
		rglz::Engine::flush_logs();
		assert(false);
	}

	void report_engine_assertion_failure(std::string_view expr, std::uint32_t line, std::string_view file_name) {
		rglz::Engine::log(rglz::LoggerSeverity::Fatal) << "Assertion failed at line " << line << " of file " << file_name << alignln << "EXPR: "<< expr;
		rglz::Engine::flush_logs();
		assert(false);
	}

	void report_assertion_failure(std::string_view expr, std::string_view msg, std::uint32_t line, std::string_view file_name) {
		rglz::Engine::app().log(rglz::LoggerSeverity::Fatal) << "Assertion failed at line " << line << " of file " << file_name << alignln << "MSG: " << msg << alignln << "EXPR: "<< expr;
		rglz::Engine::app().flush_logs();
		assert(false);
	}

	void report_assertion_failure(std::string_view expr, std::uint32_t line, std::string_view file_name) {
		rglz::Engine::app().log(rglz::LoggerSeverity::Fatal) << "Assertion failed at line " << line << " of file " << file_name << alignln << "EXPR: "<< expr;
		rglz::Engine::app().flush_logs();
		assert(false);
	}
}
