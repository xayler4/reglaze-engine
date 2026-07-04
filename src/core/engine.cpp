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
		// if (s_app->m_preferences.has_value()) {
		// 	s_app->m_preferences.value().rebuild();

		// 	s_logger.flush_logs();
		// }
		while (s_app->m_is_running) {
			s_app->context().on_event_poll(s_app);

			s_app->renderer().on_begin();
			if (s_app->active_scene().has_value()) {
				s_app->run_systems();
			}

			s_app->renderer().on_end();

			s_logger.flush_logs();
			s_app->flush_logs();
			memory::MemoryManager::update();

		}
	}

	void* Engine::internal_allocate(std::size_t size, std::size_t alignment) {
		return memory::MemoryManager::stack_allocator().allocate(size, alignment);
	}
}
