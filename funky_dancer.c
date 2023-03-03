#include <stdbool.h>
#include <stdio.h>
#include <SDL2/SDL.h>
#include <math.h>
#include "math_stuff.c"
#include "common_structs.c"

// gcc -Wall -Wextra -std=c99 -pedantic funky_dancer.c -ISDL2\include -L.\SDL2\lib -lmingw32 -lSDL2main -lSDL2 -o funky_dancer

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
	float axis[3] = { rx, ry, rz };
	len = sqrtf(axis[0]*axis[0] + axis[1]*axis[1] + axis[2]*axis[2]);
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
		float v[4] = { outMesh->vert[i*3], outMesh->vert[i*3 + 1], outMesh->vert[i*3 + 2], 1.0f };
		float v2[4];
		multiplyMatrixVector(t, v, v2);
		multiplyMatrixVector(t2, v2, v);
		multiplyMatrixVector(s3, v, v2);
		// Store transformed vertex back into mesh
		outMesh->vert[i*3] = v2[0];
		outMesh->vert[i*3 + 1] = v2[1];
		outMesh->vert[i*3 + 2] = v2[2];
	}
}

void generateSphere(float radius, int subdivisions, struct mesh *outMesh) {
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

// void line(int x0, int y0, int x1, int y1, SDL_Surface* surface, unsigned int color) { 
void line(int x0, int y0, int x1, int y1, char* pixels, unsigned int color) {
	// pitch: the length of a row of pixels in bytes (read-only)
	// const int pitch = surface->pitch;
	// char* pixels = surface->pixels;
	// need a bounds check
	int pitch = sizeof(char) * 2048;
	
	bool steep = false;
	if (abs(x0-x1) < abs(y0-y1)) {
		swap(&x0, &y0);
		swap(&x1, &y1);
		steep = true;
	}
	if (x0 > x1) {
		swap(&x0, &x1);
		swap(&y0, &y1);
	}
	int dx = x1-x0;
	int dy = y1-y0;
	int derror2 = abs(dy)*2;
	int error2 = 0;
	int y = y0;
	for (int x = x0; x <= x1; x++) {
		if (steep) {
			//image.set(y, x, color); 
			if (x >= 0 && x < 512 && y >= 0 && y < 512) {
				unsigned int* row = (unsigned int*)(pixels + pitch * y);
				row[x] = color;
			}
		} else {
			//image.set(x, y, color);
			if (x >= 0 && x < 512 && y >= 0 && y < 512) {
				unsigned int* row = (unsigned int*)(pixels + pitch * x);
				row[y] = color;
			}
		}
		error2 += derror2;
		if (error2 > dx) {
			y += (y1 > y0?1:-1);
			error2 -= dx*2;
		}
	}
}

//void draw_scene(SDL_Surface* surface, int width, int height, struct mesh *sphereMesh_ptr) {
void draw_scene(char* pixels, int width, int height, struct mesh *sphereMesh_ptr) {
	struct mesh sphereMesh = *sphereMesh_ptr;

	//int pitch = surface->pitch;

	// draw the mesh
	for (int i = 0; i < sphereMesh.faceCount; i++) {

		int faceIndex = i * 3;

		for (int j = 0; j < 3; j++) {
			int a = sphereMesh.face[faceIndex + j];
			int b = sphereMesh.face[faceIndex + (j + 1) % 3];

			struct Vec3f v0 = {
				.x = sphereMesh.vert[a * 3],
				.y = sphereMesh.vert[a * 3 + 1],
				.z = sphereMesh.vert[a * 3 + 2]
			};

			struct Vec3f v1 = {
				.x = sphereMesh.vert[b * 3],
				.y = sphereMesh.vert[b * 3 + 1],
				.z = sphereMesh.vert[b * 3 + 2]
			};

			int x0 = (v0.x + 1.) * 0.5 * width / 2. + (0.25 * width);
			int y0 = (v0.y + 1.) * 0.5 * height / 2. + (0.25 * height);
			int x1 = (v1.x + 1.) * 0.5 * width / 2. + (0.25 * width);
			int y1 = (v1.y + 1.) * 0.5 * height / 2. + (0.25 * height);

			line(x0, y0, x1, y1, pixels, 0x2299ff00);
		}
	}
}

void cloneMesh(struct mesh *originalMesh, struct mesh *newMesh) {
	// Allocate memory for the vertices and copy the values
	//if (newMesh->vert != NULL) {
	//free(newMesh->vert);
	//}
	newMesh->vert = malloc(sizeof(float) * originalMesh->vertCount * 3);
	memcpy(newMesh->vert, originalMesh->vert, sizeof(float) * originalMesh->vertCount * 3);
	newMesh->vertCount = originalMesh->vertCount;

	// Allocate memory for the faces and copy the values
	//if (newMesh->face != NULL) {
	//free(newMesh->face);
	//}
	newMesh->face = malloc(sizeof(int) * originalMesh->faceCount * 3);
	memcpy(newMesh->face, originalMesh->face, sizeof(int) * originalMesh->faceCount * 3);
	newMesh->faceCount = originalMesh->faceCount;
}

void cloneMeshToScene(struct mesh *originalMesh, struct mesh *newMesh) {
	// Allocate memory for the vertices and copy the values
	//if (newMesh->vert != NULL) {
	//free(newMesh->vert);
	//}
	newMesh->vert = malloc(sizeof(float) * originalMesh->vertCount * 3);
	memcpy(newMesh->vert, originalMesh->vert, sizeof(float) * originalMesh->vertCount * 3);
	newMesh->vertCount = originalMesh->vertCount;

	// Allocate memory for the faces and copy the values
	//if (newMesh->face != NULL) {
	//free(newMesh->face);
	//}
	newMesh->face = malloc(sizeof(int) * originalMesh->faceCount * 3);
	memcpy(newMesh->face, originalMesh->face, sizeof(int) * originalMesh->faceCount * 3);
	newMesh->faceCount = originalMesh->faceCount;
}

int main(int argc, char* argv[]) {
	char *funkyString = "Starting the funk!";
	printf("%s\n", funkyString);

	Uint32 startTicks, frameTicks;

	SDL_Window* window = SDL_CreateWindow("Funky Dancer", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, 512, 512, 0);
	// SDL_Renderer* renderer = SDL_CreateRenderer(window, -1, 0);
	SDL_SetWindowOpacity(window, 0.9);

	SDL_CreateRenderer(window, -1, 0);

	if (!window) {
		SDL_Quit();
		return 1;
	}

	SDL_Surface* screen = SDL_GetWindowSurface(window);
	SDL_Surface* surface = SDL_CreateRGBSurfaceWithFormat(0, 512, 512, 32, SDL_PIXELFORMAT_RGBX8888);

	int finishTheFunk = 0;

	// generate the mesh before the loop
	struct mesh sphereMesh;
	int subdivisions = 10;
	generateSphere(1.0, subdivisions, &sphereMesh);
	trs(&sphereMesh,
	    0, 0, 0,
	    0, 0, 0,
	    0.5, 0.5, 1);

	struct mesh sphereMeshShift;
	generateSphere(0.2, 5, &sphereMeshShift);
	trs(&sphereMeshShift,
	    1.5, 0, 0,
	    0, 0, 0,
	    1, 1, 1);

	struct mesh *allMeshes[500]; // limit to 500 for now
	int meshCount = 0;
	struct mesh *sceneObjects[500]; // limit to 500 for now
	int sceneMeshCount = 0;

	allMeshes[meshCount++] = &sphereMesh;
	allMeshes[meshCount++] = &sphereMeshShift;
	
	// first clone test, need to refine
	for (int i = 0; i < meshCount; i++) {
		struct mesh *originalMesh = allMeshes[i];

		// Create a new mesh for the scene
		struct mesh *newMesh = malloc(sizeof(struct mesh));

		// Clone the original mesh into the new mesh
		cloneMeshToScene(originalMesh, newMesh);

		// Add the new mesh to the scene meshes
		sceneObjects[i] = newMesh;

		sceneMeshCount++;
	}

	int delta = 0;

	while (!finishTheFunk) {
		startTicks = SDL_GetTicks();

		SDL_Event event;

		while (SDL_PollEvent(&event)) {
			if (event.type == SDL_QUIT) {
				finishTheFunk = 1;
			}
		}

		SDL_FillRect(surface, NULL, 0);

		// reuse memory
		for (int i = 0; i < meshCount; i++) {

			// need to free them, see if can do a check in the function
			free(sceneObjects[i]->vert);
			free(sceneObjects[i]->face);

			struct mesh *originalMesh = allMeshes[i];

			struct mesh *newMesh = sceneObjects[i];

			// Clone the original mesh into the new mesh
			cloneMeshToScene(originalMesh, newMesh);

			// Add the new mesh to the scene meshes
			sceneObjects[i] = newMesh;
		}

		trs(sceneObjects[0],
		    0, 0, 0,
		    0, 2 * delta, 1 * delta,
		    1, 1, 1);

		trs(sceneObjects[1],
		    0, 0, 0,
		    2 * delta, 5 * delta, 1.2 * delta,
		    1, 1, 1);

		SDL_LockSurface(surface);

		char* pixels = surface->pixels;

		for (int i = 0; i < sceneMeshCount; i++) {
			draw_scene(pixels, 512, 512, sceneObjects[i]);
		}

		SDL_UnlockSurface(surface);

		// copy to window
		SDL_BlitSurface(surface, NULL, screen, NULL);
		SDL_UpdateWindowSurface(window);

		//SDL_Delay(1000 / 12);
		float targetFrameTime = 1000 / 12;

		frameTicks = SDL_GetTicks() - startTicks;
		if (frameTicks < targetFrameTime) {
			SDL_Delay(targetFrameTime - frameTicks);
		}

		delta++;
	}

	// Free the mesh memory
	for (int i = 0; i < meshCount; i++) {
		free(allMeshes[i]->vert);
		free(allMeshes[i]->face);
		printf("clear success\n");
	}

	for (int i = 0; i < meshCount; i++) {
		free(sceneObjects[i]->vert);
		free(sceneObjects[i]->face);
		printf("clear success\n");
	}

	//free(sphereMeshShift.vert);
	//free(sphereMeshShift.face);

	SDL_Quit();

	return 0;
}