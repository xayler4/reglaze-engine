#include "pch.h"
#include "memory/stack_allocator.h"
#include "platform/platform.h"

#if defined RGLZ_PLATFORM_LINUX

namespace rglz {
	namespace gfx {
		ContextVariant context_variant_create(const gfx::Context& ctx) {
			switch (ctx.backend_type) {
				case gfx::BackendType::SDL:
				case gfx::BackendType::SDLVulkan: 
				default: {
					return SDLContext(ctx);
				} break;

			}
		}

		Renderer* renderer_create(gfx::Context& ctx, memory::StackAllocator& allocator) {
			switch (ctx.backend_type) {
				case gfx::BackendType::SDLVulkan: {
					SDLRendererVulkan* ptr = static_cast<SDLRendererVulkan*>(allocator.allocate<SDLRendererVulkan>(1).ptr());
					std::construct_at(ptr, static_cast<SDLContext&>(ctx));
					return ptr;
				} break;
				case gfx::BackendType::SDL: 
				default: {
					SDLRenderer* ptr = static_cast<SDLRenderer*>(allocator.allocate<SDLRenderer>(1).ptr());
					std::construct_at(ptr, static_cast<SDLContext&>(ctx));
					return ptr;
				} break;
			}
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
