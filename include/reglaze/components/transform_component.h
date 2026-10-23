#ifndef RGLZ_COMPONENTS_TRANSFORM_COMPONENTS_H
#define RGLZ_COMPONENTS_TRANSFORM_COMPONENTS_H

#include "base.h"
#include "math/mat.h"
#include "math/vec.h"

namespace rglz {
	struct TransformComponent {
		TransformComponent(const Vec3& pos, const Vec3& scale) 
			: transform_mat()
		{
			set_pos(pos);
			set_scale(scale);

			transform_mat(3, 3) = 1.0f;
		}

		inline void set_pos(const Vec3& pos) {
			transform_mat(0, 3) = pos.x;
			transform_mat(1, 3) = pos.y;
			transform_mat(2, 3) = pos.z;
		}

		inline void set_scale(const Vec3& scale) {
			transform_mat(0, 0) = scale.x;
			transform_mat(1, 1) = scale.y;
			transform_mat(2, 2) = scale.z;
		}

		inline Vec3 pos() const {
			return Vec3{
				transform_mat(0, 3),
				transform_mat(1, 3),
				transform_mat(2, 3),
			};
		}

		inline Vec3 scale() const {
			return Vec3{
				transform_mat(0, 0),
				transform_mat(1, 1),
				transform_mat(2, 2),
			};
		}

		Mat4 transform_mat;
	};

}

template<ESR_REGISTRY_PARAMS>
ESR_REGISTER_PROC_W("Logger", rglz::TransformComponent, t, stream,
	ESR_PACK({
		rglz::Vec3 pos = t.pos();
		stream << "[ x: " << pos.x << ", y: " << pos.y <<  ", z: " << pos.z << " ]";	
	})
);


#endif	// RGLZ_COMPONENTS_TRANSFORM_COMPONENTS_H
