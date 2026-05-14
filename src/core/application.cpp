#include "pch.h"
#include "core/application.h"
#include "containers/fixed_hash_map.h"
#include "memory/memory_manager.h"

namespace rglz {
	Application::Application(std::string_view file_name, std::string_view logger_name) 
		: Logger(file_name, logger_name),
		m_is_running(true)
	{
	}

	Application::~Application() {
		flush_logs();
	}

	void Application::on_startup() {
	}

	void Application::on_shutdown() {
	}

	bool Application::on_attribute_read(const Attribute& attribute) {
		return false;
	}

	bool Application::on_attribute_write(const Attribute& attribute) {
		return false;
	}

	void Application::set_preferences(const Preferences& prefs) {
		m_preferences = prefs;
	}

	void* Application::internal_allocate(std::size_t size, std::size_t alignment) {
		return memory::MemoryManager::stack_allocator().allocate(size, alignment);
	}

	void Application::write_attribute(std::string_view attribute_name, bool value) {
		RGLZ_ASSERT(m_preferences.has_value());
		Attribute* attribute = m_preferences->write_attribute(attribute_name, value);
		on_attribute_write(*attribute);
	}

	void Application::write_attribute(std::string_view attribute_name, double value) {
		RGLZ_ASSERT(m_preferences.has_value());
		Attribute* attribute = m_preferences->write_attribute(attribute_name, value);
		on_attribute_write(*attribute);
	}

	void Application::write_attribute(std::string_view attribute_name, const char* value) {
		RGLZ_ASSERT(m_preferences.has_value());
		Attribute* attribute = m_preferences->write_attribute(attribute_name, value);
		on_attribute_write(*attribute);
	}

	void Application::write_attribute(std::string_view attribute_name, std::string_view value) {
		RGLZ_ASSERT(m_preferences.has_value());
		Attribute* attribute = m_preferences->write_attribute(attribute_name, value);
		on_attribute_write(*attribute);
	}

	void Application::write_attribute(std::string_view attribute_name) {
		RGLZ_ASSERT(m_preferences.has_value());
		Attribute* attribute = m_preferences->write_attribute(attribute_name);
		on_attribute_write(*attribute);
	}

	const Attribute* Application::attribute(std::string_view attribute_name) const {
		RGLZ_ASSERT(m_preferences.has_value());

		return m_preferences.value()[attribute_name];
	}
}
