#ifndef RGLZ_GFX_SDL_CONTEXT_H
#define RGLZ_GFX_SDL_CONTEXT_H

#include "base.h"
#include "gfx/context.h"
#include <SDL3/SDL.h>
#include <cstdint>

namespace rglz {
	namespace gfx {
		struct SDLContext : public Context {
			SDLContext(const Context& context);

			SDLContext(SDLContext&& sdl_context);
			SDLContext& operator= (SDLContext&& sdl_context);

			void on_event_poll(Application* app) override;

			~SDLContext();

			SDLContext(const SDLContext&) = delete;
			SDLContext& operator= (const SDLContext&) = delete;

			SDL_Window* window;
			bool valid;

		private:
			bool m_moved;
		};
	}
}

#endif	// RGLZ_GFX_SDL_CONTEXT_H

