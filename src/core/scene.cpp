#include "pch.h"
#include "core/scene.h"
#include "memory/memory_manager.h"

namespace rglz {
	const uint16_t SceneStack::s_max_thread_count = (std::thread::hardware_concurrency() - 1< RGLZ_MAX_THREADS) ? std::thread::hardware_concurrency() - 1 : RGLZ_MAX_THREADS;


	Scene::Scene() 
		: m_additional_stack_allocator(nullptr, 0) 
	{
		ecs::world_create(&m_world);
	}

	Scene::~Scene() {
		ecs::world_destroy(&m_world);
	}

	void Scene::destroy_entity(ecs::EntityId entity) {
		ecs::entity_destroy(&m_world, entity);
	}

	SceneStackNodeBase::SceneStackNodeBase() {
	}

	SceneStack::SceneStack() 
	{
		m_scene_node_head.prev_ptr = &m_scene_node_head;
		m_scene_node_head.next_ptr = &m_scene_node_head;
		m_scene_node_head.scene_ptr = nullptr;
		m_fiber_list_count = 0;
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
