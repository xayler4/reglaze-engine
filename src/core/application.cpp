#include "pch.h"
#include "core/application.h"
#include "containers/fixed_hash_map.h"

namespace rglz {
	Application::Application(std::string_view file_name, std::string_view logger_name) : Logger(file_name, logger_name) {
	}

	Application::~Application() {
		flush_logs();
	}

	void Application::on_startup() {
	}

	void Application::on_shutdown() {
	}
}
