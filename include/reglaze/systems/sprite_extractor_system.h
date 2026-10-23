#ifndef RGLZ_SPRITE_EXTRACTOR_SYSTEM_H
#define RGLZ_SPRITE_EXTRACTOR_SYSTEM_H

#include "base.h"
#include "components/sprite_component.h"
#include "components/transform_component.h"
#include "core/scene.h"

namespace rglz {
	class SpriteExtractorSystem : public System<const TransformComponent, const SpriteComponent>{
	public:
		void on_process(Scene& scene);
	};
}

#endif	// RGLZ_SPRITE_EXTRACTOR_SYSTEM_H
