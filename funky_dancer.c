#include <stdbool.h>
#include <stdio.h>
#include <SDL2/SDL.h>
#include <math.h>

#include "common_structs.c"
#include "mat4.c"
#include "math_stuff.c"
#include "mesh_generators.c"

#ifdef __EMSCRIPTEN__
#include <emscripten.h>
#endif

// gcc -Wall -Wextra -std=c99 -pedantic funky_dancer.c -ISDL2\include -L.\SDL2\lib -lmingw32 -lSDL2main -lSDL2 -o funky_dancer

struct MouseHandler {
    int startX;
    int startY;
    bool isDown;
};

struct OrthographicCamera3D {
    struct Vec3f position;
    struct Vec3f rotation;
    struct Quaternion q_rotation;
};

struct Vec3f WorldToScreen3D(struct OrthographicCamera3D camera, struct Vec3f worldPoint) {
    // calc view matrix
    struct Mat4 translation = Mat4_Translation((struct Vec3f){
                         .x = -camera.position.x,
                         .y = -camera.position.y, 
                         .z = -camera.position.z});
    struct Mat4 rotation = QuaternionToMatrix(camera.q_rotation);
    struct Mat4 view = Mat4_Multiply(translation, rotation);
    // calc orthographic projection matrix
    float left = -1.0f;
    float right = 1.0f;
    float top = 1.0f;
    float bottom = -1.0f;
    float near = -1.0f;
    float far = 1.0f;
    struct Mat4 projection = Mat4_Orthographic(left, right, top, bottom, near, far);

    // calc view-projection matrix
    struct Mat4 viewProjection = Mat4_Multiply(projection, view);

    // transform world point to screen space
    struct Vec3f screenPoint = Mat4_MultiplyVec3(viewProjection, worldPoint);

    return screenPoint;
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

// void line(int x0, int y0, int x1, int y1, SDL_Surface* surface, unsigned int color) { 
void line(int x0, int y0, int x1, int y1, char *pixels, unsigned int color) {
    // pitch: the length of a row of pixels in bytes (read-only)
    // const int pitch = surface->pitch;
    // char* pixels = surface->pixels;
    // need a bounds check
    // int pitch = sizeof(char) * 2048;
	int pitch = sizeof(char) * 640 * 4; // there are so many questions here, I put the description above and yet I keep this odd bit of code. Why?

    bool steep = false;
    if (abs(x0 - x1) < abs(y0 - y1)) {
        swap(&x0, &y0);
        swap(&x1, &y1);
        steep = true;
    }
    if (x0 > x1) {
        swap(&x0, &x1);
        swap(&y0, &y1);
    }
    int dx = x1 - x0;
    int dy = y1 - y0;
    int derror2 = abs(dy) * 2;
    int error2 = 0;
    int y = y0;
    for (int x = x0; x <= x1; x++) {
        if (steep) {
            // image.set(y, x, color);
            if (x >= 0 && x < 640 && y >= 0 && y < 480) {
                unsigned int *row = (unsigned int *)(pixels + pitch * y);
                row[x] = color;
            }
        } else {
            // image.set(x, y, color);
            if (x >= 0 && x < 480 && y >= 0 && y < 640) {
                unsigned int *row = (unsigned int *)(pixels + pitch * x);
                row[y] = color;
            }
        }
        error2 += derror2;
        if (error2 > dx) {
            y += (y1 > y0 ? 1 : -1);
            error2 -= dx * 2;
        }
    }
}

//void draw_scene(SDL_Surface* surface, int width, int height, struct mesh *sphereMesh_ptr) {
void draw_scene(char* pixels, int width, int height, struct mesh *sphereMesh_ptr, struct OrthographicCamera3D *camera_ptr) {
    struct mesh sphereMesh = *sphereMesh_ptr;

    // char title[64];
	//snprintf(title, sizeof(title), "Mouse Position - X: %f, Y: %f", camera_ptr->position.x, camera_ptr->position.y);
    // SDL_SetWindowTitle(window, title);

    // int pitch = surface->pitch;

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

            // fix this!!!
            // int x0 = (v0.x + 1.) * 0.5 * height / 2. + (0.25 * height);
            // int y0 = (v0.y + 1.) * 0.5 * width / 2. + (0.25 * width);
            // int x1 = (v1.x + 1.) * 0.5 * height / 2. + (0.25 * height);
            // int y1 = (v1.y + 1.) * 0.5 * width / 2. + (0.25 * width);

            // line(x0, y0, x1, y1, pixels, 0x2299ff00);

            struct Vec3f screenPoint_1 = WorldToScreen3D(*camera_ptr, v0);
            struct Vec3f screenPoint_2 = WorldToScreen3D(*camera_ptr, v1);

            float horiMult = 100;
            float vertMult = 100;
            float horiOffset = 640 / 2;
            float vertOffset = 480 / 2;
            float screenX0 = screenPoint_1.x * horiMult + vertOffset;
            float screenY0 = screenPoint_1.y * vertMult + horiOffset;
            float screenX1 = screenPoint_2.x * horiMult + vertOffset;
            float screenY1 = screenPoint_2.y * vertMult + horiOffset;

            line(screenX0, screenY0, screenX1, screenY1, pixels, 0x2299ff55);
        }
    }
}

void cloneMesh(struct mesh *originalMesh, struct mesh *newMesh) {
    // Allocate memory for the vertices and copy the values
    // if (newMesh->vert != NULL) {
    // free(newMesh->vert);
    //}
    newMesh->vert = malloc(sizeof(float) * originalMesh->vertCount * 3);
    memcpy(newMesh->vert, originalMesh->vert, sizeof(float) * originalMesh->vertCount * 3);
    newMesh->vertCount = originalMesh->vertCount;

    // Allocate memory for the faces and copy the values
    // if (newMesh->face != NULL) {
    // free(newMesh->face);
    //}
    newMesh->face = malloc(sizeof(int) * originalMesh->faceCount * 3);
    memcpy(newMesh->face, originalMesh->face, sizeof(int) * originalMesh->faceCount * 3);
    newMesh->faceCount = originalMesh->faceCount;
}

void cloneMeshToScene(struct mesh *originalMesh, struct mesh *newMesh) {
    // Allocate memory for the vertices and copy the values
    // if (newMesh->vert != NULL) {
    // free(newMesh->vert);
    //}
    newMesh->vert = malloc(sizeof(float) * originalMesh->vertCount * 3);
    memcpy(newMesh->vert, originalMesh->vert, sizeof(float) * originalMesh->vertCount * 3);
    newMesh->vertCount = originalMesh->vertCount;

    // Allocate memory for the faces and copy the values
    // if (newMesh->face != NULL) {
    // free(newMesh->face);
    //}
    newMesh->face = malloc(sizeof(int) * originalMesh->faceCount * 3);
    memcpy(newMesh->face, originalMesh->face, sizeof(int) * originalMesh->faceCount * 3);
    newMesh->faceCount = originalMesh->faceCount;
}

void cloneMeshToScene_NOMALLOC(struct mesh *originalMesh, struct mesh *newMesh) {
    memcpy(newMesh->vert, originalMesh->vert, sizeof(float) * originalMesh->vertCount * 3);
    newMesh->vertCount = originalMesh->vertCount;

    memcpy(newMesh->face, originalMesh->face, sizeof(int) * originalMesh->faceCount * 3);
    newMesh->faceCount = originalMesh->faceCount;
}
void freeMem(struct mesh *sceneObjects[], int sceneObjectsCounter) {
    // free the memory for these two things
    free(sceneObjects[sceneObjectsCounter]->vert);
    free(sceneObjects[sceneObjectsCounter]->face);
}

void iterateCounter(int *sceneObjectCounter_ptr) {
	(*sceneObjectCounter_ptr)++;
}

void testCloneMesh(struct mesh *sceneObjects[], int sceneObjectsCounter, struct mesh *originalMesh2) {
    struct mesh *newMesh2 = sceneObjects[sceneObjectsCounter];

    cloneMeshToScene_NOMALLOC(originalMesh2, newMesh2);

    sceneObjects[sceneObjectsCounter] = newMesh2;
}

void recurseChildren(int delta,
                     struct mesh *sceneObjects[],
                     int *sceneObjectsCounter_ptr,
                     struct SceneObject *sceneObjectsForReal,
                     struct Vector3 transformRotationStack[],
                     struct Vector3 transformPositionStack[],
                     int transformStackDepth) {
    int sceneObjectsCounter = *sceneObjectsCounter_ptr;

#ifdef __EMSCRIPTEN__
#else
	freeMem(sceneObjects, sceneObjectsCounter); // add back in! breaks emscripten for some reason
#endif

    testCloneMesh(sceneObjects, sceneObjectsCounter, sceneObjectsForReal->mesh);

    struct Transform currentTransform = sceneObjectsForReal->transform;

	// rotate first
	trs(sceneObjects[sceneObjectsCounter],
	    0, 0, 0,
	    currentTransform.rotation.x + delta, currentTransform.rotation.y, currentTransform.rotation.z + delta,
	    1, 1, 1);

	// then move into position
	trs(sceneObjects[sceneObjectsCounter],
	    currentTransform.position.x, currentTransform.position.y, currentTransform.position.z,
	    0, 0, 0,
	    1, 1, 1);

    for (int i = transformStackDepth - 1; i >= 0; i--) {
        // go backwards through transform stack
        // rotate first
		trs(sceneObjects[sceneObjectsCounter],
		    0, 0, 0,
		    transformRotationStack[i].x + 0, transformRotationStack[i].y, transformRotationStack[i].z,
		    1, 1, 1);

        // then move into position
		trs(sceneObjects[sceneObjectsCounter],
		    transformPositionStack[i].x, transformPositionStack[i].y, transformPositionStack[i].z,
		    0, 0, 0,
		    1, 1, 1);
    }

    // Update transform stacks
    struct Vector3 newRotation = {.x = currentTransform.rotation.x + delta,
                                  .y = currentTransform.rotation.y,
                                  .z = currentTransform.rotation.z + delta};
	struct Vector3 newPosition = {
		.x = currentTransform.position.x,
		.y = currentTransform.position.y,
		.z = currentTransform.position.z
	};

    transformRotationStack[transformStackDepth] = newRotation;
    transformPositionStack[transformStackDepth] = newPosition;

    iterateCounter(sceneObjectsCounter_ptr);

    for (int j = 0; j < sceneObjectsForReal->childCount; j++) {
		recurseChildren(delta * 2,
		                sceneObjects,
		                sceneObjectsCounter_ptr,
		                sceneObjectsForReal->children[j],
		                transformRotationStack,
		                transformPositionStack,
		                transformStackDepth + 1);
    }
}

struct AppProperties {
	int width;
	int height;
	SDL_Window *window;
	SDL_Surface *screen;
	SDL_Surface *surface;
	SDL_Renderer *renderer;
	SDL_Texture *texture;
	int delta;
	struct MouseHandler mouseHandler; // change to a proper set up
	struct OrthographicCamera3D camera; // change to a proper set up
	struct mesh *allMeshes[500];
	struct mesh *sceneObjects[500];
	struct SceneObject *sceneObjectsForReal[500];
	int meshCount;
	int sceneMeshCount;
	int sceneObjectForRealCount;
	int sceneObjectsCounter2;
};

void emscriptenLoop(void *arg) {
	struct AppProperties *appProperties = (struct AppProperties*)arg;

	SDL_SetRenderDrawColor(appProperties->renderer, 0, 0, 0, 255);
	SDL_RenderClear(appProperties->renderer);

	int sceneObjectsCounter = 0;

	// attempt to render the scene meshes ----------------------------------
	for (int i = 0; i < appProperties->sceneObjectForRealCount; i++) {
		struct Vector3 cumulativeRotation[500] = {{.x = 0, .y = 0, .z = 0}};
		struct Vector3 cumulativeTransform[500] = {{.x = 0, .y = 0, .z = 0}};
		recurseChildren(appProperties->delta * 2,
		                appProperties->sceneObjects,
		                &sceneObjectsCounter,
		                appProperties->sceneObjectsForReal[i],
		                cumulativeRotation,
		                cumulativeTransform,
		                1);
	}

	// use an sdl texture for emscripten
	if (!appProperties->texture) {
		appProperties->texture = SDL_CreateTexture(appProperties->renderer,
		                                           SDL_PIXELFORMAT_RGBX8888,
		                                           SDL_TEXTUREACCESS_STREAMING,
		                                           //SDL_TEXTUREACCESS_TARGET,
		                                           640, 480);
	}

	//char *pixels;
	char *pixels = appProperties->surface->pixels;
	int pitch;
	SDL_LockTexture(appProperties->texture, NULL, (void **)&pixels, &pitch);

	// work out a better way to clear the texture(eventually may not be need though)
	for (int i = 0; i < 640 * 480 * 4; ++i) {
		pixels[i] = 0x00000000;
	}

	for (int i = 0; i < sceneObjectsCounter; i++) {
		draw_scene(pixels, 640, 480, appProperties->sceneObjects[i], &appProperties->camera);
	}

	SDL_UnlockTexture(appProperties->texture);

	SDL_RenderCopy(appProperties->renderer, appProperties->texture, NULL, NULL);
	SDL_RenderPresent(appProperties->renderer);

	appProperties->delta++;
}

int main(int argc, char *argv[]) {
    char *funkyString = "Starting the funk!";
    printf("%s\n", funkyString);

    Uint32 startTicks, frameTicks;

	struct AppProperties appProperties = { .width = 640, .height = 480 };
	appProperties.window = SDL_CreateWindow("Funky Dancer", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, 640, 480, 0);

    //SDL_Window *window = SDL_CreateWindow("Funky Dancer", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, 640, 480, 0);
    // SDL_Renderer* renderer = SDL_CreateRenderer(window, -1, 0);
	SDL_SetWindowOpacity(appProperties.window, 1.0);

	appProperties.renderer = SDL_CreateRenderer(appProperties.window, -1, 0);

	if (!appProperties.window) {
        SDL_Quit();
        return 1;
    }

	appProperties.screen = SDL_GetWindowSurface(appProperties.window);
	appProperties.surface = SDL_CreateRGBSurfaceWithFormat(0, 640, 480, 32, SDL_PIXELFORMAT_RGBX8888);
	appProperties.delta = 0;

	appProperties.sceneObjectForRealCount = 0;
	appProperties.sceneMeshCount = 0;
	appProperties.sceneObjectsCounter2 = 0;

	appProperties.mouseHandler = (struct MouseHandler){
		.startX = 0.0,
		.startY = 0.0,
		.isDown = false
	};

	appProperties.camera = (struct OrthographicCamera3D){
		.position = {.x = 0.0, .y = 0.0, .z = 0.0},
		.rotation = {.x = 0.0f, .y = 0.0f, .z = 0.0f},
		.q_rotation = {.w = 0.0f, .x = 0.0f, .y = 0.0f, .z = 0.0f},
	};

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

    struct mesh cubeMesh;
    generateCube(0.8, 0.5, 0.7, &cubeMesh);
	trs(&cubeMesh,
	    2.0, 0, 0,
	    0, 0, 0,
	    1, 1, 1);


    struct mesh *allMeshes[500]; // limit to 500 for now
    int meshCount = 0;
    //struct mesh *sceneObjects[500]; // limit to 500 for now
    int sceneMeshCount = 0;

    struct SceneObject *sceneObjectsForReal[500]; // limit to 500 for now
    int sceneObjectForRealCount = 0;

    // test rendering a scene object
    struct mesh sceneCubeTest;
    generateCube(1.0, 1.0, 1.0, &sceneCubeTest);

    struct SceneObject testObject;
    testObject.mesh = &sceneCubeTest;

	trs(&sceneCubeTest,
	    0.0, 0.5, 0.0,
	    0, 0, 0,
	    0.3, 1.0, 0.3);

    testObject.transform.position = (struct Vector3){.x = 0.0f, .y = 0.0f, .z = 0.0f};
    testObject.transform.scale = (struct Vector3){.x = 1.0f, .y = 1.0f, .z = 1.0f};
    testObject.transform.rotation = (struct Quaternion){.w = 1.0, .x = 0.0f, .y = 0.0f, .z = 0.0f};
    testObject.attachPosition = (struct Vector3){.x = 0.0f, .y = 0.0f, .z = 0.0f};
    testObject.childCount = 0;

	appProperties.sceneObjectsForReal[appProperties.sceneObjectForRealCount++] = &testObject;

    // test adding a child
    struct mesh sceneCubeChild;
    generateCube(1.0, 1.0, 1.0, &sceneCubeChild);

	trs(&sceneCubeChild,
	    0.5, 0.0, 0.0,
	    0, 0, 0,
	    1, 0.3, 0.3);
	

    struct SceneObject testObjectChild;
    testObjectChild.mesh = &sceneCubeChild;

    testObjectChild.transform.position = (struct Vector3){.x = 0.0f, .y = 1.0f, .z = 0.0f};
    testObjectChild.transform.scale = (struct Vector3){.x = 1.0f, .y = 1.0f, .z = 1.0f};
    testObjectChild.transform.rotation = (struct Quaternion){.w = 1.0, .x = 0.0f, .y = 0.0f, .z = 0.0f};
    testObjectChild.attachPosition = (struct Vector3){.x = 0.0f, .y = 0.0f, .z = 0.0f};
    testObjectChild.childCount = 0;

    // add the child to the test mesh
    testObject.children[0] = &testObjectChild;
    testObject.childCount = 1;

	// adding a third child WOW!------------------------------------------------------------
	struct mesh sceneCubeChildNested;
    generateCube(1.0, 1.0, 1.0, &sceneCubeChildNested);

	trs(&sceneCubeChildNested,
	    0.5, 0.0, 0.0,
	    0, 0, 0,
	    1, 0.3, 0.3);
	

    struct SceneObject testObjectChildNested;
    testObjectChildNested.mesh = &sceneCubeChildNested;

    testObjectChildNested.transform.position = (struct Vector3){.x = 1.0f, .y = 0.0f, .z = 0.0f};
    testObjectChildNested.transform.scale = (struct Vector3){.x = 1.0f, .y = 1.0f, .z = 1.0f};
    testObjectChildNested.transform.rotation = (struct Quaternion){.w = 1.0, .x = 0.0f, .y = 0.0f, .z = 0.0f};
    testObjectChildNested.attachPosition = (struct Vector3){.x = 0.0f, .y = 0.0f, .z = 0.0f};
    testObjectChildNested.childCount = 0;

    // add the child to the test mesh
    testObjectChild.children[0] = &testObjectChildNested;
    testObjectChild.childCount = 1;
    // fin!------------------------------------------------------------

    // sceneObjectsForReal[sceneObjectForRealCount++] = &testObject;

    allMeshes[meshCount++] = &sphereMesh;
    allMeshes[meshCount++] = &sphereMeshShift;
    allMeshes[meshCount++] = &cubeMesh;

    //int sceneObjectsCounter2 = 0;
    // clone the sceneObject stuff
    for (int i = 0; i < appProperties.sceneObjectForRealCount; i++) {
		struct mesh *originalMesh = appProperties.sceneObjectsForReal[i]->mesh;

        // Create a new mesh for the scene
        struct mesh *newMesh = malloc(sizeof(struct mesh));

        // Clone the original mesh into the new mesh
        cloneMeshToScene(originalMesh, newMesh);

        // Add the new mesh to the scene meshes
		appProperties.sceneObjects[appProperties.sceneObjectsCounter2] = newMesh;

		appProperties.sceneObjectsCounter2++;

		printf("%i childcount", appProperties.sceneObjectsForReal[i]->childCount);

        // need to handle this better
		for (int j = 0; j < appProperties.sceneObjectsForReal[i]->childCount; j++) {
			struct mesh *originalMesh2 = appProperties.sceneObjectsForReal[i]->children[j]->mesh;
            struct mesh *newMesh2 = malloc(sizeof(struct mesh));

            // Clone the original mesh into the new mesh
            cloneMeshToScene(originalMesh2, newMesh2);

            // Add the new mesh to the scene meshes
			appProperties.sceneObjects[appProperties.sceneObjectsCounter2] = newMesh2;

			appProperties.sceneObjectsCounter2++;

			for (int k = 0; k < appProperties.sceneObjectsForReal[i]->children[j]->childCount; k++) {
				struct mesh *originalMesh3 = appProperties.sceneObjectsForReal[i]->children[j]->children[k]->mesh;
                struct mesh *newMesh3 = malloc(sizeof(struct mesh));

                // Clone the original mesh into the new mesh
                cloneMeshToScene(originalMesh3, newMesh3);

                // Add the new mesh to the scene meshes
				appProperties.sceneObjects[appProperties.sceneObjectsCounter2] = newMesh3;

				appProperties.sceneObjectsCounter2++;
            }
        }
    }

    //int delta = 0;

	// use a different loop for emscripten
#ifdef __EMSCRIPTEN__
	emscripten_set_main_loop_arg(emscriptenLoop, &appProperties, 24, 1);
#else
    while (!finishTheFunk) {
        startTicks = SDL_GetTicks();

        SDL_Event event;

        while (SDL_PollEvent(&event)) {
            if (event.type == SDL_QUIT) {
                finishTheFunk = 1;
            }

            if (event.type == SDL_MOUSEMOTION) {
                int mouseX = event.motion.x;
                int mouseY = event.motion.y;

				if (appProperties.mouseHandler.isDown) {
					appProperties.camera.position.x = (float)(appProperties.mouseHandler.startX - mouseX) * 0.003;
					appProperties.camera.position.y = (float)(appProperties.mouseHandler.startY - mouseY) * 0.003;
					//camera.rotation.x = (float)(mouseHandler.startX - mouseX) * 0.01;
				}
			}

            // mouseHandler
            //  handle the mouse down
            if (event.type == SDL_MOUSEBUTTONDOWN) {
                if (event.button.button == SDL_BUTTON_LEFT) {
					appProperties.mouseHandler.startX = event.button.x;
					appProperties.mouseHandler.startY = event.button.y;
					appProperties.mouseHandler.isDown = true;
                }
            }

            if (event.type == SDL_MOUSEBUTTONUP) {
                if (event.button.button == SDL_BUTTON_LEFT) {
					appProperties.mouseHandler.isDown = false;
                }
            }
        }

		SDL_FillRect(appProperties.surface, NULL, 0);

        int sceneObjectsCounter = 0;

        // attempt to render the scene meshes ----------------------------------
        for (int i = 0; i < appProperties.sceneObjectForRealCount; i++) {
            struct Vector3 cumulativeRotation[500] = {{.x = 0, .y = 0, .z = 0}};
            struct Vector3 cumulativeTransform[500] = {{.x = 0, .y = 0, .z = 0}};
			recurseChildren(appProperties.delta * 2, appProperties.sceneObjects, &sceneObjectsCounter, appProperties.sceneObjectsForReal[i], cumulativeRotation,
                            cumulativeTransform, 1);
        }

		SDL_LockSurface(appProperties.surface);

		char *pixels = appProperties.surface->pixels;

		//char title[64];
		//snprintf(title, sizeof(title), "Mouse Position - X: %f, Y: %f", camera.position.x, camera.position.y);

		//SDL_SetWindowTitle(window, title);

        for (int i = 0; i < sceneObjectsCounter; i++) {
			draw_scene(pixels, 640, 480, appProperties.sceneObjects[i], &appProperties.camera);
        }

		SDL_UnlockSurface(appProperties.surface);

        // copy to window
		SDL_BlitSurface(appProperties.surface, NULL, appProperties.screen, NULL);
		SDL_UpdateWindowSurface(appProperties.window);

        // SDL_Delay(1000 / 12);
        float targetFrameTime = 1000 / 24;

        frameTicks = SDL_GetTicks() - startTicks;
        if (frameTicks < targetFrameTime) {
            SDL_Delay(targetFrameTime - frameTicks);
        }

		appProperties.delta++;
    }
#endif

    // Free the mesh memory
    for (int i = 0; i < meshCount; i++) {
        free(allMeshes[i]->vert);
        free(allMeshes[i]->face);
        printf("clear success\n");
    }

    for (int i = 0; i < meshCount; i++) {
        free(appProperties.sceneObjects[i]->vert);
        free(appProperties.sceneObjects[i]->face);
        printf("clear success\n");
    }

    // free(sphereMeshShift.vert);
    // free(sphereMeshShift.face);

    SDL_Quit();

    return 0;
}