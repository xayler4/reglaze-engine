#include "systems/camera_extractor_system.h"
#include "gfx/renderer.h"

namespace rglz {

	CameraExtractorSystem::CameraExtractorSystem(uint16_t width, uint16_t height) 
		: width(width)
		, height(height)
	{

	}

	void CameraExtractorSystem::on_process(Scene& scene) {
		std::span<const Camera2DComponent> camera_component_view = get_component_view<const Camera2DComponent>(scene);
		if (camera_component_view.size()) {
			Engine::app_renderer().set_frame_projection_view_mat((camera_component_view.end() - 1)->compute_projection_view_mat(width, height));
		}
	}
}
