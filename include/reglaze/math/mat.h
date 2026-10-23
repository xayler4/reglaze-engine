#ifndef RGLZ_MATH_MAT_H
#define RGLZ_MATH_MAT_H

#include "base.h"
#include "math/vec.h"
#include "easer/easer.h"
#include <array>

// Column-Major
namespace rglz {
	struct Mat3 {
		static inline Mat3 identity() {
			Mat3 out_mat{};

			out_mat(0,0) = 1.0f;
			out_mat(1,1) = 1.0f;
			out_mat(2,2) = 1.0f;

			return out_mat;
		}

		static inline Mat3 translate(const Vec2& pos) {
			Mat3 out_mat = Mat3::identity();

			out_mat(0, 2) = pos.x;
			out_mat(1, 2) = pos.y;

			return out_mat;
		}

		static inline Mat3 scale(const Vec2& s) {
			Mat3 out_mat = Mat3::identity();

			out_mat(0, 0) = s.x;
			out_mat(1, 1) = s.y;

			return out_mat;
		}

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

			const uint8_t offset = i;

			return Vec3{data[0 + offset], data[3 + offset], data[6 + offset]};
		}

		inline Vec3 column(const uint8_t i) const {
			RGLZ_ENGINE_ASSERT(i < 3);

			const uint8_t offset = i * 3;

			return Vec3{data[0 + offset], data[1 + offset], data[2 + offset]};
		}

		inline float& operator() (uint8_t r, uint8_t c) {
			RGLZ_ENGINE_ASSERT(r < 3 && c < 3);

			return data[c * 3 + r];
		}

		inline const float& operator() (uint8_t r, uint8_t c) const {
			RGLZ_ENGINE_ASSERT(r < 3 && c < 3);

			return data[c * 3 + r];
		}

		ESR_BEGIN();
		ESR_FIELD(ESR_PACK(std::array<float, 9>), data, "GPU");
		ESR_END();
	};

	struct Mat4 {
		static inline Mat4 ortho(uint16_t l, uint16_t r, uint16_t b, uint16_t t, float n, float f) {
			Mat4 out_mat{};

			out_mat(0,0) = 2.0f / (r - l);
			out_mat(1,1) = 2.0f / (b - t);
			out_mat(2,2) = -1.0f / (f - n);

			out_mat(0,3) = -(r + l) / (r - l);
			out_mat(1,3) = -(b + t) / (b - t);
			out_mat(2,3) = -n / (f - n);

			out_mat(3,3) = 1.0f;

			return out_mat;
		}

		static inline Mat4 identity() {
			Mat4 out_mat{};

			out_mat(0,0) = 1.0f;
			out_mat(1,1) = 1.0f;
			out_mat(2,2) = 1.0f;
			out_mat(3,3) = 1.0f;

			return out_mat;
		}

		static inline Mat4 translate(const Vec3& pos) {
			Mat4 out_mat = Mat4::identity();

			out_mat(0, 3) = pos.x;
			out_mat(1, 3) = pos.y;
			out_mat(2, 3) = pos.z;

			return out_mat;
		}

		static inline Mat4 scale(const Vec3& s) {
			Mat4 out_mat = Mat4::identity();

			out_mat(0, 0) = s.x;
			out_mat(1, 1) = s.y;
			out_mat(2, 2) = s.z;

			return out_mat;
		}

		inline Mat4 operator+ (const Mat4& mat) const {
			Mat4 out_mat;

			for (uint8_t i = 0; i < data.size(); i++) {
				out_mat.data[i] = data[i] + mat.data[i];
			}

			return out_mat;
		}

		inline Mat4& operator+= (const Mat4& mat) {
			for (uint8_t i = 0; i < data.size(); i++) {
				data[i] += mat.data[i];
			}

			return *this;
		}

		inline Mat4 operator- (const Mat4& mat) const {
			Mat4 out_mat;

			for (uint8_t i = 0; i < data.size(); i++) {
				out_mat.data[i] = data[i] - mat.data[i];
			}

			return out_mat;
		}

		inline Mat4& operator-= (const Mat4& mat) {
			for (uint8_t i = 0; i < data.size(); i++) {
				data[i] -= mat.data[i];
			}

			return *this;
		}

		inline Mat4 operator* (const Mat4& mat) const {
			Mat4 out_mat;

			const Vec4 r1 = row(0);
			const Vec4 r2 = row(1);
			const Vec4 r3 = row(2);
			const Vec4 r4 = row(3);

			const Vec4 c1 = mat.column(0);
			const Vec4 c2 = mat.column(1);
			const Vec4 c3 = mat.column(2);
			const Vec4 c4 = mat.column(3);

			out_mat.data[0] = Vec4::dot(r1, c1);
			out_mat.data[1] = Vec4::dot(r2, c1);
			out_mat.data[2] = Vec4::dot(r3, c1);
			out_mat.data[3] = Vec4::dot(r4, c1);

			out_mat.data[4] = Vec4::dot(r1, c2);
			out_mat.data[5] = Vec4::dot(r2, c2);
			out_mat.data[6] = Vec4::dot(r3, c2);
			out_mat.data[7] = Vec4::dot(r4, c2);

			out_mat.data[8] = Vec4::dot(r1, c3);
			out_mat.data[9] = Vec4::dot(r2, c3);
			out_mat.data[10] = Vec4::dot(r3, c3);
			out_mat.data[11] = Vec4::dot(r4, c3);

			out_mat.data[12] = Vec4::dot(r1, c4);
			out_mat.data[13] = Vec4::dot(r2, c4);
			out_mat.data[14] = Vec4::dot(r3, c4);
			out_mat.data[15] = Vec4::dot(r4, c4);

			return out_mat;
		}

		inline Mat4& operator*= (const Mat4& mat) {
			const Vec4 r1 = row(0);
			const Vec4 r2 = row(1);
			const Vec4 r3 = row(2);
			const Vec4 r4 = row(3);

			const Vec4 c1 = mat.column(0);
			const Vec4 c2 = mat.column(1);
			const Vec4 c3 = mat.column(2);
			const Vec4 c4 = mat.column(3);

			data[0] = Vec4::dot(r1, c1);
			data[1] = Vec4::dot(r2, c1);
			data[2] = Vec4::dot(r3, c1);
			data[3] = Vec4::dot(r4, c1);

			data[4] = Vec4::dot(r1, c2);
			data[5] = Vec4::dot(r2, c2);
			data[6] = Vec4::dot(r3, c2);
			data[7] = Vec4::dot(r4, c2);

			data[8] = Vec4::dot(r1, c3);
			data[9] = Vec4::dot(r2, c3);
			data[10] = Vec4::dot(r3, c3);
			data[11] = Vec4::dot(r4, c3);

			data[12] = Vec4::dot(r1, c4);
			data[13] = Vec4::dot(r2, c4);
			data[14] = Vec4::dot(r3, c4);
			data[15] = Vec4::dot(r4, c4);

			return *this;
		}

		inline Vec4 row(const uint8_t i) const {
			RGLZ_ENGINE_ASSERT(i < 4);

			const uint8_t offset = i;

			return Vec4{data[0 + offset], data[4 + offset], data[8 + offset], data[12 + offset]};
		}

		inline Vec4 column(const uint8_t i) const {
			RGLZ_ENGINE_ASSERT(i < 4);

			const uint8_t offset = i * 4;

			return Vec4{data[0 + offset], data[1 + offset], data[2 + offset], data[3 + offset]};
		}

		inline float& operator() (uint8_t r, uint8_t c) {
			RGLZ_ENGINE_ASSERT(r < 4 && c < 4);

			return data[c * 4 + r];
		}

		inline const float& operator() (uint8_t r, uint8_t c) const {
			RGLZ_ENGINE_ASSERT(r < 4 && c < 4);

			return data[c * 4 + r];
		}

		ESR_BEGIN();
		ESR_FIELD(ESR_PACK(std::array<float, 16>), data, "GPU");
		ESR_END();
	};
}

#endif	// RGLZ_MATH_MAT_H
