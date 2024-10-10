#ifndef RGLZ_APPLICATION_H
#define RGLZ_APPLICATION_H

#include "base.h"

namespace rglz {
	class Application {
	public:
		Application();
		~Application();

		virtual void on_startup();

		virtual void on_shutdown();
	};
}


#endif	// RGLZ_APPLICATION_H
