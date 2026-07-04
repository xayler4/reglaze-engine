#ifndef RGLZ_SCENE_H
#define RGLZ_SCENE_H

#include "base.h"
#include "memory/stack_allocator.h"
#include "core/ecs.h"
#include <type_traits>
#include <utility>
#include <span>

#define RGLZ_MAX_THREADS 16
#define RGLZ_MAX_ALIGNMENT 64

namespace rglz {
	class SceneStack;

	class SystemBase;

	template<typename... TArgs>
	class System;

	template<typename T>
	concept IsSystem = std::is_base_of_v<SystemBase, std::decay_t<T>>;

	template<typename T, typename... TArgs>
	concept IsOneOf = (std::is_same_v<T, TArgs> || ...);


	class Scene {
	public:
		Scene();
		virtual ~Scene();

		template<typename T>
		inline static const ecs::ComponentId component_id() {
			static const ecs::ComponentId component_id = gen_component_id();

			return component_id;
		}

	protected:
		virtual void on_init() = 0;

		template<typename T>
		void allocate_component_list(std::size_t count) {
			memory::StackAllocator::Marker marker = m_additional_stack_allocator.allocate<T>(count);
			ecs::world_register_component_pool(&m_world, marker, count, component_id<T>(), sizeof(T));
		}

		template<typename... TArgs>
		ecs::EntityId create_entity(TArgs&&... components) {
			if constexpr (sizeof...(TArgs)) {
				ecs::ComponentDescriptor component_descriptor_list[sizeof...(TArgs)];
				component_descriptor_list_helper(component_descriptor_list, std::forward<TArgs>(components)...);
				return ecs::entity_create(&m_world, component_descriptor_list, sizeof...(TArgs));
			}
			else {
				return ecs::entity_create(&m_world, nullptr, 0);
			}
		}

		template<typename... TArgs>
		void add_components(ecs::EntityId entity, TArgs&&... components) {
			if constexpr (sizeof...(TArgs)) {
				ecs::ComponentDescriptor component_descriptor_list[sizeof...(TArgs)];
				component_descriptor_list_helper(&component_descriptor_list, components...);
				ecs::entity_add_components(&m_world, entity, component_descriptor_list, sizeof...(TArgs));
			}
			else {
				ecs::entity_add_components(&m_world, entity, nullptr, 0);
			}
		}

		template<typename... TArgs>
		void remove_components(ecs::EntityId entity) {
			if constexpr (sizeof...(TArgs)) {
				ecs::ComponentBitmask component_bitmask = 0;
				component_bitmask_helper<TArgs...>(component_bitmask);
				ecs::entity_remove_components(&m_world, entity, component_bitmask);
			}
		}

		template<typename T>
		void get_component(ecs::EntityId entity) {
			ecs::entity_get_component(&m_world, entity, component_id<T>());
		}

		void destroy_entity(ecs::EntityId entity);

		template<typename... TArgs>
		bool has_components(ecs::EntityId entity) {
			if constexpr (sizeof...(TArgs)) {
				ecs::ComponentBitmask component_bitmask = 0;
				component_bitmask_helper<TArgs...>(component_bitmask);
				return ecs::entity_has_components(&m_world, entity, component_bitmask);
			}
			return false;
		}

		template<typename T>
		std::span<T> get_component_view() {
			uint32_t count;
			void* component_list = ecs::world_get_component_list(&m_world, component_id<T>(), &count);
			return std::span<T>(static_cast<T*>(component_list), count);
		}

	private:
		inline static ecs::ComponentId gen_component_id() {
			static ecs::ComponentId id = 1;
			return id++;
		}

		template<typename T, typename... TArgs>
		inline void component_descriptor_list_helper(ecs::ComponentDescriptor* component_descriptor_p, T&& current, TArgs&&... rest) {
			ecs::component_descriptor_create(component_descriptor_p, &current, component_id<T>());

			if constexpr (sizeof...(TArgs)) {
				component_descriptors_list_helper(component_descriptor_p + 1, rest...);
			}
		}

		template<typename T, typename... TArgs>
		inline void component_bitmask_helper(ecs::ComponentBitmask& component_bitmask) {
			component_bitmask |= ecs::component_id_to_bitmask(component_id<T>());
			if constexpr (sizeof...(TArgs)) {
				component_bitmask_helper<TArgs...>(component_bitmask);
			}
		}

	private:
		ecs::World m_world;
		memory::StackAllocator m_additional_stack_allocator;

		friend class SceneStack;

		template<typename... TArgs>
		friend class System;
	};

	class SystemBase {
	private:
		SystemBase() = default;

		template<IsSystem T>
		static void component_system_func(ecs::World* world_p, void* data, void* shared_data) {
			(static_cast<T*>(data))->on_process(*(static_cast<Scene*>(shared_data)));
		}

		template<typename... TArgs>
		friend class System;
	};

	template<typename... TArgs>
	class System : public SystemBase {
	public:
		virtual void on_process(Scene& scene) = 0;

		template<IsOneOf<TArgs>... TComponents>
		inline ecs::EntityId create_entity(Scene& scene, TComponents&&... components) {
			return scene.create_entity(std::forward<TComponents>(components)...);
		}

		template<IsOneOf<TArgs>... TComponents>
		inline void add_components(Scene& scene, ecs::EntityId entity, TComponents&&... components) {
			scene.add_components(entity, std::forward<TComponents>(components)...);
		}

		template<IsOneOf<TArgs>... TComponents>
		inline void remove_components(Scene& scene, ecs::EntityId entity) {
			scene.remove_components<TComponents...>(entity);
		}

		template<IsOneOf<TArgs...> TComponent>
		inline void get_component(Scene& scene, ecs::EntityId entity) {
			scene.get_component<TComponent>(scene, entity);
		}

		inline void destroy_entity(Scene& scene, ecs::EntityId entity) {
			scene.destroy_entity(entity);
		}

		template<IsOneOf<TArgs>... TComponents>
		inline bool has_components(Scene& scene, ecs::EntityId entity) {
			return scene.has_components<TComponents...>(entity);
		}

		template<IsOneOf<TArgs...> TComponent>
		inline std::span<TComponent> get_component_view(Scene& scene) {
			return scene.get_component_view<TComponent>();
		}

	protected:
		System() = default;

	private:
		template<IsSystem T>
		static void get_component_system_descriptor(ecs::ComponentSystemDescriptor* descriptor, void* data) {

			descriptor->func = SystemBase::component_system_func<T>;
			descriptor->component_write_bitmask = 0;
			descriptor->component_read_bitmask = 0;
			descriptor->data_p = data;

			component_system_descriptor_helper<TArgs...>(descriptor);
			
		}

		template<typename T, IsSystem... TSystems>
		static void component_system_descriptor_helper(ecs::ComponentSystemDescriptor* component_system_descriptor_p) {
			if constexpr (std::is_const_v<T>) {
				component_system_descriptor_p->component_read_bitmask |= ecs::component_id_to_bitmask(Scene::component_id<T>);
			} 
			else {
				component_system_descriptor_p->component_write_bitmask |= ecs::component_id_to_bitmask(Scene::component_id<T>());
			}
			if constexpr (sizeof...(TSystems)) {
				component_system_descriptor_helper<TSystems...>(component_system_descriptor_p);
			}
		}


		friend class Scene;
		friend class SceneStack;
	};

	struct SceneStackNodeBase {
		SceneStackNodeBase();

		SceneStackNodeBase* prev_ptr;
		SceneStackNodeBase* next_ptr;
		Scene* scene_ptr;
	};

	template<typename TScene, typename = std::enable_if_t<std::is_base_of_v<Scene, TScene>>>
	class SceneStackNode : public SceneStackNodeBase {
	public:
		template<typename... TArgs>
		SceneStackNode(TArgs&&... args)
			: scene(std::forward<TArgs>(args)...)
		{
			scene_ptr = &scene;
		}

	public:
		TScene scene;
	};

	class SceneMarkerBase {
	protected:	
		SceneMarkerBase(Scene* ptr)
			: m_ptr(ptr) {

			}

	protected:
		inline operator Scene*() const {
			return m_ptr;
		}

		inline Scene* operator-> () const {
			return m_ptr;
		}

	private:
		Scene* m_ptr;

		friend class SceneStack;
	};

	template<typename TScene, typename = std::enable_if_t<std::is_base_of_v<Scene, TScene>>>
	class SceneMarker : public SceneMarkerBase {
	public:
		inline operator TScene*() const {
			return SceneMarkerBase::operator Scene*();
		}

		inline TScene* operator-> () const {
			return static_cast<TScene*>(SceneMarkerBase::operator->());
		}

	private:
		SceneMarker(TScene* ptr) 
			: SceneMarkerBase(ptr) 
		{}

		friend class SceneStack;
	};
	
	class SceneStack {
	public:
		SceneStack();
		~SceneStack();

	protected:
		template<typename TScene, typename... TArgs, typename = std::enable_if_t<std::is_base_of_v<Scene, TScene>>>
		SceneMarker<TScene> push_scene_stack(std::size_t additional_stack_size, TArgs&&... args) {
			SceneStackNodeBase* scene_node;
			SceneStackNodeBase* last_scene_node = m_scene_node_head.prev_ptr;
			additional_stack_size += RGLZ_MAX_ALIGNMENT;
			
			if (m_scene_node_head.scene_ptr == nullptr) {
				scene_node = &m_scene_node_head;
				void* raw = internal_allocate(sizeof(TScene) + additional_stack_size, alignof(TScene));
				std::construct_at(reinterpret_cast<TScene*>(raw), std::forward<TArgs>(args)...);
				scene_node->scene_ptr = reinterpret_cast<TScene*>(raw);
				scene_node->scene_ptr->m_additional_stack_allocator = memory::StackAllocator(static_cast<uint8_t*>(raw) + sizeof(TScene), additional_stack_size, "Scene Allocator");
			}
			else {
				scene_node = static_cast<SceneStackNodeBase*>(internal_allocate(sizeof(SceneStackNode<TScene>) + additional_stack_size, alignof(SceneStackNode<TScene>)));
				std::construct_at(static_cast<SceneStackNode<TScene>*>(scene_node), std::forward<TArgs>(args)...);
				scene_node->scene_ptr->m_additional_stack_allocator = memory::StackAllocator(reinterpret_cast<uint8_t*>(scene_node->scene_ptr) + sizeof(TScene), additional_stack_size, "Scene Allocator");
			}

			scene_node->next_ptr = &m_scene_node_head;
			scene_node->prev_ptr = last_scene_node;

			last_scene_node->next_ptr = scene_node;
			m_scene_node_head.prev_ptr = scene_node;

			scene_node->scene_ptr->on_init();

			return SceneMarker(static_cast<TScene*>(scene_node->scene_ptr));
		}

		std::optional<SceneMarkerBase> pop_scene_stack() {
			SceneStackNodeBase* last_scene_node = m_scene_node_head.prev_ptr;

			SceneStackNodeBase* new_last_scene_node = last_scene_node->prev_ptr;

			if (last_scene_node->scene_ptr == nullptr) {
				return {};
			}
			else if (last_scene_node == new_last_scene_node) {
				std::destroy_at(last_scene_node->scene_ptr);
				internal_deallocate(last_scene_node->scene_ptr);
				last_scene_node->scene_ptr = nullptr;

			}
			else {
				new_last_scene_node->next_ptr = &m_scene_node_head;
				m_scene_node_head.prev_ptr = new_last_scene_node;
				std::destroy_at(last_scene_node->scene_ptr);
				internal_deallocate(last_scene_node);
			}

			return SceneMarkerBase(new_last_scene_node->scene_ptr);
		}

		bool is_scene_valid(SceneMarkerBase marker) {
			Scene* current_marker = m_scene_node_head.prev_ptr->scene_ptr;
			if (marker > current_marker) {
				return false;
			}

			return true;
		}

		void reset_scene_stack_at_marker(SceneMarkerBase marker) {
			RGLZ_ENGINE_ASSERT(is_scene_valid(marker));
			while (pop_scene_stack().value() != marker);
			if (m_active_scene.has_value()) {
				if (!is_scene_valid(m_active_scene.value())) {
					reset_active_scene();
				}
			}
		}

		void clear_scene_stack() {
			while (pop_scene_stack().has_value());
			reset_active_scene();
		}

		template<IsSystem... TSystems>
		void set_active_scene(SceneMarkerBase marker, TSystems&&... systems) {
			RGLZ_ENGINE_ASSERT(is_scene_valid(marker));

			ecs::ComponentSystemDescriptor component_system_descriptor_list[sizeof...(TSystems)];

			m_active_scene = marker;

			if constexpr (sizeof...(TSystems)) {
				component_system_descriptor_helper<TSystems...>(component_system_descriptor_list, std::forward<TSystems>(systems)...);
			}

			ecs::component_system_descriptor_list_sort(component_system_descriptor_list, sizeof...(TSystems));
			m_fiber_list_count = ecs::component_system_fiber_list_create(component_system_descriptor_list, sizeof...(TSystems), m_component_system_fiber_list, s_max_thread_count);
		}

		void reset_active_scene() {
			m_active_scene.reset();
			m_fiber_list_count = 0;
		}

		std::optional<SceneMarkerBase> active_scene() {
			return m_active_scene;
		}

	private:
		void* internal_allocate(std::size_t size, std::size_t alignment);
		void internal_deallocate(void* marker);

		template<IsSystem T, IsSystem... TSystems>
		void component_system_descriptor_helper(ecs::ComponentSystemDescriptor* component_system_descriptor_p, T&& system, TSystems&&... systems) {
			std::decay_t<T>::template get_component_system_descriptor<std::decay_t<T>>(component_system_descriptor_p, &system);

			if constexpr (sizeof...(TSystems)) {
				component_system_descriptor_helper<TSystems...>(component_system_descriptor_p + 1, std::forward<TSystems>(systems)...);
			}
		}

		void run_systems() {
			ecs::component_system_fiber_list_run(m_component_system_fiber_list, m_fiber_list_count, &m_active_scene->m_ptr->m_world, m_active_scene->m_ptr);
		}

	private:
		static const uint16_t s_max_thread_count;
		uint16_t m_fiber_list_count;
		SceneStackNodeBase m_scene_node_head;
		std::optional<SceneMarkerBase> m_active_scene;
		ecs::ComponentSystemFiber m_component_system_fiber_list[RGLZ_MAX_THREADS];

		friend class Engine;
	};
}

#endif
