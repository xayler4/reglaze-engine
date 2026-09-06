#ifndef RGLZ_GFX_RENDERER_COMMANDS_H
#define RGLZ_GFX_RENDERER_COMMANDS_H

#include "base.h"
#include <math/mat.h>

namespace rglz {
	namespace gfx {
		struct DrawSpriteCommand {
			Mat3 transform;	
			 // sprite;
		};
	}
}

#endif	// RGLZ_GFX_RENDERER_COMMANDS_H
