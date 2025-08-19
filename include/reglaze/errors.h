#ifndef RGLZ_ERRORS_H
#define RGLZ_ERRORS_H

#include <string_view>
#include <cstdint>

#ifndef RGLZ_ENGINE_RELEASE
	#define RGLZ_ENGINE_ASSERT(expr)	\
		{	\
			if (!(expr)) {	\
				rglz::report_engine_assertion_failure(#expr, __LINE__, __FILE__); \
			}	\
		}
	#define RGLZ_ENGINE_ASSERT_MSG(expr, msg)	\
		{	\
			if (!(expr)) {	\
				rglz::report_engine_assertion_failure(#expr, msg, __LINE__, __FILE__); \
			}	\
		}
#else
	#define RGLZ_ENGINE_ASSERT(expr)
	#define RGLZ_ENGINE_ASSERT_MSG(expr, msg)
#endif

#ifndef RGLZ_RELEASE
	#define RGLZ_ASSERT(expr)	\
		{	\
			if (!(expr)) {	\
				rglz::report_assertion_failure(#expr, __LINE__, __FILE__);	\
			}	\
		}
	#define RGLZ_ASSERT_MSG(expr, msg)	\
		{	\
			if (!(expr)) {	\
				rglz::report_assertion_failure(#expr, msg, __LINE__, __FILE__);	\
			}	\
		}
#else
	#define RGLZ_ASSERT(expr)
	#define RGLZ_ASSERT_MSG(expr, msg)
#endif

namespace rglz {
	class FatalError {

	};

	void report_engine_assertion_failure(std::string_view expr, std::uint32_t line, std::string_view file_name);

	void report_engine_assertion_failure(std::string_view expr, std::string_view msg, std::uint32_t line, std::string_view file_name);
	void report_assertion_failure(std::string_view expr, std::uint32_t line, std::string_view file_name);

	void report_assertion_failure(std::string_view expr, std::string_view msg, std::uint32_t line, std::string_view file_name);
}

#endif	// RGLZ_ERRORS_H
