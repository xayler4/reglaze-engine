#include "pch.h"
#include "gfx/sdl_renderer.h"

namespace rglz {
	namespace gfx {
		SDLRenderer::Cache::Cache() {
			color_target_info = {};
			color_target_info.clear_color = {0.0f, 0.0f, 0.0f, 1.0f};
			color_target_info.load_op = SDL_GPU_LOADOP_CLEAR;
			color_target_info.store_op = SDL_GPU_STOREOP_STORE;
		}

		SDLRenderer::SDLRenderer(SDLContext& ctx, uint16_t w, uint16_t h, const char* sdl_backend_id) 
			: Renderer(ctx, w, h)
			, m_device(nullptr)
			, m_moved(false)
		{	
			SDLContext& sdl_context = static_cast<SDLContext&>(context());
			m_device = SDL_CreateGPUDevice(SDL_GPU_SHADERFORMAT_SPIRV, true, sdl_backend_id);
			if (m_device == nullptr) {
				RGLZ_ENGINE_LOG_ERROR("Failed to create SDL GPUDevice: ", std::string_view(SDL_GetError()));
				throw BackendError{};
			}

			if (!SDL_ClaimWindowForGPUDevice(m_device, sdl_context.window)) {
				RGLZ_ENGINE_LOG_ERROR("Failed to claim SDL Window for SDL GPUDevice: ", std::string_view(SDL_GetError()));
				throw BackendError{};
			}

		}

		SDLRenderer::SDLRenderer(SDLRenderer&& sdl_renderer)
			: Renderer(std::move(sdl_renderer))
			, m_device(sdl_renderer.m_device)
			, m_moved(sdl_renderer.m_moved)
		{
			sdl_renderer.m_device = nullptr;
			sdl_renderer.m_moved = true;
		}

		SDLRenderer& SDLRenderer::operator= (SDLRenderer&& sdl_renderer) {
			Renderer::operator = (std::move(sdl_renderer));
			m_device = sdl_renderer.m_device;
			m_moved = sdl_renderer.m_moved;

			sdl_renderer.m_device = nullptr;
			sdl_renderer.m_moved = true;

			return *this;
		}

		SDLRenderer::~SDLRenderer() {
			if (m_device != nullptr && !m_moved) {
				SDLContext& sdl_context = static_cast<SDLContext&>(context());
				if (sdl_context.window != nullptr) {
					SDL_ReleaseWindowFromGPUDevice(m_device, sdl_context.window);
				}
				SDL_DestroyGPUDevice(m_device);
			}
		}

		void SDLRenderer::on_begin() {
			SDLContext& sdl_context = static_cast<SDLContext&>(context());

			SDL_WindowFlags flags = SDL_GetWindowFlags(sdl_context.window);
			if (flags & (SDL_WINDOW_MINIMIZED | SDL_WINDOW_HIDDEN | SDL_WINDOW_OCCLUDED)) {
				sdl_context.valid = false;
				return;
			}

			m_primary_command_buffer = SDL_AcquireGPUCommandBuffer(m_device);
			if (!m_primary_command_buffer) {
				sdl_context.valid = false;
				return;
			}

			uint32_t w, h;
			bool acquired = SDL_WaitAndAcquireGPUSwapchainTexture(
				m_primary_command_buffer, sdl_context.window, &m_swapchain_texture, &w, &h);

			if (acquired && m_swapchain_texture != nullptr) {
				sdl_context.width = (uint16_t)w;    
				sdl_context.height = (uint16_t)h;    
				sdl_context.valid = true;

				m_cache.color_target_info.texture = m_swapchain_texture;
				m_render_pass = SDL_BeginGPURenderPass(m_primary_command_buffer, &m_cache.color_target_info, 1, nullptr);
			} 
			else {
				sdl_context.valid = false;
			}
		}

		void SDLRenderer::on_end() {
			SDLContext& sdl_context = static_cast<SDLContext&>(context());

			if (sdl_context.valid) {
				SDL_EndGPURenderPass(m_render_pass);
				SDL_SubmitGPUCommandBuffer(m_primary_command_buffer);
			}
		}

		SDLRendererVulkan::SDLRendererVulkan(gfx::SDLContext& ctx, uint16_t w, uint16_t h) 
			: SDLRenderer(ctx, w, h, "vulkan")
		{

		}

	}
}
