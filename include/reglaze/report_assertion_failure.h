#ifndef RGLZ_REPORT_ASSERTION_FAILURE_H
#define RGLZ_REPORT_ASSERTION_FAILURE_H

#include "base.h"
#include <string_view>
#include <cstdint>

#define RGLZ_ENGINE_ASSERT(expr)	\
	{	\
		if (!(expr)) {	\
			report_engine_assertion_failure(#expr, __LINE__, __FILE__); \
		}	\
	}
#define RGLZ_ENGINE_ASSERT_MSG(expr, msg)	\
	{	\
		if (!(expr)) {	\
			report_engine_assertion_failure(#expr, msg, __LINE__, __FILE__); \
		}	\
	}

#define RGLZ_ASSERT(expr)	\
	{	\
		if (!(expr)) {	\
			report_assertion_failure(#expr, __LINE__, __FILE__);	\
		}	\
	}
#define RGLZ_ASSERT_MSG(expr, msg)	\
	{	\
		if (!(expr)) {	\
			report_assertion_failure(#expr, msg, __LINE__, __FILE__);	\
		}	\
	}

namespace rglz {
	void report_engine_assertion_failure(std::string_view expr, std::uint32_t line, std::string_view file_name);

	void report_engine_assertion_failure(std::string_view expr, std::string_view msg, std::uint32_t line, std::string_view file_name);
	void report_assertion_failure(std::string_view expr, std::uint32_t line, std::string_view file_name);

	void report_assertion_failure(std::string_view expr, std::string_view msg, std::uint32_t line, std::string_view file_name);
}

#endif	// RGLZ_REPORT_ASSERTION_FAILURE_H
