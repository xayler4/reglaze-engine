#include "pch.h"
#include "memory/memory_manager.h"

namespace rglz {
	Logger<3> Engine::s_logger = Logger<3>("rglz_latest_log.txt", "RGLZ");
	Application* Engine::s_app = nullptr;

	void Engine::startup() {
		if (s_app->m_preferences.has_value()) {
			for (auto pair : s_app->m_preferences.value()) {
				s_app->on_attribute_read(pair.value);
			}
		}
	}

	void Engine::shutdown() {
		std::destroy_at(s_app);
		s_logger.flush_logs();
	}

	void Engine::run() {
		memory::MemoryManager::update();
		if (s_app->m_preferences.has_value()) {
			s_app->m_preferences.value().rebuild();
		}
		s_logger.flush_logs();
		s_app->flush_logs();
	}

	void* Engine::internal_allocate(std::size_t size) {
		return memory::MemoryManager::stack_allocator().allocate(size);
	}
}
