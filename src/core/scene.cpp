#include "pch.h"
#include "core/scene.h"
#include "memory/memory_manager.h"

namespace rglz {
	SceneStack::SceneStack() 
	{
		m_scene_node_head.prev_ptr = &m_scene_node_head;
		m_scene_node_head.next_ptr = &m_scene_node_head;
		m_scene_node_head.scene_ptr = nullptr;
	}

	SceneStack::~SceneStack() {
		memory::MemoryManager::scene_stack_allocator().clear();
	}

	void* SceneStack::internal_allocate(std::size_t size, std::size_t alignment) {
		return memory::MemoryManager::scene_stack_allocator().allocate(size, alignment);
	}

	void SceneStack::internal_deallocate(void* marker) {
		memory::MemoryManager::scene_stack_allocator().reset_at_marker(memory::StackAllocator::Marker(static_cast<std::uint8_t*>(marker)));
	}
}
