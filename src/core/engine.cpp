#include "core/engine.h"
#include "memory/memory_manager.h"

namespace rglz {
	Application* Engine::s_app = nullptr;

	void Engine::startup() {

	}

	void Engine::shutdown() {

	}

	void Engine::run() {

	}

	void* Engine::internal_allocate(std::size_t size) {
		return memory::MemoryManager::stack_allocator().allocate(size);
	}
}
