#ifndef RGLZ_IMAGE_H
#define RGLZ_IMAGE_H

namespace rglz {

	struct Image {
		int n_channels;
		int width;
		int height;
		uint8_t* data;
	};

	struct GPUImage {
		int width;
		int height;
		void* handle;
	};
}

#endif	// RGLZ_IMAGE_H
