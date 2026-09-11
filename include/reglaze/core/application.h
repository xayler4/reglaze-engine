#ifndef RGLZ_APPLICATION_H
#define RGLZ_APPLICATION_H

#include "base.h"
#include "core/logger.h"
#include "core/preferences.h"
#include "core/scene.h"
#include "core/events.h"
#include "core/action_manager.h"
#include "gfx/sdl_renderer.h"
#include <variant>

namespace rglz {
	class Engine;

	class Application : 
		public Logger<RGLZ_LOGGER_LOG_MAX_CUNCURRENT_LOGS>,
		public SceneStack
	{
	public:
		Application(std::string_view file_name = "app_log_latest.txt", std::string_view logger_name = "APP");
		virtual ~Application();

		inline void quit() {
			m_is_running = false;
		}

		inline const ActionManager& actions() const {
			return m_actions;
		}

	protected:
		virtual void on_startup();

		virtual void on_shutdown();

		bool on_state_change(std::string_view state);

		void set_preferences(const Preferences& prefs);

		void set_context_and_renderer(gfx::Context ctx, uint16_t renderer_width, uint16_t renderer_height);

		void write_attribute(std::string_view attribute_name, bool value);

		void write_attribute(std::string_view attribute_name, double value);

		void write_attribute(std::string_view attribute_name, const char* value);

		void write_attribute(std::string_view attribute_name, std::string_view);

		void write_attribute(std::string_view attribute_name);

		const Attribute* attribute(std::string_view attribute_name) const;

		template<IsSystem T, typename... TArgs>
		T& register_system(TArgs&&... args) {
			T* system_out = static_cast<T*>(internal_allocate(sizeof(T), alignof(T)));
			std::construct_at(system_out, std::forward<TArgs>(args)...);
			return *system_out;
		}


		inline std::optional<Preferences> preferences() {
			return m_preferences;
		}

		inline gfx::Renderer& renderer() {
			RGLZ_ENGINE_ASSERT(m_renderer.has_value());
			return std::visit([](auto& r) -> gfx::Renderer& {
				return r;
			}, m_renderer.value());
		}

		inline gfx::Context& context() {
			RGLZ_ENGINE_ASSERT(m_context.has_value());
			return std::visit([](auto& c) -> gfx::Context& {
				return c;
			}, m_context.value());
		}

		inline ActionManager& actions() {
			return m_actions;
		}

	private:
		virtual bool on_attribute_read(const Attribute& attribute);

		virtual bool on_attribute_write(const Attribute& attribute);

		virtual void on_event(const Event& event);
		void* internal_allocate(std::size_t size, std::size_t alignment);

	private:
		bool m_is_running;
		std::optional<Preferences> m_preferences;
		std::optional<gfx::ContextVariant> m_context;
		std::optional<gfx::RendererVariant> m_renderer;
		ActionManager m_actions;

		friend class Engine;
		PLATFORM_FRIEND_BACKEND_CONTEXT_CLASSES
	};
}

#endif	// RGLZ_APPLICATION_H
