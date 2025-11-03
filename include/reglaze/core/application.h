#ifndef RGLZ_APPLICATION_H
#define RGLZ_APPLICATION_H

#include "base.h"
#include "core/logger.h"
#include "core/preferences.h"

namespace rglz {
	class Engine;

	class Application : public Logger<RGLZ_LOGGER_LOG_MAX_CUNCURRENT_LOGS> {
	public:
		Application(std::string_view file_name = "app_log_latest.txt", std::string_view logger_name = "APP");
		virtual ~Application();

		virtual void on_startup();

		virtual void on_shutdown();

		virtual void on_attribute_read(const Attribute& attribute);

		virtual void on_attribute_write(const Attribute& attribute);

		void set_preferences(const Preferences&);

		void write_attribute(std::string_view attribute_name, bool value);

		void write_attribute(std::string_view attribute_name, double value);

		void write_attribute(std::string_view attribute_name, const char* value);

		void write_attribute(std::string_view attribute_name, std::string_view);

		void write_attribute(std::string_view attribute_name);

		const Attribute* attribute(std::string_view attribute_name) const;

		inline std::optional<Preferences> preferences() {
			return m_preferences;
		}

	private:
		std::optional<Preferences> m_preferences;

		friend class Engine;
	};
}


#endif	// RGLZ_APPLICATION_H
