#ifndef RGLZ_CAMERA_COMPONENT_H
#define RGLZ_CAMERA_COMPONENT_H

#include "base.h"

namespace rglz {
	struct Camera2DComponent {
		Camera2DComponent(const Vec3& pos, const Vec2& zoom)
			: pos(pos)
			, zoom(zoom)
		{
		}

		Mat4 compute_projection_mat(uint16_t width, uint16_t height) const {
			return Mat4::ortho(0, width, 0, height, -1.0f, 1.0f);
		}

		Mat4 compute_view_mat(uint16_t width, uint16_t height) const {
			Mat4 out_mat;

			float center_x = width / 2.0f;
			float center_y = height / 2.0f;

			Mat4 move_to_center = Mat4::translate({center_x, center_y, pos.z});
			Mat4 apply_zoom     = Mat4::scale({zoom.x, zoom.y, 1.0f});
			Mat4 move_to_camera = Mat4::translate({-pos.x, -pos.y, 0.0f});

			 out_mat = move_to_center * apply_zoom * move_to_camera;

			 return out_mat;
		}

		Mat4 compute_projection_view_mat(uint16_t width, uint16_t height) const {
			return compute_projection_mat(width, height) * compute_view_mat(width, height);
		}

		Vec3 pos;
		Vec2 zoom;
	};
}

#endif	// RGLZ_CAMERA_COMPONENT_H
