void multiplyMatrixVector(float *m, float *v, float *out) {
    out[0] = m[0] * v[0] + m[4] * v[1] + m[8] * v[2] + m[12] * v[3];
    out[1] = m[1] * v[0] + m[5] * v[1] + m[9] * v[2] + m[13] * v[3];
    out[2] = m[2] * v[0] + m[6] * v[1] + m[10] * v[2] + m[14] * v[3];
    out[3] = m[3] * v[0] + m[7] * v[1] + m[11] * v[2] + m[15] * v[3];
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
