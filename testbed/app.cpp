#include "reglaze.h"

using namespace rglz;

class App : public Application {
public:
	App() {
		RGLZ_LOG_INFO("App initialized");
		// load_options("options.txt", 
		// 	Option("RendererBackend", OptionType::STRING, "Vullkan", OptionFlag::LOAD_AFTER_SET | OptionFlag::CONCEAL);
		// 	Option("MaxFPS", OptionType::INTEGER, 60, OptionFlag::LOAD_ONLY_AT_STARTUP | OptionFlag::EXPOSE);
		// )

		// set_option("RendererBackend", "OpenGL");
	}

	// on_option_load(const Option& option) {
	// 	if (option.name == "RendererBackend") {
			
	// 	}
	// 	else if 

	// }

	~App() {
		RGLZ_LOG_INFO("App shutdown");
	}
};

RGLZ_REGISTER_CLIENT_APP(App);
RGLZ_MEMORY_PROFILE_DEFAULT_AND_MIN_ALLOC_SIZE(16*1024, 0);
