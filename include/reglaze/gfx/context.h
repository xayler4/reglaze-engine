#ifndef RGLZ_GFX_CONTEXT_H
#define RGLZ_GFX_CONTEXT_H

#include "base.h"
#include <cstdint>

namespace rglz {
	class Application;

	namespace gfx {

		enum class BackendType : uint8_t;

		struct Context {
			Context(uint16_t w, uint16_t h, std::string_view n, BackendType t) 
				: width(w)
				, height(h)
				, name(n)
				, backend_type(t)
			{

			}
			virtual void on_event_poll(Application* application) {

			};

			uint16_t width;
			uint16_t height;
			std::string_view name;	
			gfx::BackendType backend_type;
		};
	}
}

#endif	// RGLZ_GFX_CONTEXT_H

