#include <stdbool.h>
#include <stdio.h>
#include <SDL2/SDL.h>
#include <math.h>

// gcc -Wall -Wextra -std=c99 -pedantic funky_dancer.c -ISDL2\include -L.\SDL2\lib -lmingw32 -lSDL2main -lSDL2 -o funky_dancer

struct mesh {
	float *vert;
	int vertCount;
	int *face;
	int faceCount;
};

struct Vec3f {
	float x;
	float y;
	float z;
};


void swap(int *xp, int *yp) {
	int temp = *xp;
	*xp = *yp;
	*yp = temp;
}

void rotateMesh(float zRotateAngle, struct mesh *outMesh) {
	int i;
	float cosAngle = cos(zRotateAngle);
	float sinAngle = sin(zRotateAngle);
	for (i = 0; i < outMesh->vertCount; i++) {
		float x = outMesh->vert[i * 3];
		float y = outMesh->vert[i * 3 + 1];
		outMesh->vert[i * 3] = x * cosAngle - y * sinAngle;
		outMesh->vert[i * 3 + 1] = x * sinAngle + y * cosAngle;
	}
}

void rotateMeshY(float zRotateAngle, struct mesh *outMesh) {
	int i;
	float cosAngle = cos(zRotateAngle);
	float sinAngle = sin(zRotateAngle);
	for (i = 0; i < outMesh->vertCount; i++) {
		float x = outMesh->vert[i * 3];
		float y = outMesh->vert[i * 3 + 1];
		float z = outMesh->vert[i * 3 + 2];
		outMesh->vert[i * 3] = x * cosAngle + z * sinAngle;
		outMesh->vert[i * 3 + 1] = y;
		outMesh->vert[i * 3 + 2] = -x * sinAngle + z * cosAngle;
	}
}

void translateMesh(float xShift, struct mesh *outMesh) {
	int i;
	for (i = 0; i < outMesh->vertCount; i++) {
		float x = outMesh->vert[i * 3];
		float y = outMesh->vert[i * 3 + 1];
		float z = outMesh->vert[i * 3 + 2];
		outMesh->vert[i * 3] = x + xShift;
		outMesh->vert[i * 3 + 1] = y;
		outMesh->vert[i * 3 + 2] = z;
	}
}

void generateSphere(float radius, int subdivisions, struct mesh *outMesh) {
	int i, j;
	int vertCount = 0;
	int faceCount = 0;

	// Allocate memory for verts and faces
	outMesh->vert = (float*)malloc(sizeof(float) * 3 * (subdivisions+1) * (subdivisions+1));
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
	if (x0>x1) { 
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
			y += (y1>y0?1:-1); 
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

			line(x0, y0, x1, y1, pixels, 0xff00ff00);
		}
	}

}

int main(int argc, char* argv[]) {
    char *funkyString = "Starting the funk!";
    printf("%s\n", funkyString);

	Uint32 startTicks, frameTicks;

	SDL_Window* window = SDL_CreateWindow("Funky Dancer", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, 512, 512, 0);
	// SDL_Renderer* renderer = SDL_CreateRenderer(window, -1, 0);
	SDL_CreateRenderer(window, -1, 0);

	if (!window) {
		SDL_Quit();
		return 1;
	}

	SDL_Surface* screen = SDL_GetWindowSurface(window);
	SDL_Surface* surface = SDL_CreateRGBSurfaceWithFormat(0, 512, 512,32, SDL_PIXELFORMAT_RGBX8888);

	int finishTheFunk = 0;

	// generate the mesh before the loop
	struct mesh sphereMesh;
	int subdivisions = 10;
	generateSphere(1.0, subdivisions, &sphereMesh);

	struct mesh sphereMeshShift;
	generateSphere(0.2, 5, &sphereMeshShift);
	translateMesh(1.5, &sphereMeshShift);

	struct mesh allMeshes[500]; // limit to 500 for now
	int meshCount = 0;

	while (!finishTheFunk) {
		startTicks = SDL_GetTicks();

		SDL_Event event;

		while (SDL_PollEvent(&event)) {
			if (event.type == SDL_QUIT) {
				finishTheFunk = 1;
			}
		}

		SDL_FillRect(surface, NULL, 0);

		rotateMesh(0.1, &sphereMesh);
		rotateMeshY(0.05, &sphereMesh);

		rotateMesh(0.05, &sphereMeshShift);
		rotateMeshY(0.07, &sphereMeshShift);

		SDL_LockSurface(surface);

		char* pixels = surface->pixels;

		// draw_scene(surface, 512, 512, &sphereMesh);
		draw_scene(pixels, 512, 512, &sphereMesh);

		// draw_scene(surface, 512, 512, &sphereMeshShift);
		draw_scene(pixels, 512, 512, &sphereMeshShift);

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
	}

	// Free the mesh memory
	free(sphereMesh.vert);
	free(sphereMesh.face);

	free(sphereMeshShift.vert);
	free(sphereMeshShift.face);

	SDL_Quit();

	return 0;
}