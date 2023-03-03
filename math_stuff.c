void multiplyMatrixVector(float *m, float *v, float *out) {
	out[0] = m[0]*v[0] + m[4]*v[1] + m[8]*v[2] + m[12]*v[3];
	out[1] = m[1]*v[0] + m[5]*v[1] + m[9]*v[2] + m[13]*v[3];
	out[2] = m[2]*v[0] + m[6]*v[1] + m[10]*v[2] + m[14]*v[3];
	out[3] = m[3]*v[0] + m[7]*v[1] + m[11]*v[2] + m[15]*v[3];
}