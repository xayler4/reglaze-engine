#include "pch.h"
#include "gfx/renderer.h"

namespace rglz {
	namespace gfx {
		Renderer::Renderer(Context& ctx) 
			: m_context(&ctx)
			, m_projection_view_mat(Mat4::identity())
			, m_current_frame_index(0)
		{
			m_frame_data_list.resize(RGLZ_RENDERER_MAX_FRAMES_IN_FLIGHT);
			for (FrameData& frame_data : m_frame_data_list) {
				frame_data.sprite_upload_matrices_stream.set_size(RGLZ_RENDERER_MAX_SPRITES*sizeof(Mat4));
			}
		}
		Renderer::~Renderer() = default;


		void Renderer::advance_current_frame_index() {
			if (++m_current_frame_index % RGLZ_RENDERER_MAX_FRAMES_IN_FLIGHT == 0) {
				m_current_frame_index = 0;
			}
		}

		void Renderer::empty_command_lists() {
			m_draw_sprite_command_list.resize(0);
			m_sprite_batch_list.resize(0);
			sprite_upload_matrices_write_stream().set_write_idx(0);
			m_projection_view_mat = Mat4::identity();
		}

		void Renderer::push_draw_sprite_command(const Mat4& transform, const GPUImage& gpu_image) {
			m_draw_sprite_command_list.emplace_back(transform, gpu_image);
		}

		void Renderer::create_sprite_batch_and_model_list() {
			std::sort(m_draw_sprite_command_list.begin(), m_draw_sprite_command_list.end(), 
				[](const DrawSpriteCommand& a, const DrawSpriteCommand& b) {
					return a.gpu_image.handle < b.gpu_image.handle;
				}
			);
			
			SpriteBatch current_sprite_batch = {nullptr, 0, 0};
			for (const DrawSpriteCommand& command : m_draw_sprite_command_list) {
				if (current_sprite_batch.image_handle != command.gpu_image.handle)	{
					if (current_sprite_batch.instance_count != 0) {
						m_sprite_batch_list.push_back(current_sprite_batch);

						current_sprite_batch.start_instance += current_sprite_batch.instance_count;
					}
					current_sprite_batch.image_handle = command.gpu_image.handle;
					current_sprite_batch.instance_count = 0;
				}

				sprite_upload_matrices_write_stream() << command.transform;
				current_sprite_batch.instance_count++;
			}
			if (current_sprite_batch.instance_count != 0) {
				m_sprite_batch_list.push_back(current_sprite_batch);
			}
		}
	}
}
