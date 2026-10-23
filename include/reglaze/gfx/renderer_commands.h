#ifndef RGLZ_GFX_RENDERER_COMMANDS_H
#define RGLZ_GFX_RENDERER_COMMANDS_H

#include "base.h"
#include "core/image.h"
#include <math/mat.h>

namespace rglz {
	namespace gfx {
		struct DrawSpriteCommand {
			Mat4 transform;	
			GPUImage gpu_image;
		};
	}
}

#endif	// RGLZ_GFX_RENDERER_COMMANDS_H
