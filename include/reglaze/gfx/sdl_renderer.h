#ifndef RGLZ_GFX_SDL_RENDERER_H
#define RGLZ_GFX_SDL_RENDERER_H

#include "base.h"
#include "gfx/renderer.h"
#include "gfx/sdl_context.h"
#include "containers/fixed_string.h"
#include <cstdint>

namespace rglz {
	namespace gfx {
		class SDLRenderer : public Renderer {
		public:
			struct Cache {
				Cache();

				SDL_GPUColorTargetInfo color_target_info;
			};

		public:
			SDLRenderer(SDLContext& ctx, uint16_t w, uint16_t h, const char* sdl_backend_id = "");
			SDLRenderer(SDLRenderer&& sdl_renderer);

			virtual ~SDLRenderer();

			void on_begin() override;
			void on_end() override;

			SDLRenderer& operator= (SDLRenderer&& sdl_renderer);

		private:
			Cache m_cache;
			SDL_GPUDevice* m_device;
			SDL_GPUTexture* m_swapchain_texture;
			SDL_GPURenderPass* m_render_pass;
			SDL_GPUCommandBuffer* m_primary_command_buffer;
			bool m_moved;
		};

		class SDLRendererVulkan : public SDLRenderer {
		public:
			SDLRendererVulkan(gfx::SDLContext& ctx, uint16_t w, uint16_t h);
		};
	}	
}

#endif	// RGLZ_GFX_SDL_RENDERER_H
