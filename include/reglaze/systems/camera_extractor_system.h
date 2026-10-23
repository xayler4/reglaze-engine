#ifndef RGLZ_CAMERA_EXTRACTOR_SYSTEM_H
#define RGLZ_CAMERA_EXTRACTOR_SYSTEM_H

#include "base.h"
#include "components/camera_component.h"
#include "components/transform_component.h"
#include "core/scene.h"

namespace rglz {
	class CameraExtractorSystem : public System<const Camera2DComponent>{
	public:
		CameraExtractorSystem(uint16_t width, uint16_t height);
	public:
		void on_process(Scene& scene);

		uint16_t width;
		uint16_t height;
	};
}

#endif	// RGLZ_CAMERA_EXTRACTOR_SYSTEM_H
