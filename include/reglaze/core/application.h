#ifndef RGLZ_APPLICATION_H
#define RGLZ_APPLICATION_H

#include "base.h"
#include "core/logger.h"

namespace rglz {
	class Engine;

	class Application : public Logger<RGLZ_LOGGER_LOG_MAX_CUNCURRENT_LOGS> {
	public:
		Application(const char* file_name = "app_log_latest.txt", const char* logger_name = "APP");
		virtual ~Application();

		virtual void on_startup();

		virtual void on_shutdown();

	private:
		friend class Engine;
	};
}


#endif	// RGLZ_APPLICATION_H
