#include "pch.h"
#include "gfx/renderer.h"

namespace rglz {
	namespace gfx {
		Renderer::Renderer(Context& ctx, uint16_t w, uint16_t h) 
			: m_context(&ctx)
			, m_width(w)
			, m_height(h)
		{}

		Renderer::Renderer(Renderer&& renderer) 
			: m_context(renderer.m_context)
			, m_width(renderer.m_width)
			, m_height(renderer.m_height)
		{
			renderer.m_context = nullptr;
		}

		Renderer& Renderer::operator= (Renderer&& renderer) {
			m_context = renderer.m_context;
			m_width = renderer.m_width;
			m_height = renderer.m_height;

			renderer.m_context = nullptr;

			return *this;
		}
	}
}
