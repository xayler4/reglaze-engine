#ifndef RGLZ_PLATFORM_H
#define RGLZ_PLATFORM_H

#if defined __linux__ || __linux || linux
	#define RGLZ_PLATFORM_LINUX
#elif defined _WIN32
	#define RGLZ_PLATFORM_WINDOWS
#endif

#include "gfx/sdl_context.h"
#include "gfx/sdl_renderer.h"
#include <variant>

namespace rglz {
	namespace gfx {
	#if defined RGLZ_PLATFORM_LINUX
		enum class BackendType : uint8_t {
			SDL,
			SDLVulkan,
			Count
		};

		#define PLATFORM_FRIEND_BACKEND_CONTEXT_CLASSES \
			friend class gfx::SDLContext;

		using ContextVariant = std::variant<SDLContext>;
		using RendererVariant = std::variant<SDLRenderer>;
	#endif

		ContextVariant context_variant_create(const gfx::Context& ctx);
		RendererVariant renderer_variant_create(gfx::Context& ctx, uint16_t renderer_width, uint16_t renderer_height);
		std::string_view backend_type_to_str(BackendType type);

	}
}


#endif	// RGLZ_PLATFORM_H
