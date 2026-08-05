#ifndef RGLZ_GFX_RENDERER_H
#define RGLZ_GFX_RENDERER_H

#include "base.h"
#include "gfx/context.h"
#include "gfx/renderer_commands.h"
#include <cstdint>
#include <span>

namespace rglz {
	class Engine;

	namespace gfx {
		class Renderer {
		public:
			struct FrameData {
				std::span<DrawSpriteCommand> draw_sprite_command_list;
			};

		protected:
			Renderer(Context& ctx, uint16_t w, uint16_t h);
			Renderer(Renderer&& renderer);

			Renderer& operator= (Renderer&& renderer);

			Renderer(const Renderer&) = delete;
			Renderer& operator= (const Renderer&) = delete;

			inline Context& context() {
				RGLZ_ENGINE_ASSERT(m_context != nullptr);
				return *m_context;
			}

			inline uint16_t width() const {
				return m_width;
			}

			inline uint16_t height() const {
				return m_height;
			}

			inline void set_width_and_height(uint16_t w, uint16_t h) {
				m_width = w;
				m_height = h;
			}

		private:
			virtual void on_begin() = 0;
			virtual void on_end() = 0;

		private:
			Context* m_context;
			uint16_t m_width;
			uint16_t m_height;

			friend class rglz::Engine;
		};
	}	
}

#endif	// RGLZ_GFX_RENDERER_H
