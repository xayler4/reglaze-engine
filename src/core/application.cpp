#include "base.h"
#include "core/application.h"
#include <iostream>

namespace rglz {
	Application::Application() : m_logger("app_log_latest.txt", "APP"){
		m_logger.log(LoggerSeverity::Info) << "Application started";
	}

	Application::~Application() {
	}

	void Application::on_startup() {
	}

	void Application::on_shutdown() {
	}

	void Application::flush_logger() {
		m_logger.flush();
	}
}
