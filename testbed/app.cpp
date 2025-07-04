#include "reglaze.h"

using namespace rglz;

class App : public Application {
public:
	App() {
		RGLZ_LOG_INFO("App initialized");
	}

	~App() {
		RGLZ_LOG_INFO("App shutdown");
	}
};

RGLZ_REGISTER_CLIENT_APP(App);
