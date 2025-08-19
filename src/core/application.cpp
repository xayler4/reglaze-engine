#include "pch.h"
#include "core/application.h"
#include <iostream>

namespace rglz {
	Application::Application(const char* file_name, const char* logger_name) : Logger(file_name, logger_name) {
	}

	Application::~Application() {
		flush_logs();
	}

	void Application::on_startup() {
	}

	void Application::on_shutdown() {
	}
}
