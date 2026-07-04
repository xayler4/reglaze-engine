#include "reglaze.h"

using namespace rglz;

struct TransformComponent {
	int x;
	int y;
};

template<ESR_REGISTRY_PARAMS>
ESR_REGISTER_PROC_W("Logger", TransformComponent, t, stream,
	ESR_PACK({
		stream << "[ x: " << t.x << ", y: " << t.y << " ]";	
	})
);

class CustomScene : public Scene {
public:
	void on_init() {
		allocate_component_list<TransformComponent>(200);
		ecs::EntityId myentity = create_entity(TransformComponent{333});
		destroy_entity(myentity);
	}
};

class TestSystem : public System<TransformComponent> {
public:
	void on_process(Scene& scene) {
		std::span<TransformComponent> transform_component_view = get_component_view<TransformComponent>(scene);
		for (TransformComponent& t : transform_component_view) {
			// RGLZ_LOG_INFO(t);
		}
	}
};

template<typename T>
class UpdateGpuSystem : public System<const T> {
public:

	// void on_process(Scene& scene) {
	// 	std::span<const T> view = get_component_view<const T>(scene);

	// 	for (const T& v : view) {
	// 		stream << v;
	// 	}

	// 	stream.upload();
	// }

private:
	// GpuBufferStream stream;
};

class App : public Application {
public:
	App() {
		RGLZ_LOG_INFO("App initialized");

		set_preferences({"options.cfg", {
			Attribute("a", false),
			Attribute("ab", 7.0),
			Attribute("abc", 20.5),
			Attribute("abcd", "my_personal_data"),
			Attribute("abcde"),
			Attribute("abcdef", true),
			Attribute("abcdefg", "random_string"),
		}});

		set_context_and_renderer(gfx::Context(1280, 720, "myGame", gfx::BackendType::SDL), 1280, 720);

		write_attribute("a", "Something_something");
		write_attribute("abcdefg", "my other something something");
		write_attribute("abcde", "hey there");
		write_attribute("abcd", "well this is getting boring");
		write_attribute("abcdef", 132831.231);

		SceneMarker<CustomScene> scene = push_scene_stack<CustomScene>(32000);
		SceneMarker<CustomScene> scene1 = push_scene_stack<CustomScene>(64000);
		SceneMarker<CustomScene> scene2 = push_scene_stack<CustomScene>(64000);
		
		test_system = &register_system<TestSystem>();

		set_active_scene(scene, *test_system);

		// register_input<bool>("forward");

		// reset_scene_stack_at_marker(scene);
	}

	~App() {
		RGLZ_LOG_INFO("App shutdown");
	}
	
	bool on_attribute_read(const Attribute& attribute) override {
		RGLZ_LOG_INFO_GET(log, "Read ");
		RGLZ_LOG_INFO_TO(log, attribute);

		return false;
	}

	bool on_attribute_write(const Attribute& attribute) override {
		RGLZ_LOG_INFO_GET(log, "Write ");
		RGLZ_LOG_INFO_TO(log, attribute);

		return false;
	}

	void on_event(const Event& event) override {
		if (event.type == EventType::Quit) {
			quit();
		}
	}

	TestSystem* test_system;
};

RGLZ_REGISTER_CLIENT_APP(App);
RGLZ_DEFAULT_APP_MEMORY_PROFILE({
	MemoryProfile profile;	

	profile.allocation_size = 1280000;
	profile.core_stack_allocator_allocation_size_percent = 50.0;
	profile.scene_stack_allocator_allocation_size_percent = 40.0;
	profile.single_frame_allocator_allocation_size_percent = 10.0;

	return profile;
});
