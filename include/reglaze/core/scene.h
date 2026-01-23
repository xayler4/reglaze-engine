#ifndef RGLZ_SCENE_H
#define RGLZ_SCENE_H

#include "base.h"
#include <type_traits>

namespace rglz {

	class SceneStack;

	class Scene {
	public:
		virtual ~Scene() = default;
	};

	struct SceneStackNodeBase {
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

		template<typename TScene, typename... TArgs, typename = std::enable_if_t<std::is_base_of_v<Scene, TScene>>>
		SceneMarker<TScene> emplace_scene_stack(TArgs&&... args) {
			SceneStackNodeBase* scene_node;
			SceneStackNodeBase* last_scene_node = m_scene_node_head.prev_ptr;

			if (m_scene_node_head.scene_ptr == nullptr) {
				scene_node = &m_scene_node_head;
				void* raw = internal_allocate(sizeof(TScene), alignof(TScene));
				std::construct_at(reinterpret_cast<TScene*>(raw), std::forward<TArgs>(args)...);
				scene_node->scene_ptr = reinterpret_cast<TScene*>(raw);
			}
			else {
				scene_node = static_cast<SceneStackNodeBase*>(internal_allocate(sizeof(SceneStackNode<TScene>), alignof(SceneStackNode<TScene>)));
				std::construct_at(static_cast<SceneStackNode<TScene>*>(scene_node), std::forward<TArgs>(args)...);
			}

			scene_node->next_ptr = &m_scene_node_head;
			scene_node->prev_ptr = last_scene_node;

			last_scene_node->next_ptr = scene_node;
			m_scene_node_head.prev_ptr = scene_node;

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
		}

		void clear_scene_stack() {
			while (pop_scene_stack().has_value());
		}

	private:
		void* internal_allocate(std::size_t size, std::size_t alignment);
		void internal_deallocate(void* marker);

	private:
		SceneStackNodeBase m_scene_node_head;
	};
}

#endif
