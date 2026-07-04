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

	void Application::on_event(const Event& event) {

	}

	void Application::set_preferences(const Preferences& prefs) {
		m_preferences = prefs;
	}

	void Application::set_context_and_renderer(gfx::Context ctx, uint16_t renderer_width, uint16_t renderer_height) {
    	using underlying_t = std::underlying_type_t<gfx::BackendType>;

		bool has_backend_failed;
		uint8_t n_tried_backend = 0;
		do {
			has_backend_failed = false;

			if (n_tried_backend) {
				RGLZ_ENGINE_LOG_INFO("Falling back to ", backend_type_to_str(ctx.backend_type), " backend");
			}

			try {
				m_context = std::move(gfx::context_variant_create(ctx));
				m_renderer = std::move(gfx::renderer_variant_create(context(), renderer_width, renderer_height));
			}
			catch (const BackendError& err) {
				has_backend_failed = true;

				RGLZ_ENGINE_LOG_ERROR(backend_type_to_str(ctx.backend_type), " backend initialization failed!");

				ctx.backend_type = gfx::BackendType(underlying_t(ctx.backend_type) + 1);
				if (underlying_t(ctx.backend_type) == underlying_t(gfx::BackendType::Count)) {
					ctx.backend_type = gfx::BackendType(0);	
				}
			}

			RGLZ_ENGINE_ALWAYS_ASSERT_MSG(n_tried_backend++ != underlying_t(gfx::BackendType::Count), "Initialization failed for all available backends!");
	}
	while(has_backend_failed);
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
