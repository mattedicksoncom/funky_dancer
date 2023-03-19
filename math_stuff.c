void multiplyMatrixVector(float *m, float *v, float *out) {
    out[0] = m[0] * v[0] + m[4] * v[1] + m[8] * v[2] + m[12] * v[3];
    out[1] = m[1] * v[0] + m[5] * v[1] + m[9] * v[2] + m[13] * v[3];
    out[2] = m[2] * v[0] + m[6] * v[1] + m[10] * v[2] + m[14] * v[3];
    out[3] = m[3] * v[0] + m[7] * v[1] + m[11] * v[2] + m[15] * v[3];
}

struct Vec3f vec3f_CrossProduct(struct Vec3f a, struct Vec3f b) {
	struct Vec3f result;
	result.x = a.y * b.z - a.z * b.y;
	result.y = a.z * b.x - a.x * b.z;
	result.z = a.x * b.y - a.y * b.x;
	return result;
}

float vec3f_dotProduct(struct Vec3f a, struct Vec3f b) {
	return a.x * b.x + a.y * b.y + a.z * b.z;
}

struct Vec3f vec3f_subtract(struct Vec3f a, struct Vec3f b) {
	struct Vec3f result = {a.x - b.x, a.y - b.y, a.z - b.z};
	return result;
}

void vec3f_normalize(struct Vec3f *v) {
	float length = sqrtf(v->x * v->x + v->y * v->y + v->z * v->z);
	v->x /= length;
	v->y /= length;
	v->z /= length;
}

struct Vec3f barycentric(struct Vec3f vert1, struct Vec3f vert2, struct Vec3f vert3, struct Vec3f pixelCoord) {
	struct Vec3f s[2];

	s[0].x = vert3.x - vert1.x;
	s[0].y = vert2.x - vert1.x;
	s[0].z = vert1.x - pixelCoord.x;

	s[1].x = vert3.y - vert1.y;
	s[1].y = vert2.y - vert1.y;
	s[1].z = vert1.y - pixelCoord.y;

	struct Vec3f barycentricCoords = vec3f_CrossProduct(s[0], s[1]);

	// check if outside the triangle
	if (fabs(barycentricCoords.z) > 1e-2) {
		return (struct Vec3f) {
			1.0f - (barycentricCoords.x + barycentricCoords.y) / barycentricCoords.z,
			barycentricCoords.y / barycentricCoords.z,
			barycentricCoords.x / barycentricCoords.z
		};
	}
	return (struct Vec3f) { -1, 1, 1 };
}

struct Quaternion EulerToQuaternion(struct Vec3f euler) {
    // to radians (other stuff is using radians though so need to convert)
    float pitch = euler.x * M_PI / 180.0f;
    float yaw = euler.y * M_PI / 180.0f;
    float roll = euler.z * M_PI / 180.0f;

    // half angles
    float hp = pitch / 2;
    float hy = yaw / 2;
    float hr = roll / 2;

    // sin cos
    float sp = sinf(hp);
    float sy = sinf(hy);
    float sr = sinf(hr);
    float cp = cosf(hp);
    float cy = cosf(hy);
    float cr = cosf(hr);

    struct Quaternion q;
    q.w = cr * cp * cy + sr * sp * sy;
    q.x = sr * cp * cy - cr * sp * sy;
    q.y = cr * sp * cy + sr * cp * sy;
    q.z = cr * cp * sy - sr * sp * cy;

    return q;
}

struct Mat4 QuaternionToMatrix(struct Quaternion q) {
    struct Mat4 result;
    float xx = q.x * q.x;
    float xy = q.x * q.y;
    float xz = q.x * q.z;
    float xw = q.x * q.w;
    float yy = q.y * q.y;
    float yz = q.y * q.z;
    float yw = q.y * q.w;
    float zz = q.z * q.z;
    float zw = q.z * q.w;

    result.m[0] = 1 - 2 * (yy + zz);
    result.m[1] = 2 * (xy - zw);
    result.m[2] = 2 * (xz + yw);
    result.m[3] = 0;

    result.m[4] = 2 * (xy + zw);
    result.m[5] = 1 - 2 * (xx + zz);
    result.m[6] = 2 * (yz - xw);
    result.m[7] = 0;

    result.m[8] = 2 * (xz - yw);
    result.m[9] = 2 * (yz + xw);
    result.m[10] = 1 - 2 * (xx + yy);
    result.m[11] = 0;

    result.m[12] = 0;
    result.m[13] = 0;
    result.m[14] = 0;
    result.m[15] = 1;

    return result;
}

void trs(struct mesh *outMesh,
         float tx, float ty, float tz,
         float rx, float ry, float rz,
         float sx, float sy, float sz) {
	// Translation matrix
	float t[16] = {
		1.0f, 0.0f, 0.0f, 0.0f,
		0.0f, 1.0f, 0.0f, 0.0f,
		0.0f, 0.0f, 1.0f, 0.0f,
		tx, ty, tz, 1.0f
	};

	// Rotation matrix
	float c, s, len;
	float axis[3] = {rx, ry, rz};
	len = sqrtf(axis[0] * axis[0] + axis[1] * axis[1] + axis[2] * axis[2]);
	if (len != 0.0f) {
		c = cosf(M_PI / 180.0f * len);
		s = sinf(M_PI / 180.0f * len);
		axis[0] /= len;
		axis[1] /= len;
		axis[2] /= len;
	} else {
		c = 1.0f;
		s = 0.0f;
	}
	float t2[16] = {
		axis[0]*axis[0]*(1.0f-c) + c, axis[0]*axis[1]*(1.0f-c)-axis[2]*s, axis[0]*axis[2]*(1.0f-c) + axis[1]*s, 0.0f,
		axis[0]*axis[1]*(1.0f-c) + axis[2]*s, axis[1]*axis[1]*(1.0f-c) + c, axis[1]*axis[2]*(1.0f-c)-axis[0]*s, 0.0f,
		axis[0]*axis[2]*(1.0f-c)-axis[1]*s, axis[1]*axis[2]*(1.0f-c) + axis[0]*s, axis[2]*axis[2]*(1.0f-c) + c, 0.0f,
		0.0f, 0.0f, 0.0f, 1.0f
	};

	// Scale matrix
	float s3[16] = {
		sx, 0.0f, 0.0f, 0.0f,
		0.0f, sy, 0.0f, 0.0f,
		0.0f, 0.0f, sz, 0.0f,
		0.0f, 0.0f, 0.0f, 1.0f
	};

	// Transform each vertex
	for (int i = 0; i < outMesh->vertCount; i++) {
		// Apply translation, rotation, and scaling to vertex
		float v[4] = {outMesh->vert[i * 3], outMesh->vert[i * 3 + 1], outMesh->vert[i * 3 + 2], 1.0f};
		float v2[4];
		multiplyMatrixVector(t, v, v2);
		multiplyMatrixVector(t2, v2, v);
		multiplyMatrixVector(s3, v, v2);
		// Store transformed vertex back into mesh
		outMesh->vert[i * 3] = v2[0];
		outMesh->vert[i * 3 + 1] = v2[1];
		outMesh->vert[i * 3 + 2] = v2[2];
	}
}

void eularRotate(
	struct mesh *outMesh,
    float rx,
	float ry,
	float rz
) {
	float c, s, len;
	float axis[3] = {rx, ry, rz};
	len = sqrtf(axis[0] * axis[0] + axis[1] * axis[1] + axis[2] * axis[2]);
	if (len != 0.0f) {
		c = cosf(M_PI / 180.0f * len);
		s = sinf(M_PI / 180.0f * len);
		axis[0] /= len;
		axis[1] /= len;
		axis[2] /= len;
	} else {
		c = 1.0f;
		s = 0.0f;
	}
	float rotationMatrix[16] = {
		axis[0]*axis[0]*(1.0f-c) + c, axis[0]*axis[1]*(1.0f-c)-axis[2]*s, axis[0]*axis[2]*(1.0f-c) + axis[1]*s, 0.0f,
		axis[0]*axis[1]*(1.0f-c) + axis[2]*s, axis[1]*axis[1]*(1.0f-c) + c, axis[1]*axis[2]*(1.0f-c)-axis[0]*s, 0.0f,
		axis[0]*axis[2]*(1.0f-c)-axis[1]*s, axis[1]*axis[2]*(1.0f-c) + axis[0]*s, axis[2]*axis[2]*(1.0f-c) + c, 0.0f,
		0.0f, 0.0f, 0.0f, 1.0f
	};

	for (int i = 0; i < outMesh->vertCount; i++) {
		float v[4] = {outMesh->vert[i * 3], outMesh->vert[i * 3 + 1], outMesh->vert[i * 3 + 2], 1.0f};
		float v2[4];
		multiplyMatrixVector(rotationMatrix, v, v2);

		outMesh->vert[i * 3] = v2[0];
		outMesh->vert[i * 3 + 1] = v2[1];
		outMesh->vert[i * 3 + 2] = v2[2];
	}
}