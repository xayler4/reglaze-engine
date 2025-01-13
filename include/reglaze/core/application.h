#ifndef RGLZ_APPLICATION_H
#define RGLZ_APPLICATION_H

#include "base.h"
#include "core/logger.h"

namespace rglz {
	class Engine;

	class Application {
	public:
		Application();
		~Application();

		virtual void on_startup();

		virtual void on_shutdown();

	private:
		void flush_logger();

	private:
		Logger<> m_logger;

		friend class Engine;
	};
}


#endif	// RGLZ_APPLICATION_H
