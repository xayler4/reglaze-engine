#include "pch.h"
#include "gfx/sdl_renderer.h"
#include "gfx/compiled_shaders/sprite.frag.spv.h"
#include "gfx/compiled_shaders/sprite.vert.spv.h"

namespace rglz {
	namespace gfx {
		SDLRenderer::Cache::Cache() {
			color_target_info = {};
			color_target_info.clear_color = {0.0f, 0.0f, 0.0f, 1.0f};
			color_target_info.load_op = SDL_GPU_LOADOP_CLEAR;
			color_target_info.store_op = SDL_GPU_STOREOP_STORE;
		}

		SDLRenderer::SDLRenderer(SDLContext& ctx, const char* sdl_backend_id) 
			: Renderer(ctx)
			, m_device(nullptr)
			, m_quad_buffer(nullptr)
			, m_sprite_pipeline(nullptr)
			, m_sprite_sampler(nullptr)
		{	
			SDLContext& sdl_context = static_cast<SDLContext&>(context());
			SDL_SetGPUAllowedFramesInFlight(m_device, RGLZ_RENDERER_MAX_FRAMES_IN_FLIGHT);

			m_device = SDL_CreateGPUDevice(SDL_GPU_SHADERFORMAT_SPIRV, true, sdl_backend_id);
			if (m_device == nullptr) {
				RGLZ_ENGINE_LOG_ERROR("Failed to create SDL GPUDevice: ", std::string_view(SDL_GetError()));
				throw BackendError{};
			}

			if (!SDL_ClaimWindowForGPUDevice(m_device, sdl_context.window)) {
				RGLZ_ENGINE_LOG_ERROR("Failed to claim SDL Window for SDL GPUDevice: ", std::string_view(SDL_GetError()));
				throw BackendError{};
			}
			{
				const float quad_vertices[] = {
					0.0f, 0.0f,  0.0f, 0.0f, // Top-Left
					1.0f, 0.0f,  1.0f, 0.0f, // Top-Right
					1.0f, 1.0f,  1.0f, 1.0f, // Bottom-Right
					
					0.0f, 0.0f,  0.0f, 0.0f, // Top-Left
					1.0f, 1.0f,  1.0f, 1.0f, // Bottom-Right
					0.0f, 1.0f,  0.0f, 1.0f  // Bottom-Left
				};

				SDL_GPUBufferCreateInfo buffer_info = {};
				buffer_info.usage = SDL_GPU_BUFFERUSAGE_VERTEX;
				buffer_info.size = sizeof(quad_vertices);
				m_quad_buffer = SDL_CreateGPUBuffer(m_device, &buffer_info);

				SDL_GPUTransferBufferCreateInfo transfer_info = {};
				transfer_info.usage = SDL_GPU_TRANSFERBUFFERUSAGE_UPLOAD;
				transfer_info.size = sizeof(quad_vertices);
				SDL_GPUTransferBuffer* transfer = SDL_CreateGPUTransferBuffer(m_device, &transfer_info);

				void* map = SDL_MapGPUTransferBuffer(m_device, transfer, false);
				std::memcpy(map, quad_vertices, sizeof(quad_vertices));
				SDL_UnmapGPUTransferBuffer(m_device, transfer);

				SDL_GPUCommandBuffer* cmd = SDL_AcquireGPUCommandBuffer(m_device);
				SDL_GPUCopyPass* copy = SDL_BeginGPUCopyPass(cmd);
				SDL_GPUTransferBufferLocation src = { transfer, 0 };
				SDL_GPUBufferRegion dst = { m_quad_buffer, 0, sizeof(quad_vertices) };
				SDL_UploadToGPUBuffer(copy, &src, &dst, false);
				SDL_EndGPUCopyPass(copy);

				SDL_GPUFence* fence = SDL_SubmitGPUCommandBufferAndAcquireFence(cmd);

				SDL_WaitForGPUFences(m_device, true, &fence, 1);
				SDL_ReleaseGPUFence(m_device, fence);

				SDL_ReleaseGPUTransferBuffer(m_device, transfer);
			}

			{
				SDL_GPUSamplerCreateInfo sampler_create_info = {};
				sampler_create_info.min_filter = SDL_GPU_FILTER_NEAREST;
				sampler_create_info.mag_filter = SDL_GPU_FILTER_NEAREST;
				sampler_create_info.mipmap_mode = SDL_GPU_SAMPLERMIPMAPMODE_NEAREST;

				sampler_create_info.address_mode_u = SDL_GPU_SAMPLERADDRESSMODE_CLAMP_TO_EDGE;
				sampler_create_info.address_mode_v = SDL_GPU_SAMPLERADDRESSMODE_CLAMP_TO_EDGE;
				sampler_create_info.address_mode_w = SDL_GPU_SAMPLERADDRESSMODE_CLAMP_TO_EDGE;

				m_sprite_sampler = SDL_CreateGPUSampler(m_device, &sampler_create_info);
				if (m_sprite_sampler == nullptr) {
					RGLZ_ENGINE_LOG_ERROR("Failed to create sprite GPU sampler: ", std::string_view(SDL_GetError()));
					throw BackendError{};
				}
			}

			{
				SDL_GPUShader* sprite_vert_shader = create_shader_from_memory(sprite_vert_spv, sprite_vert_spv_len, SDL_GPU_SHADERSTAGE_VERTEX, 0, 0, 0, 1); // Uniform buffer for the Camera
                SDL_GPUShader* sprite_frag_shader = create_shader_from_memory(sprite_frag_spv, sprite_frag_spv_len, SDL_GPU_SHADERSTAGE_FRAGMENT, 1);
                
                SDL_GPUGraphicsPipelineCreateInfo pipeline_create_info = {};
                pipeline_create_info.vertex_shader = sprite_vert_shader;
                pipeline_create_info.fragment_shader = sprite_frag_shader;
                pipeline_create_info.primitive_type = SDL_GPU_PRIMITIVETYPE_TRIANGLELIST;

                SDL_GPUVertexBufferDescription vertex_buffer_descs[2] = {};
                
                // Buffer 0: Quad (Per-Vertex)
                vertex_buffer_descs[0].slot = 0;
                vertex_buffer_descs[0].pitch = sizeof(float) * 4; // 2 floats Position + 2 floats UV
                vertex_buffer_descs[0].input_rate = SDL_GPU_VERTEXINPUTRATE_VERTEX;
                vertex_buffer_descs[0].instance_step_rate = 0;

                // Buffer 1: Instance Matrices (Per-Instance)
                vertex_buffer_descs[1].slot = 1;
				vertex_buffer_descs[1].pitch = sizeof(Mat4);
                vertex_buffer_descs[1].input_rate = SDL_GPU_VERTEXINPUTRATE_INSTANCE;
                vertex_buffer_descs[1].instance_step_rate = 0;


                SDL_GPUVertexAttribute attributes[6] = {};

                // Attribute 0: Quad Position (vec2)
                attributes[0].location = 0; 
                attributes[0].buffer_slot = 0; // Pulls from Buffer 0
                attributes[0].format = SDL_GPU_VERTEXELEMENTFORMAT_FLOAT2;
                attributes[0].offset = 0;

                // Attribute 1: Quad UV (vec2)
                attributes[1].location = 1; 
                attributes[1].buffer_slot = 0;
                attributes[1].format = SDL_GPU_VERTEXELEMENTFORMAT_FLOAT2;
                attributes[1].offset = sizeof(float) * 2;

                // Attributes 2, 3, 4, 5: Instance Mat4 (four vec4s)
                for (int i = 0; i < 4; ++i) {
                    attributes[2 + i].location = 2 + i;
                    attributes[2 + i].buffer_slot = 1; // Pulls from Buffer 1
                    attributes[2 + i].format = SDL_GPU_VERTEXELEMENTFORMAT_FLOAT4;
                    attributes[2 + i].offset = sizeof(float) * 4 * i; // Offset 16 bytes per column
                }

                pipeline_create_info.vertex_input_state.vertex_buffer_descriptions = vertex_buffer_descs;
                pipeline_create_info.vertex_input_state.num_vertex_buffers = 2;
                
                pipeline_create_info.vertex_input_state.vertex_attributes = attributes;
                pipeline_create_info.vertex_input_state.num_vertex_attributes = 6;


                pipeline_create_info.rasterizer_state.fill_mode = SDL_GPU_FILLMODE_FILL;
                pipeline_create_info.rasterizer_state.cull_mode = SDL_GPU_CULLMODE_NONE;
                pipeline_create_info.rasterizer_state.front_face = SDL_GPU_FRONTFACE_COUNTER_CLOCKWISE;

                pipeline_create_info.depth_stencil_state.enable_depth_test = false;
                pipeline_create_info.depth_stencil_state.enable_depth_write = false;
                pipeline_create_info.depth_stencil_state.compare_op = SDL_GPU_COMPAREOP_LESS_OR_EQUAL;

                SDL_GPUColorTargetDescription color_target = {};
                color_target.format = SDL_GetGPUSwapchainTextureFormat(m_device, sdl_context.window); 
                color_target.blend_state.enable_blend = true;
                color_target.blend_state.color_blend_op = SDL_GPU_BLENDOP_ADD;
                color_target.blend_state.src_color_blendfactor = SDL_GPU_BLENDFACTOR_SRC_ALPHA;
                color_target.blend_state.dst_color_blendfactor = SDL_GPU_BLENDFACTOR_ONE_MINUS_SRC_ALPHA;
                color_target.blend_state.alpha_blend_op = SDL_GPU_BLENDOP_ADD;
                color_target.blend_state.src_alpha_blendfactor = SDL_GPU_BLENDFACTOR_ONE;
                color_target.blend_state.dst_alpha_blendfactor = SDL_GPU_BLENDFACTOR_ONE_MINUS_SRC_ALPHA;

                pipeline_create_info.target_info.color_target_descriptions = &color_target;
                pipeline_create_info.target_info.num_color_targets = 1;

                pipeline_create_info.target_info.depth_stencil_format = SDL_GPU_TEXTUREFORMAT_D32_FLOAT;
                pipeline_create_info.target_info.has_depth_stencil_target = false;

                m_sprite_pipeline = SDL_CreateGPUGraphicsPipeline(m_device, &pipeline_create_info);

                SDL_ReleaseGPUShader(m_device, sprite_vert_shader);
                SDL_ReleaseGPUShader(m_device, sprite_frag_shader);
			}

			{
				Renderer::GPUUploadStream& upload_stream = sprite_upload_matrices_stream();

				SDL_GPUTransferBufferCreateInfo transfer_create_info = {};
				transfer_create_info.usage = SDL_GPU_TRANSFERBUFFERUSAGE_UPLOAD;
				transfer_create_info.size = sizeof(Mat4) * RGLZ_RENDERER_MAX_SPRITES;

				SDL_GPUBufferCreateInfo buffer_create_info{};
				buffer_create_info.usage = SDL_GPU_BUFFERUSAGE_VERTEX;
				buffer_create_info.size = sizeof(Mat4) * RGLZ_RENDERER_MAX_SPRITES;

				for (FrameData& frame_data : m_frame_data_list) {
					frame_data.sprite_instance_transfer_buffer = SDL_CreateGPUTransferBuffer(m_device, &transfer_create_info);
					frame_data.sprite_instance_buffer = SDL_CreateGPUBuffer(m_device, &buffer_create_info);

				}
			}
		}
		SDLRenderer::~SDLRenderer() {
            if (m_device != nullptr) {
                SDLContext& sdl_context = static_cast<SDLContext&>(context());
                
                SDL_ReleaseGPUGraphicsPipeline(m_device, m_sprite_pipeline);
                SDL_ReleaseGPUSampler(m_device, m_sprite_sampler);
                
                if (m_quad_buffer) {
                    SDL_ReleaseGPUBuffer(m_device, m_quad_buffer);
                }

                for (FrameData& frame_data : m_frame_data_list) {
                    if (frame_data.sprite_instance_transfer_buffer) {
                        SDL_ReleaseGPUTransferBuffer(m_device, frame_data.sprite_instance_transfer_buffer);
                    }
                    if (frame_data.sprite_instance_buffer) {
                        SDL_ReleaseGPUBuffer(m_device, frame_data.sprite_instance_buffer);
                    }
                }

                if (sdl_context.window != nullptr) {
                    SDL_ReleaseWindowFromGPUDevice(m_device, sdl_context.window);
                }
                SDL_DestroyGPUDevice(m_device);
            }
        }

		void SDLRenderer::on_begin() {
			SDLContext& sdl_context = static_cast<SDLContext&>(context());
			FrameData& frame = frame_data();

			SDL_WindowFlags flags = SDL_GetWindowFlags(sdl_context.window);
			if (flags & (SDL_WINDOW_MINIMIZED | SDL_WINDOW_HIDDEN | SDL_WINDOW_OCCLUDED)) {
				sdl_context.valid = false;
				return;
			}

			frame.primary_command_buffer = SDL_AcquireGPUCommandBuffer(m_device);
			if (!frame.primary_command_buffer) {
				sdl_context.valid = false;
				return;
			}

			uint32_t w, h;
			bool acquired = SDL_WaitAndAcquireGPUSwapchainTexture(
				frame.primary_command_buffer, sdl_context.window, &frame.swapchain_texture, &w, &h);

			if (acquired && frame.swapchain_texture != nullptr) {
				sdl_context.width = (uint16_t)w;    
				sdl_context.height = (uint16_t)h;    
				sdl_context.valid = true;

				m_cache.color_target_info.texture = frame.swapchain_texture;
			} 
			else {
				sdl_context.valid = false;
			}
		}

		void SDLRenderer::on_end() {
            SDLContext& sdl_context = static_cast<SDLContext&>(context());
            FrameData& frame = frame_data();

            if (sdl_context.valid) {
                void* mapped_memory = SDL_MapGPUTransferBuffer(m_device, frame.sprite_instance_transfer_buffer, false);
                
                sprite_upload_matrices_stream().set_data(static_cast<uint8_t*>(mapped_memory));

                create_sprite_batch_and_model_list();
                size_t sprite_model_mat_buffer_size = sprite_upload_matrices_write_stream().get_write_idx();
                
                SDL_UnmapGPUTransferBuffer(m_device, frame.sprite_instance_transfer_buffer);

                // Copy pass
                if (sprite_model_mat_buffer_size) {
                    SDL_GPUCopyPass* copy_pass = SDL_BeginGPUCopyPass(frame.primary_command_buffer);
                    SDL_GPUTransferBufferLocation src = { frame.sprite_instance_transfer_buffer, 0 };
                    SDL_GPUBufferRegion dst = { frame.sprite_instance_buffer, 0, (uint32_t)sprite_model_mat_buffer_size };
                    
                    SDL_UploadToGPUBuffer(copy_pass, &src, &dst, false);
                    SDL_EndGPUCopyPass(copy_pass);
                }

                // Draw pass
                {
                    SDL_GPURenderPass* render_pass = SDL_BeginGPURenderPass(frame.primary_command_buffer, &m_cache.color_target_info, 1, nullptr);

                    SDL_BindGPUGraphicsPipeline(render_pass, m_sprite_pipeline);
                    const Mat4& proj_view_mat = projection_view_mat();
                    SDL_PushGPUVertexUniformData(frame.primary_command_buffer, 0, &proj_view_mat, sizeof(Mat4));

                    SDL_GPUBufferBinding vertex_bindings[2];
                    vertex_bindings[0] = { m_quad_buffer, 0 };
                    vertex_bindings[1] = { frame.sprite_instance_buffer, 0 };
                    SDL_BindGPUVertexBuffers(render_pass, 0, vertex_bindings, 2);

                    for (const SpriteBatch& sprite_batch : frame_sprite_batch_list()) {
                        SDL_GPUTexture* texture = static_cast<SDL_GPUTexture*>(sprite_batch.image_handle);
                        SDL_GPUTextureSamplerBinding tex_bind = { texture, m_sprite_sampler };
                        SDL_BindGPUFragmentSamplers(render_pass, 0, &tex_bind, 1);

                        SDL_DrawGPUPrimitives(
                            render_pass, 
                            6, 
                            sprite_batch.instance_count, 
                            0, 
                            sprite_batch.start_instance
                        );
                    }

                    SDL_EndGPURenderPass(render_pass);
                }

                SDL_SubmitGPUCommandBuffer(frame.primary_command_buffer);
            }

            empty_command_lists();
            advance_current_frame_index();
        }

		GPUImage SDLRenderer::create_gpu_image(const Image& image) {
			GPUImage out_gpu_image{};
			out_gpu_image.width = image.width;
			out_gpu_image.height = image.height;

			SDL_GPUTextureCreateInfo texture_create_info{};
			texture_create_info.type = SDL_GPU_TEXTURETYPE_2D;
    		texture_create_info.format = SDL_GPU_TEXTUREFORMAT_R8G8B8A8_UNORM;
    		texture_create_info.width = image.width;
    		texture_create_info.height = image.height;
    		texture_create_info.layer_count_or_depth = 1;
    		texture_create_info.num_levels = 1;
    		texture_create_info.usage = SDL_GPU_TEXTUREUSAGE_SAMPLER;

			SDL_GPUTexture* texture = SDL_CreateGPUTexture(m_device, &texture_create_info);
			uint32_t buffer_size = image.width * image.height * image.n_channels;
			SDL_GPUTransferBufferCreateInfo transfer_create_info = {};
			transfer_create_info.usage = SDL_GPU_TRANSFERBUFFERUSAGE_UPLOAD;
			transfer_create_info.size = buffer_size;

			SDL_GPUTransferBuffer* transfer_buffer = SDL_CreateGPUTransferBuffer(m_device, &transfer_create_info);

			void* map = SDL_MapGPUTransferBuffer(m_device, transfer_buffer, false);
			std::memcpy(map, image.data, buffer_size);
			SDL_UnmapGPUTransferBuffer(m_device, transfer_buffer);

			SDL_GPUCommandBuffer* cmd_buffer = SDL_AcquireGPUCommandBuffer(m_device);
			SDL_GPUCopyPass* copy_pass = SDL_BeginGPUCopyPass(cmd_buffer);

			SDL_GPUTextureTransferInfo src = {};
			src.transfer_buffer = transfer_buffer;
			src.offset = 0;

			SDL_GPUTextureRegion dst = {};
			dst.texture = texture;
			dst.w = image.width;
			dst.h = image.height;
			dst.d = 1;

			SDL_UploadToGPUTexture(copy_pass, &src, &dst, false);
            SDL_EndGPUCopyPass(copy_pass);

            SDL_GPUFence* fence = SDL_SubmitGPUCommandBufferAndAcquireFence(cmd_buffer);

            SDL_WaitForGPUFences(m_device, true, &fence, 1);

            SDL_ReleaseGPUFence(m_device, fence);
            SDL_ReleaseGPUTransferBuffer(m_device, transfer_buffer);

            out_gpu_image.handle = texture;

            return out_gpu_image;
			
		}

		void SDLRenderer::destroy_gpu_image(const GPUImage& gpu_image) {
			SDL_GPUTexture* texture = static_cast<SDL_GPUTexture*>(gpu_image.handle);

			SDL_ReleaseGPUTexture(m_device, texture);
		}

		SDL_GPUShader* SDLRenderer::create_shader_from_memory(const uint8_t* code, std::size_t code_size, SDL_GPUShaderStage stage, uint8_t num_samplers, uint8_t num_storage_textures, uint8_t num_storage_buffers, uint8_t num_uniform_buffers) {
			SDL_GPUShaderCreateInfo shader_create_info{};
			shader_create_info.code = code;
			shader_create_info.code_size = code_size;
			shader_create_info.entrypoint = "main";

			shader_create_info.format = SDL_GPU_SHADERFORMAT_SPIRV;
			shader_create_info.stage = stage;

			shader_create_info.num_samplers = num_samplers; 
			shader_create_info.num_storage_textures = num_storage_textures;
			shader_create_info.num_storage_buffers = num_storage_buffers;
			shader_create_info.num_uniform_buffers = num_uniform_buffers;

    		return SDL_CreateGPUShader(m_device, &shader_create_info);
		}

		SDLRendererVulkan::SDLRendererVulkan(gfx::SDLContext& ctx)
			: SDLRenderer(ctx, "vulkan")
		{

		}

	}
}
