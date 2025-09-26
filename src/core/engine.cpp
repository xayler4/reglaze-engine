#include "pch.h"
#include "core/engine.h"
#include "memory/memory_manager.h"
#include "core/preferences.h"

namespace rglz {
	Application* Engine::s_app = nullptr;
	Logger<3> Engine::s_logger = Logger<3>("rglz_latest_log.txt", "RGLZ");

	void Engine::startup() {
		Preferences pref("options.txt", {
			Attribute("a", false),
			Attribute("ab", 7.0),
			Attribute("abc", 20.5),
			Attribute("abcd", "my_personal_data"),
			Attribute("abcde"),
			Attribute("abcdef", true),
			Attribute("abcdefg", "random_string")
		});
	}

	void Engine::shutdown() {
		std::destroy_at(s_app);
		s_logger.flush_logs();
	}

	void Engine::run() {
		memory::MemoryManager::update();
		s_app->flush_logs();
		s_logger.flush_logs();
	}

	void* Engine::internal_allocate(std::size_t size) {
		return memory::MemoryManager::stack_allocator().allocate(size);
	}
}
