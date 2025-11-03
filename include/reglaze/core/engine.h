#ifndef RGLZ_ENGINE_H
#define RGLZ_ENGINE_H

#include "base.h"
#include "core/application.h"
#include "core/preferences.h"
#include "core/logger.h"
#include "core/memory_profile.h"
#include <type_traits>
#include <memory>
#include <utility>

#define RGLZ_REGISTER_CLIENT_APP(app_type) \
namespace rglz { \
	void register_client_app() { \
		Engine::register_client_app<app_type>(); \
	} \
}

#define RGLZ_DEFAULT_APP_MEMORY_PROFILE(default_app_memory_profile_body) \
namespace rglz { \
	MemoryProfile default_app_memory_profile() { \
		default_app_memory_profile_body \
	} \
}

// Engine logger

#if RGLZ_ENGINE_LOGGER_DEBUG_ENABLE == 1
	#define RGLZ_ENGINE_LOG_DEBUG(...) (::rglz::Engine::log(::rglz::LoggerSeverity::Debug).stream(__VA_ARGS__))
	#define RGLZ_ENGINE_LOG_DEBUG_GET(log_name, ...) auto log_name = (::rglz::Engine::log(::rglz::LoggerSeverity::Debug).stream( __VA_ARGS__))
	#define RGLZ_ENGINE_LOG_DEBUG_TO(log_name, ...)	\
		RGLZ_ENGINE_ASSERT_MSG(log_name.severity() == ::rglz::LoggerSeverity::Debug && log_name.logger() == ::rglz::Engine::logger(), "Log is not owned by engine logger!");	\
		log_name.stream(__VA_ARGS__);
#else
	#define RGLZ_ENGINE_LOG_DEBUG(...)
	#define RGLZ_ENGINE_LOG_DEBUG_GET(log_name, ...)
	#define RGLZ_ENGINE_LOG_DEBUG_TO(log_name, ...)
#endif

#if RGLZ_ENGINE_LOGGER_TRACE_ENABLE == 1
	#define RGLZ_ENGINE_LOG_TRACE(...) (::rglz::Engine::log(::rglz::LoggerSeverity::Trace).stream(__VA_ARGS__))
	#define RGLZ_ENGINE_LOG_TRACE_GET(log_name, ...) auto log_name = (::rglz::Engine::log(::rglz::LoggerSeverity::Trace).stream( __VA_ARGS__))
	#define RGLZ_ENGINE_LOG_TRACE_TO(log_name, ...)	\
		RGLZ_ENGINE_ASSERT_MSG(log_name.severity() == ::rglz::LoggerSeverity::Trace && log_name.logger() == ::rglz::Engine::logger(), "Log is not owned by engine logger!");	\
		log_name.stream(__VA_ARGS__);
#else
	#define RGLZ_ENGINE_LOG_TRACE(...)
	#define RGLZ_ENGINE_LOG_TRACE_GET(log_name, ...)
	#define RGLZ_ENGINE_LOG_TRACE_TO(log_name, ...)
#endif
	
#if RGLZ_ENGINE_LOGGER_INFO_ENABLE == 1
	#define RGLZ_ENGINE_LOG_INFO(...) (::rglz::Engine::log(::rglz::LoggerSeverity::Info).stream(__VA_ARGS__))
	#define RGLZ_ENGINE_LOG_INFO_GET(log_name, ...) auto log_name = (::rglz::Engine::log(::rglz::LoggerSeverity::Info).stream( __VA_ARGS__))
	#define RGLZ_ENGINE_LOG_INFO_TO(log_name, ...)	\
		RGLZ_ENGINE_ASSERT_MSG(log_name.severity() == ::rglz::LoggerSeverity::Info && log_name.logger() == ::rglz::Engine::logger(), "Log is not owned by engine logger!");	\
		log_name.stream(__VA_ARGS__);
#else
	#define RGLZ_ENGINE_LOG_INFO(...)
	#define RGLZ_ENGINE_LOG_INFO_GET(log_name, ...)
	#define RGLZ_ENGINE_LOG_INFO_TO(log_name, ...)
#endif
	
#if RGLZ_ENGINE_LOGGER_WARN_ENABLE == 1
	#define RGLZ_ENGINE_LOG_WARN(...) (::rglz::Engine::log(::rglz::LoggerSeverity::Warn).stream(__VA_ARGS__))
	#define RGLZ_ENGINE_LOG_WARN_GET(log_name, ...) auto log_name = (::rglz::Engine::log(::rglz::LoggerSeverity::Warn).stream( __VA_ARGS__))
	#define RGLZ_ENGINE_LOG_WARN_TO(log_name, ...)	\
		RGLZ_ENGINE_ASSERT_MSG(log_name.severity() == ::rglz::LoggerSeverity::Warn && log_name.logger() == ::rglz::Engine::logger(), "Log is not owned by engine logger!");	\
		log_name.stream(__VA_ARGS__);
#else
	#define RGLZ_ENGINE_LOG_WARN(...)
	#define RGLZ_ENGINE_LOG_WARN_GET(log_name, ...)
	#define RGLZ_ENGINE_LOG_WARN_TO(log_name, ...)
#endif
	
#if RGLZ_ENGINE_LOGGER_ERROR_ENABLE == 1
	#define RGLZ_ENGINE_LOG_ERROR(...) (::rglz::Engine::log(::rglz::LoggerSeverity::Error).stream(__VA_ARGS__))
	#define RGLZ_ENGINE_LOG_ERROR_GET(log_name, ...) auto log_name = (::rglz::Engine::log(::rglz::LoggerSeverity::Error).stream( __VA_ARGS__))
	#define RGLZ_ENGINE_LOG_ERROR_TO(log_name, ...)	\
		RGLZ_ENGINE_ASSERT_MSG(log_name.severity() == ::rglz::LoggerSeverity::Info && log_name.logger() == ::rglz::Engine::logger(), "Log is not owned by engine logger!");	\
		log_name.stream(__VA_ARGS__);
#else
	#define RGLZ_ENGINE_LOG_ERROR(...)
	#define RGLZ_ENGINE_LOG_ERROR_GET(log_name, ...)
	#define RGLZ_ENGINE_LOG_ERROR_TO(log_name, ...)
#endif

// App logger

#if RGLZ_LOGGER_DEBUG_ENABLE == 1
	#define RGLZ_LOG_DEBUG(...) (::rglz::Engine::app().log(::rglz::LoggerSeverity::Debug).stream(__VA_ARGS__))
	#define RGLZ_LOG_DEBUG_GET(log_name, ...) auto log_name = (::rglz::Engine::app().log(::rglz::LoggerSeverity::Debug).stream( __VA_ARGS__))
	#define RGLZ_LOG_DEBUG_TO(log_name, ...)	\
		RGLZ_ASSERT_MSG(log_name.severity() == ::rglz::LoggerSeverity::Debug && log_name.logger() == &::rglz::Engine::app(), "Log is not owned by app logger!");	\
		log_name.stream(__VA_ARGS__)
#else
	#define RGLZ_LOG_DEBUG(...)
	#define RGLZ_LOG_DEBUG_GET(log_name, ...)
	#define RGLZ_LOG_DEBUG_TO(log_name, ...)
#endif


#if RGLZ_LOGGER_TRACE_ENABLE == 1
	#define RGLZ_LOG_TRACE(...) (::rglz::Engine::app().log(::rglz::LoggerSeverity::Trace).stream(__VA_ARGS__))
	#define RGLZ_LOG_TRACE_GET(log_name, ...) auto log_name = (::rglz::Engine::app().log(::rglz::LoggerSeverity::Trace).stream( __VA_ARGS__))
	#define RGLZ_LOG_TRACE_TO(log_name, ...)	\
		RGLZ_ASSERT_MSG(log_name.severity() == ::rglz::LoggerSeverity::Trace && log_name.logger() == &::rglz::Engine::app(), "Log is not owned by app logger!");	\
		log_name.stream(__VA_ARGS__)
#else
	#define RGLZ_LOG_TRACE(...)
	#define RGLZ_LOG_TRACE_GET(log_name, ...)
	#define RGLZ_LOG_TRACE_TO(log_name, ...)
#endif
	

#if RGLZ_LOGGER_INFO_ENABLE == 1
	#define RGLZ_LOG_INFO(...) (::rglz::Engine::app().log(::rglz::LoggerSeverity::Info).stream(__VA_ARGS__))
	#define RGLZ_LOG_INFO_GET(log_name, ...) auto log_name = (::rglz::Engine::app().log(::rglz::LoggerSeverity::Info).stream( __VA_ARGS__))
	#define RGLZ_LOG_INFO_TO(log_name, ...)	\
		RGLZ_ASSERT_MSG(log_name.severity() == ::rglz::LoggerSeverity::Info && log_name.logger() == &::rglz::Engine::app(), "Log is not owned by app logger!");	\
		log_name.stream(__VA_ARGS__)
#else
	#define RGLZ_LOG_INFO(...)
	#define RGLZ_LOG_INFO_GET(log_name, ...)
	#define RGLZ_LOG_INFO_TO(log_name, ...)
#endif

#if RGLZ_LOGGER_WARN_ENABLE == 1
	#define RGLZ_LOG_WARN(...) (::rglz::Engine::app().log(::rglz::LoggerSeverity::Warn).stream(__VA_ARGS__))
	#define RGLZ_LOG_WARN_GET(log_name, ...) auto log_name = (::rglz::Engine::app().log(::rglz::LoggerSeverity::Warn).stream( __VA_ARGS__))
	#define RGLZ_LOG_WARN_TO(log_name, ...)	\
		RGLZ_ASSERT_MSG(log_name.severity() == ::rglz::LoggerSeverity::Warn && log_name.logger() == &::rglz::Engine::app(), "Log is not owned by app logger!");	\
		log_name.stream(__VA_ARGS__)
#else
	#define RGLZ_LOG_WARN(...)
	#define RGLZ_LOG_WARN_GET(log_name, ...)
	#define RGLZ_LOG_WARN_TO(log_name, ...)
#endif
	
#if RGLZ_LOGGER_ERROR_ENABLE == 1
	#define RGLZ_LOG_ERROR(...) (::rglz::Engine::app().log(::rglz::LoggerSeverity::Error).stream(__VA_ARGS__))
	#define RGLZ_LOG_ERROR_GET(log_name, ...) auto log_name = (::rglz::Engine::app().log(::rglz::LoggerSeverity::Error).stream( __VA_ARGS__))
	#define RGLZ_LOG_ERROR_TO(log_name, ...)	\
		RGLZ_ASSERT_MSG(log_name.severity() == ::rglz::LoggerSeverity::Error && log_name.logger() == &::rglz::Engine::app(), "Log is not owned by app logger!");	\
		log_name.stream(__VA_ARGS__)
#else
	#define RGLZ_LOG_ERROR(...)
	#define RGLZ_LOG_ERROR_GET(log_name, ...)
	#define RGLZ_LOG_ERROR_TO(log_name, ...)
#endif

namespace rglz {
	class Engine {
	public:
		using LoggerType = Logger<3>;

	public:
		static void startup();

		static void shutdown();

		static void run();

		template<typename TApplication, typename = std::enable_if_t<std::is_base_of_v<Application, TApplication>>> 
		static void register_client_app() {
			void* raw_app = internal_allocate(sizeof(TApplication));
			s_app = static_cast<TApplication*>(raw_app);
			std::construct_at(static_cast<TApplication*>(raw_app));
		}

		static inline Application& app() {
			return *s_app;
		}

		static inline LoggerType::Log log(LoggerSeverity severity) {
			return s_logger.log(severity);

		}
		static inline void end_log(LoggerType::Log& log) {
			s_logger.end_log(log);

		}
		static inline void flush_logs() {
			s_logger.flush_logs();
		}
		static inline const LoggerType* logger() {
			return &s_logger;
		}

		Engine() = delete;
		Engine(const Engine&) = delete;

	private:
		Engine(const Preferences& preferences);

		static void* internal_allocate(std::size_t size);

	private:
		static LoggerType s_logger;
		static Application* s_app;
	};
}

#endif	// RGLZ_ENGINE_H
