#include "pch.h"
#include "platform/platform.h"

#if defined RGLZ_PLATFORM_LINUX

namespace rglz {
	namespace gfx {
		ContextVariant context_variant_create(const gfx::Context& ctx) {
			switch (ctx.backend_type) {
				case gfx::BackendType::SDL:
				case gfx::BackendType::SDLVulkan: {
					return SDLContext(ctx);
				} break;

			}

			return SDLContext(ctx);
		}

		RendererVariant renderer_variant_create(gfx::Context& ctx, uint16_t renderer_width, uint16_t renderer_height) {
			switch (ctx.backend_type) {
				case gfx::BackendType::SDL: {
					return SDLRenderer(static_cast<SDLContext&>(ctx), renderer_width, renderer_height);
				} break;
				case gfx::BackendType::SDLVulkan: {
					return SDLRendererVulkan(static_cast<SDLContext&>(ctx), renderer_width, renderer_height);
				} break;

			}

			return SDLRenderer(static_cast<SDLContext&>(ctx), renderer_width, renderer_height);
		}

		std::string_view backend_type_to_str(BackendType type) {
			switch (type) {
				case gfx::BackendType::SDL: {
					return "SDL";
				} break;
				case gfx::BackendType::SDLVulkan: {
					return "SDLVulkan";
				} break;
				default: {
					return "";
				}
			}
		}
	}
}

#endif
