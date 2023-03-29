void generateSphere(float radius, int subdivisions, struct Mesh *outMesh) {
	int i, j;
	int vertCount = 0;
	int faceCount = 0;

	// Allocate memory for verts and faces
	outMesh->vert = (float*)malloc(sizeof(float) * 3 * (subdivisions + 1) * (subdivisions + 1));
	outMesh->face = (int*)malloc(sizeof(int) * 6 * subdivisions * subdivisions);

	// Generate verts
	for (i = 0; i <= subdivisions; i++) {
		float theta = (float)i / (float)subdivisions * M_PI;
		for (j = 0; j <= subdivisions; j++) {
			float phi = (float)j / (float)subdivisions * 2.0f * M_PI;
			float x = sinf(theta) * cosf(phi);
			float y = sinf(theta) * sinf(phi);
			float z = cosf(theta);
			outMesh->vert[vertCount++] = x * radius;
			outMesh->vert[vertCount++] = y * radius;
			outMesh->vert[vertCount++] = z * radius;
		}
	}

	// Generate faces
	for (i = 0; i < subdivisions; i++) {
		for (j = 0; j < subdivisions; j++) {
			int a = i * (subdivisions + 1) + j;
			int b = a + 1;
			int c = (i + 1) * (subdivisions + 1) + j;
			int d = c + 1;
			outMesh->face[faceCount++] = a;
			outMesh->face[faceCount++] = b;
			outMesh->face[faceCount++] = c;

			outMesh->face[faceCount++] = b;
			outMesh->face[faceCount++] = d;
			outMesh->face[faceCount++] = c;
		}
	}

	// Set the number of vertices and faces
	outMesh->vertCount = vertCount / 3;
	outMesh->faceCount = faceCount / 3;
}

void generateCube(float width, float length, float height, struct Mesh *outMesh) {
	float *v;
	int *f;

	// Allocate memory for the vertices and faces
	outMesh->vertCount = 8;
	outMesh->vert = (float *) malloc(sizeof(float) * outMesh->vertCount * 3);
	outMesh->faceCount = 12;
	outMesh->face = (int *) malloc(sizeof(int) * outMesh->faceCount * 3);

	// Define the vertices of the cube
	v = outMesh->vert;

	float halfX = width * 0.5;
	float halfY = length * 0.5;
	float halfZ = height * 0.5;

	v[0] = -halfX; v[1] = -halfY; v[2] = -halfZ; // bottom-left-back
	v[3] = halfX; v[4] = -halfY; v[5] = -halfZ; // bottom-right-back
	v[6] = halfX; v[7] = halfY; v[8] = -halfZ; // top-right-back
	v[9] = -halfX; v[10] = halfY; v[11] = -halfZ; // top-left-back

	v[12] = -halfX; v[13] = -halfY; v[14] = halfZ; // bottom-left-front
	v[15] = halfX; v[16] = -halfY; v[17] = halfZ; // bottom-right-front
	v[18] = halfX; v[19] = halfY; v[20] = halfZ; // top-right-front
	v[21] = -halfX; v[22] = halfY; v[23] = halfZ; // top-left-front

	// Define the faces of the cube
	f = outMesh->face;
	// front face
	f[0] = 0; f[1] = 1; f[2] = 2;
	f[3] = 2; f[4] = 3; f[5] = 0;
	// back face
	f[6] = 4; f[7] = 6; f[8] = 5;
	f[9] = 4; f[10] = 7; f[11] = 6;
	// left face
	f[12] = 4; f[13] = 5; f[14] = 1;
	f[15] = 1; f[16] = 0; f[17] = 4;
	// right face
	f[18] = 3; f[19] = 2; f[20] = 6;
	f[21] = 6; f[22] = 7; f[23] = 3;
	// top face
	f[24] = 4; f[25] = 0; f[26] = 3;
	f[27] = 3; f[28] = 7; f[29] = 4;
	// bottom face
	f[30] = 1; f[31] = 5; f[32] = 6;
	f[33] = 6; f[34] = 2; f[35] = 1;
}
