#ifndef RGLZ_MATH_MAT_H
#define RGLZ_MATH_MAT_H

#include "base.h"
#include "math/vec.h"
#include <array>

namespace rglz {
	struct Mat3 {
		inline Mat3 operator+ (const Mat3& mat) const {
			Mat3 out_mat;

			for (uint8_t i = 0; i < data.size(); i++) {
				out_mat.data[i] = data[i] + mat.data[i];
			}

			return out_mat;
		}

		inline Mat3& operator+= (const Mat3& mat) {
			for (uint8_t i = 0; i < data.size(); i++) {
				data[i] += mat.data[i];
			}

			return *this;
		}

		inline Mat3 operator- (const Mat3& mat) const {
			Mat3 out_mat;

			for (uint8_t i = 0; i < data.size(); i++) {
				out_mat.data[i] = data[i] - mat.data[i];
			}

			return out_mat;
		}

		inline Mat3& operator-= (const Mat3& mat) {
			for (uint8_t i = 0; i < data.size(); i++) {
				data[i] -= mat.data[i];
			}

			return *this;
		}

		inline Mat3 operator* (const Mat3& mat) const {
			Mat3 out_mat;

			const Vec3 r1 = row(0);
			const Vec3 r2 = row(1);
			const Vec3 r3 = row(2);

			const Vec3 c1 = mat.column(0);
			const Vec3 c2 = mat.column(1);
			const Vec3 c3 = mat.column(2);

			out_mat.data[0] = Vec3::dot(r1, c1);
			out_mat.data[1] = Vec3::dot(r2, c1);
			out_mat.data[2] = Vec3::dot(r3, c1);

			out_mat.data[3] = Vec3::dot(r1, c2);
			out_mat.data[4] = Vec3::dot(r2, c2);
			out_mat.data[5] = Vec3::dot(r3, c2);

			out_mat.data[6] = Vec3::dot(r1, c3);
			out_mat.data[7] = Vec3::dot(r2, c3);
			out_mat.data[8] = Vec3::dot(r3, c3);

			return out_mat;
		}

		inline Mat3& operator*= (const Mat3& mat) {
			const Vec3 r1 = row(0);
			const Vec3 r2 = row(1);
			const Vec3 r3 = row(2);

			const Vec3 c1 = mat.column(0);
			const Vec3 c2 = mat.column(1);
			const Vec3 c3 = mat.column(2);

			data[0] = Vec3::dot(r1, c1);
			data[1] = Vec3::dot(r2, c1);
			data[2] = Vec3::dot(r3, c1);

			data[3] = Vec3::dot(r1, c2);
			data[4] = Vec3::dot(r2, c2);
			data[5] = Vec3::dot(r3, c2);

			data[6] = Vec3::dot(r1, c3);
			data[7] = Vec3::dot(r2, c3);
			data[8] = Vec3::dot(r3, c3);

			return *this;
		}

		inline Vec3 row(const uint8_t i) const {
			RGLZ_ENGINE_ASSERT(i < 3);

			const uint8_t offset = i * 3;

			return Vec3{data[0 + offset], data[3 + offset], data[6 + offset]};
		}

		inline Vec3 column(const uint8_t i) const {
			RGLZ_ENGINE_ASSERT(i < 3);

			const uint8_t offset = i * 3;

			return Vec3{data[0 + offset], data[1 + offset], data[2 + offset]};
		}

		std::array<float, 9> data;
	};
}

#endif	// RGLZ_MATH_MAT_H
