#include "reglaze.h"

using namespace rglz;

class Menu : public Scene<TransformComponent, RendererComponent> {
	Menu() {
	}

	~Menu() {
		
	}
};

class Hub : public Scene<TransformComponent, RendererComponent> {
	Hub() {
	}

	~Hub() {
		
	}
};

class Match : public Scene<TransformComponent, RendererComponent> {
	Match() {
	}

	~Match() {
		
	}
};

class System : public System<TransformComponent, FireAttributeComponent> {
	void on_process(ccnt::vector<Scene>& scenes) override {
		for (Scene& scene : scenes) {
			for (TransformComponent& t : scene.view<TransformComponent>()) {
				t.x = 2.0f;
			}
		}
	}
};

class Renderer : public RendererSystem<const TransformComponent, const FireAttributeComponent> {
	void on_process(const ccnt::vector<Scene>& scenes) override {
		
	}
}


class App : public Application {
public:
	App() {
		log(LoggerSeverity::Info) << "App initialized";
		load_options("options.txt", 
			Option("RendererBackend", OptionType::STRING, "Vullkan", OptionFlag::LOAD_AFTER_SET | OptionFlag::CONCEAL);
			Option("MaxFPS", OptionType::INTEGER, 60, OptionFlag::LOAD_ONLY_AT_STARTUP | OptionFlag::EXPOSE);
		)

		set_option("RendererBackend", "OpenGL");
	}

	on_option_load(const Option& option) {
		if (option.name == "RendererBackend") {
			
		}
		else if 

	}

	~App() {
		log(LoggerSeverity::Info) << "App shutdown";
	}
};

RGLZ_REGISTER_CLIENT_APP(App);

