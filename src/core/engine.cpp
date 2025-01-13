#include "core/engine.h"
#include "memory/memory_manager.h"

namespace rglz {
	Application* Engine::s_app = nullptr;
	Logger<> Engine::s_logger = Logger("rglz_latest_log.txt", "RGLZ");

	void Engine::startup() {

	}

	void Engine::shutdown() {
		std::destroy_at(s_app);
	}

	void Engine::run() {
		s_logger.flush();
		s_app->flush_logger();
	}

	void* Engine::internal_allocate(std::size_t size) {
		return memory::MemoryManager::stack_allocator().allocate(size);
	}
}
