#include "reglaze.h"
#include <iostream>

using namespace rglz;

class App : public Application {
public:
	App() {
	}
};

RGLZ_REGISTER_CLIENT_APP(App);
RGLZ_MEMORY_PROFILE(4048, 0);
