#ifndef RGLZ_APPLICATION_H
#define RGLZ_APPLICATION_H

#include "base.h"
#include "core/logger.h"
#include "core/preferences.h"
#include "core/scene.h"

namespace rglz {
	class Engine;

	class Application : 
		public Logger<RGLZ_LOGGER_LOG_MAX_CUNCURRENT_LOGS>,
		public SceneStack {
	public:
		Application(std::string_view file_name = "app_log_latest.txt", std::string_view logger_name = "APP");
		virtual ~Application();

		virtual void on_startup();

		virtual void on_shutdown();

		bool on_state_change(std::string_view state);

		virtual bool on_attribute_read(const Attribute& attribute);

		virtual bool on_attribute_write(const Attribute& attribute);

		void set_preferences(const Preferences&);

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

		inline void quit() {
			m_is_running = false;
		}
	private:
		void* internal_allocate(std::size_t size, std::size_t alignment);

	private:
		bool m_is_running;
		std::optional<Preferences> m_preferences;

		friend class Engine;
	};
}


#endif	// RGLZ_APPLICATION_H
