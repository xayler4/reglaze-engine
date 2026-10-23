#include "reglaze.h"

using namespace rglz;

ActionId forward_action;
ActionId back_action;
ActionId left_action;
ActionId right_action;

ActionId zoom_in_action;
ActionId zoom_out_action;

ActionId reset_action;

class CustomScene : public Scene {
public:
	CustomScene(const GPUImage& gpu_image) {
		m_image = gpu_image;
	}

	void on_init(std::size_t additional_stack_size) {

		int w = 500;
		int h = 500;
		reserve_components<TransformComponent>(w*h);
		reserve_components<SpriteComponent>(w*h);
		reserve_components<Camera2DComponent>(1);

		for (int i = 0; i < h; i++) {
			for (int j = 0; j < w; j++) {
				float x = (float)j*200;
				float y = (float)i*200;
				ecs::EntityId myentity = create_entity(TransformComponent({x, y, 0}, {100.0f, 100.0f, 1.0f}), SpriteComponent{m_image});

			}

		}

		ecs::EntityId camera = create_entity(Camera2DComponent({0, 0, 0}, {1.0f, 1.0f}));
		
		// add_components<TransformComponent>(myentity, TransformComponent{});
	}

private:
	GPUImage m_image;
};

class TestSystem : public System<TransformComponent, Camera2DComponent> {
public:
	void on_process(Scene& scene) {
		// std::span<TransformComponent> transform_component_view = get_component_view<TransformComponent>(scene);

		std::span<Camera2DComponent> camera_component_view = get_component_view<Camera2DComponent>(scene);

		for (Camera2DComponent& camera : camera_component_view) {
			Vec3 vertical{0.0f, 100.0f, 0.0f};
			Vec3 horizontal{100.0f, 0.0f, 0.0f};
			Vec2 zoom{0.05f, 0.05f};

			if (Engine::app().actions().is_pressed(reset_action)) {
				camera.pos = {0, 0 ,0};
				camera.zoom = {1.0f, 1.0f};
				return;
			}

			if (Engine::app().actions().is_down(forward_action)) {
				camera.pos -= vertical;
;
			}
			else if (Engine::app().actions().is_down(back_action)) {
				camera.pos += vertical;
			}
			else if (Engine::app().actions().is_down(right_action)) {
				camera.pos += horizontal;

			}
			else if (Engine::app().actions().is_down(left_action)) {
				camera.pos -= horizontal;
			}

			if (Engine::app().actions().is_down(zoom_in_action)) {
;
				camera.zoom += zoom;
			}
			else if (Engine::app().actions().is_down(zoom_out_action)) {
				if (camera.zoom.x > 0.1f) {
					camera.zoom -= zoom;
				}
			}
		}
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

		set_context_and_renderer(gfx::Context(1280, 720, "myGame", gfx::BackendType::SDL));

		// write_attribute("a", "Something_something");
		// write_attribute("abcdefg", "my other something something");
		// write_attribute("abcde", "hey there");
		// write_attribute("abcd", "well this is getting boring");
		// write_attribute("abcdef", 132831.231);

		{
			Image image = Engine::load_image("res/dragon.png");

			RGLZ_LOG_INFO(image.height, " ", image.width, " ", image.n_channels);
			gpu_image = renderer().create_gpu_image(image);
		}

		SceneMarker<CustomScene> scene = push_scene_stack<CustomScene>((sizeof(TransformComponent)+sizeof(SpriteComponent)) * (RGLZ_MAX_ENTITIES - 1) + sizeof(Camera2DComponent), gpu_image);
		// SceneMarker<CustomScene> scene1 = push_scene_stack<CustomScene>(64000);
		// SceneMarker<CustomScene> scene2 = push_scene_stack<CustomScene>(64000);
		
		test_system = &register_system<TestSystem>();
		sprite_extractor_system = &register_system<SpriteExtractorSystem>();
		camera_extractor_system = &register_system<CameraExtractorSystem>(1280, 720);
		std::array<SystemBase*, 3> main_pipeline = { test_system, sprite_extractor_system, camera_extractor_system };

		set_active_scene(scene, main_pipeline);


		forward_action = actions().create(ActionType::Button);
		back_action = actions().create(ActionType::Button);
		left_action = actions().create(ActionType::Button);
		right_action = actions().create(ActionType::Button);
		reset_action = actions().create(ActionType::Button);
		zoom_in_action = actions().create(ActionType::Button);
		zoom_out_action = actions().create(ActionType::Button);

		// reset_scene_stack_at_marker(scene);
	}

	~App() {
		renderer().destroy_gpu_image(gpu_image);
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
		switch (event.type) {
			case EventType::Quit: {
				quit();
			} break;
			case EventType::KeyDown: {
				if (event.key == rglz::Key::W) {
					actions().press(forward_action);
				}
				if (event.key == rglz::Key::S) {
					actions().press(back_action);
				}
				if (event.key == rglz::Key::A) {
					actions().press(left_action);
				}
				if (event.key == rglz::Key::D) {
					actions().press(right_action);
				}
				if (event.key == rglz::Key::R) {
					actions().press(reset_action);
				}
				if (event.key == rglz::Key::Q) {
					actions().press(zoom_out_action);
				}
				if (event.key == rglz::Key::E) {
					actions().press(zoom_in_action);
				}
			} break;
			case EventType::KeyUp: {
				if (event.key == rglz::Key::W) {
					actions().release(forward_action);
				}
				if (event.key == rglz::Key::S) {
					actions().release(back_action);
				}
				if (event.key == rglz::Key::A) {
					actions().release(left_action);
				}
				if (event.key == rglz::Key::D) {
					actions().release(right_action);
				}
				if (event.key == rglz::Key::R) {
					actions().release(reset_action);
				}
				if (event.key == rglz::Key::Q) {
					actions().release(zoom_out_action);
				}
				if (event.key == rglz::Key::E) {
					actions().release(zoom_in_action);
				}
			} break;
		}
	}

public:
	TestSystem* test_system;
	SpriteExtractorSystem* sprite_extractor_system;
	CameraExtractorSystem* camera_extractor_system;
	GPUImage gpu_image;
};

RGLZ_REGISTER_CLIENT_APP(App);
RGLZ_DEFAULT_APP_MEMORY_PROFILE({
	MemoryProfile profile;

	profile.allocation_size = 12800000;
	profile.core_stack_allocator_allocation_size_percent = 40.0;
	profile.scene_stack_allocator_allocation_size_percent = 30.0;
	profile.free_list_allocator_allocation_size_percent = 20.0;
	profile.single_frame_allocator_allocation_size_percent = 10.0;

	return profile;
});
