#include "pch.h"
#include "core/scene.h"
#include "memory/memory_manager.h"

namespace rglz {
	const uint16_t SceneStack::s_max_thread_count = 
		(std::thread::hardware_concurrency() > 1)
		? ((std::thread::hardware_concurrency() - 1 < RGLZ_MAX_THREADS)
			? (std::thread::hardware_concurrency() - 1)
			: RGLZ_MAX_THREADS)
		: 1;


	Scene::Scene() 
		: m_additional_stack_allocator(nullptr, 0, "Scene Allocator") 
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

	void SceneStack::set_active_scene(SceneMarkerBase marker, const std::span<SystemBase*> system_list) {
		RGLZ_ENGINE_ASSERT(is_scene_valid(marker));

ecs::ComponentSystemDescriptor* component_system_descriptor_list = 
        reinterpret_cast<ecs::ComponentSystemDescriptor*>(
            static_cast<uint8_t*>(memory::MemoryManager::single_frame_allocator().allocate(
                sizeof(ecs::ComponentSystemDescriptor) * system_list.size(), 
                alignof(ecs::ComponentSystemDescriptor)
            ))
        );

    m_active_scene = marker;

    for (size_t i = 0; i < system_list.size(); ++i) {
        system_list[i]->get_component_system_descriptor_dynamic(&component_system_descriptor_list[i]);
    }

    ecs::component_system_descriptor_list_sort(component_system_descriptor_list, system_list.size());
    m_fiber_list_count = ecs::component_system_fiber_list_create(
        component_system_descriptor_list, 
        system_list.size(), 
        m_component_system_fiber_list, 
        s_max_thread_count
    );
	}

	void* SceneStack::internal_allocate(std::size_t size, std::size_t alignment) {
		return memory::MemoryManager::scene_stack_allocator().allocate(size, alignment);
	}

	void SceneStack::internal_deallocate(void* marker) {
		memory::MemoryManager::scene_stack_allocator().reset_at_marker(memory::StackAllocator::Marker(static_cast<std::uint8_t*>(marker)));
	}
}
