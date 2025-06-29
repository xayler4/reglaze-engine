#include "reglaze.h"

using namespace rglz;

class App : public Application {
public:
	App() {
		log(LoggerSeverity::Info) << "App initialized";

	}

	~App() {
		log(LoggerSeverity::Info) << "App shutdown";

	}
};

RGLZ_REGISTER_CLIENT_APP(App);
