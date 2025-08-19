#ifndef RGLZ_ENGINE_H
#define RGLZ_ENGINE_H

#include "base.h"
#include "core/application.h"
#include "core/logger.h"
#include <type_traits>
#include <memory>

#define RGLZ_REGISTER_CLIENT_APP(app_type) \
namespace rglz { \
	void register_client_app() { \
		Engine::register_client_app<app_type>(); \
	} \
}

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
			s_app = static_cast<Application*>(internal_allocate(sizeof(TApplication)));
			std::construct_at(static_cast<TApplication*>(s_app));
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
		static void* internal_allocate(std::size_t size);

	private:
		static LoggerType s_logger;
		static Application* s_app;
	};
}

#endif	// RGLZ_ENGINE_H
