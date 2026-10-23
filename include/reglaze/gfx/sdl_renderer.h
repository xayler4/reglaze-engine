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

			struct FrameData {
				SDL_GPUTexture* swapchain_texture = nullptr;
				SDL_GPUCommandBuffer* primary_command_buffer = nullptr;
				SDL_GPUTransferBuffer* sprite_instance_transfer_buffer;
				SDL_GPUBuffer* sprite_instance_buffer;
			};

		public:
			SDLRenderer(SDLContext& ctx, const char* sdl_backend_id = "");
			~SDLRenderer() override;

			void on_begin() override;
			void on_end() override;

			GPUImage create_gpu_image(const Image& image) override;
			void destroy_gpu_image(const GPUImage& image) override;

			inline FrameData& frame_data() {
				return m_frame_data_list[current_frame_index()];
			}

		private:
			SDL_GPUShader* create_shader_from_memory(const uint8_t* code, std::size_t code_size, SDL_GPUShaderStage stage,
					uint8_t num_samplers = 0,
					uint8_t num_storage_textures = 0,
					uint8_t num_storage_buffers = 0,
					uint8_t num_uniform_buffers = 0);

		private:
			Cache m_cache;
			std::array<FrameData, RGLZ_RENDERER_MAX_FRAMES_IN_FLIGHT> m_frame_data_list;
			SDL_GPUDevice* m_device;
			SDL_GPUBuffer* m_quad_buffer;
			SDL_GPUSampler* m_sprite_sampler;
			SDL_GPUGraphicsPipeline* m_sprite_pipeline;
		};

		class SDLRendererVulkan : public SDLRenderer {
		public:
			SDLRendererVulkan(gfx::SDLContext& ctx);
		};
	}	
}

#endif	// RGLZ_GFX_SDL_RENDERER_H
