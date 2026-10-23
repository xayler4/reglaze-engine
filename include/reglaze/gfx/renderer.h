#ifndef RGLZ_GFX_RENDERER_H
#define RGLZ_GFX_RENDERER_H

#include "base.h"
#include "gfx/context.h"
#include "gfx/renderer_commands.h"
#include "containers/fixed_vector.h"
#include "math/mat.h"
#include <cstdint>
#include <span>

namespace rglz {
	class Engine;

	namespace gfx {

		class Renderer {
		public:
			struct GPUUploadStream : public ::esr::Stream<GPUUploadStream> {
				GPUUploadStream(void* buffer, std::size_t size) 
					: ::esr::Stream<GPUUploadStream>((uint8_t*)buffer, size) {

				}

				static consteval std::string_view get_channel() {
					return "GPU";
				}

				template<typename T>
				static consteval std::size_t get_alignof() {
					return 1;
				}

			};

			struct FrameData {
				FrameData() 
					: FrameData(nullptr, 0) {
				}

				FrameData(void* buffer, std::size_t size) 
					: sprite_upload_matrices_stream(buffer, size)
					, sprite_upload_matrices_write_stream(sprite_upload_matrices_stream)
				{}

				FrameData operator=(const FrameData&) = delete;

				GPUUploadStream sprite_upload_matrices_stream;
				esr::WriteStream<GPUUploadStream>  sprite_upload_matrices_write_stream;
			};

			struct SpriteBatch {
				void* image_handle;
				uint32_t start_instance;
				uint32_t instance_count;
			};

		public:
			virtual GPUImage create_gpu_image(const Image& image) = 0;
			virtual void destroy_gpu_image(const GPUImage& gpu_image) = 0;

			void push_draw_sprite_command(const Mat4& transform, const GPUImage& gpu_image);

			void set_frame_projection_view_mat(const Mat4& mat) {
				m_projection_view_mat = mat;
			}

			virtual ~Renderer() = 0;

		protected:
			Renderer(Context& ctx);
			Renderer(const Renderer&) = delete;
			Renderer& operator= (const Renderer&) = delete;

			void advance_current_frame_index();

			void create_sprite_batch_and_model_list();

			void empty_command_lists();

			inline Context& context() {
				RGLZ_ENGINE_ASSERT(m_context != nullptr);
				return *m_context;
			}

			inline uint8_t current_frame_index() const {
				return m_current_frame_index;
			}


			inline std::span<const DrawSpriteCommand> frame_draw_sprite_command_list() const {
				return std::span<const DrawSpriteCommand>(m_draw_sprite_command_list.data(), m_draw_sprite_command_list.size());
			}

			inline std::span<const SpriteBatch> frame_sprite_batch_list() const {
				return std::span<const SpriteBatch>(m_sprite_batch_list.data(), m_sprite_batch_list.size());
			}

			inline GPUUploadStream& sprite_upload_matrices_stream() {
				return m_frame_data_list[m_current_frame_index].sprite_upload_matrices_stream;
			}

			inline esr::WriteStream<GPUUploadStream>& sprite_upload_matrices_write_stream() {
				return m_frame_data_list[m_current_frame_index].sprite_upload_matrices_write_stream;
			}

			inline std::span<FrameData> frame_data_list() {
				return m_frame_data_list;
			}

			inline const Mat4& projection_view_mat() const {
				return m_projection_view_mat;

			}

		private:
			virtual void on_begin() = 0;
			virtual void on_end() = 0;

		private:
			FixedVector<DrawSpriteCommand, RGLZ_RENDERER_MAX_SPRITES> m_draw_sprite_command_list;
			FixedVector<SpriteBatch, RGLZ_RENDERER_MAX_SPRITES> m_sprite_batch_list;
			FixedVector<FrameData, RGLZ_RENDERER_MAX_FRAMES_IN_FLIGHT> m_frame_data_list;
			Mat4 m_projection_view_mat;
			Context* m_context;
			uint8_t m_current_frame_index;

			friend class rglz::Engine;
		};
	}	
}

#endif	// RGLZ_GFX_RENDERER_H
