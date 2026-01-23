#include "reglaze.h"

using namespace rglz;

class CustomScene : public Scene {
public:
	CustomScene() {
	}

	~CustomScene() override {
	}
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
			Attribute("abcdefg", "random_string")
		}});

		write_attribute("a", "Something_something");
		write_attribute("abcdefg", "my other something something");
		write_attribute("abcde", "hey there");
		write_attribute("abcd", "well this is getting boring");
		write_attribute("abcdef", 132831.231);

		SceneMarker<CustomScene> scene = emplace_scene_stack<CustomScene>();
		SceneMarker<CustomScene> scene1 = emplace_scene_stack<CustomScene>();
		SceneMarker<CustomScene> scene2 = emplace_scene_stack<CustomScene>();

		reset_scene_stack_at_marker(scene);
	}

	~App() {
		RGLZ_LOG_INFO("App shutdown");
	}

	void on_attribute_read(const Attribute& attribute) override {
		RGLZ_LOG_INFO_GET(log, "Read ");
		RGLZ_LOG_INFO_TO(log, attribute);
	}

	void on_attribute_write(const Attribute& attribute) override {
		RGLZ_LOG_INFO_GET(log, "Write ");
		RGLZ_LOG_INFO_TO(log, attribute);
	}
};

RGLZ_REGISTER_CLIENT_APP(App);
RGLZ_DEFAULT_APP_MEMORY_PROFILE({
	MemoryProfile profile;	

	profile.allocation_size = 320000;
	profile.stack_allocator_allocation_size_percent = 50.0;
	profile.scene_stack_allocator_allocation_size_percent = 40.0;
	profile.single_frame_allocator_allocation_size_percent = 10.0;

	return profile;
});

