struct Mat4 {
	float m[16];
};

struct Mat4 Mat4_Identity() {
	return (struct Mat4){.m = {1, 0, 0, 0,
	0, 1, 0, 0,
	0, 0, 1, 0,
	0, 0, 0, 1}};
}

struct Mat4 Mat4_Translation(struct Vec3f translation) {
	return (struct Mat4){.m = {1, 0, 0, translation.x,
	0, 1, 0, translation.y,
	0, 0, 1, translation.z,
	0, 0, 0, 1}};
}

struct Mat4 Mat4_RotationX(float angle) {
	float sine = sinf(angle);
	float cosine = cosf(angle);

	return (struct Mat4){.m = {1, 0, 0, 0,
	0, cosine, -sine, 0,
	0, sine, cosine, 0,
	0, 0, 0, 1}};
}

struct Mat4 Mat4_RotationY(float angle) {
	float sine = sinf(angle);
	float cosine = cosf(angle);

	return (struct Mat4){.m = {cosine, 0, sine, 0,
	0, 1, 0, 0,
	-sine, 0, cosine, 0,
	0, 0, 0, 1}};
}

struct Mat4 Mat4_RotationZ(float angle) {
	float sine = sinf(angle);
	float cosine = cosf(angle);

	return (struct Mat4){.m = {cosine, -sine, 0, 0,
	sine, cosine, 0, 0,
	0, 0, 1, 0,
	0, 0, 0, 1}};
}

struct Mat4 Mat4_Orthographic(float left, float right, float top, float bottom, float near, float far) {
	float tx = -((right + left) / (right - left));
	float ty = -((top + bottom) / (top - bottom));
	float tz = -((far + near) / (far - near));

	return (struct Mat4){.m = {2 / (right - left), 0, 0, tx,
	0, 2 / (top - bottom), 0, ty,
	0, 0, -2 / (far - near), tz,
	0, 0, 0, 1}};
}

struct Mat4 Mat4_Multiply(struct Mat4 a, struct Mat4 b) {
	struct Mat4 result;
	for (int y = 0; y < 4; ++y) {
		for (int x = 0; x < 4; ++x) {
			result.m[y * 4 + x] = 0;
			for (int k = 0; k < 4; ++k) {
				result.m[y * 4 + x] += a.m[y * 4 + k] * b.m[k * 4 + x];
			}
		}
	}
	return result;
}

struct Vec3f Mat4_MultiplyVec3(struct Mat4 m, struct Vec3f v) {
	float x = m.m[0] * v.x + m.m[4] * v.y + m.m[8] * v.z + m.m[12];
	float y = m.m[1] * v.x + m.m[5] * v.y + m.m[9] * v.z + m.m[13];
	float z = m.m[2] * v.x + m.m[6] * v.y + m.m[10] * v.z + m.m[14];
	return (struct Vec3f){.x = x, .y = y, .z = z};
}
