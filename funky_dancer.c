#include <stdbool.h>
#include <stdio.h>
#include <SDL2/SDL.h>
#include <math.h>

#include "common_structs.c"
#include "mat4.c"
#include "math_stuff.c"
#include "pixel_drawing.c"
#include "mesh_generators.c"

#ifdef __EMSCRIPTEN__
#include <emscripten.h>
#endif

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

void draw_scene(char* pixels, int width, int height, struct mesh *sphereMesh_ptr, struct OrthographicCamera3D *camera_ptr) {
    struct mesh sphereMesh = *sphereMesh_ptr;

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

void recurseChildren(
	int delta,
	struct mesh *sceneObjects[],
	int *sceneObjectsCounter_ptr,
	struct SceneObject *sceneObjectsForReal,
	struct Vector3 transformRotationStack[],
	struct Vector3 transformPositionStack[],
	int transformStackDepth
) {
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

void recurseChildrenSetup(
	struct mesh *sceneObjects[],
	int *sceneObjectsCounter_ptr,
	struct SceneObject *sceneObjectsForReal
) {
	int sceneObjectsCounter = *sceneObjectsCounter_ptr;

	struct mesh *originalMesh = sceneObjectsForReal->mesh;

	// Create a new mesh for the scene
	struct mesh *newMesh = malloc(sizeof(struct mesh));

	// Clone the original mesh into the new mesh
	cloneMeshToScene(originalMesh, newMesh);

	// Add the new mesh to the scene meshes
	sceneObjects[sceneObjectsCounter] = newMesh;

	//sceneObjectsCounter2++;
	iterateCounter(sceneObjectsCounter_ptr);

	// need to handle this better
	for (int j = 0; j < sceneObjectsForReal->childCount; j++) {
		recurseChildrenSetup(
		                sceneObjects,
		                sceneObjectsCounter_ptr,
		                sceneObjectsForReal->children[j]
		);
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
	struct mesh *sceneObjects[500];
	struct SceneObject *sceneObjectsForReal[500];
	int meshCount;
	int sceneMeshCount;
	int sceneObjectForRealCount;
	int sceneObjectsCounter2;
	int frameTicks;
	int startTicks;
	int finishTheFunk;
};

void interactionHandler(SDL_Event *event, struct AppProperties *appProperties) {
	if (event->type == SDL_MOUSEMOTION) {
		int mouseX = event->motion.x;
		int mouseY = event->motion.y;

		if (appProperties->mouseHandler.isDown) {
			appProperties->camera.position.x = (float)(appProperties->mouseHandler.startX - mouseX) * 0.003;
			appProperties->camera.position.y = (float)(appProperties->mouseHandler.startY - mouseY) * 0.003;
			//camera.rotation.x = (float)(mouseHandler.startX - mouseX) * 0.01;
		}
	}

	// mouseHandler
	//  handle the mouse down
	if (event->type == SDL_MOUSEBUTTONDOWN) {
		if (event->button.button == SDL_BUTTON_LEFT) {
			appProperties->mouseHandler.startX = event->button.x;
			appProperties->mouseHandler.startY = event->button.y;
			appProperties->mouseHandler.isDown = true;
		}
	}

	if (event->type == SDL_MOUSEBUTTONUP) {
		if (event->button.button == SDL_BUTTON_LEFT) {
			appProperties->mouseHandler.isDown = false;
		}
	}
}

void emscriptenLoop(void *arg) {
	struct AppProperties *appProperties = (struct AppProperties*)arg;

	SDL_SetRenderDrawColor(appProperties->renderer, 0, 0, 0, 255);
	SDL_RenderClear(appProperties->renderer);

	int sceneObjectsCounter = 0;

	SDL_Event event;

	while (SDL_PollEvent(&event)) {
		interactionHandler(&event, appProperties);
	}

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

void nativeLoop(void *arg) {
	struct AppProperties *appProperties = (struct AppProperties*)arg;

	appProperties->startTicks = SDL_GetTicks();

	SDL_Event event;

	while (SDL_PollEvent(&event)) {
		if (event.type == SDL_QUIT) {
			appProperties->finishTheFunk = 1;
		}

		interactionHandler(&event, appProperties);
	}

	SDL_FillRect(appProperties->surface, NULL, 0);

	int sceneObjectsCounter = 0;

	// attempt to render the scene meshes ----------------------------------
	for (int i = 0; i < appProperties->sceneObjectForRealCount; i++) {
		struct Vector3 cumulativeRotation[500] = {{.x = 0, .y = 0, .z = 0}};
		struct Vector3 cumulativeTransform[500] = {{.x = 0, .y = 0, .z = 0}};
		recurseChildren(appProperties->delta * 2, appProperties->sceneObjects, &sceneObjectsCounter, appProperties->sceneObjectsForReal[i], cumulativeRotation,
		                cumulativeTransform, 1);
	}

	SDL_LockSurface(appProperties->surface);

	char *pixels = appProperties->surface->pixels;

	for (int i = 0; i < sceneObjectsCounter; i++) {
		draw_scene(pixels, 640, 480, appProperties->sceneObjects[i], &appProperties->camera);
	}

	SDL_UnlockSurface(appProperties->surface);

	// copy to window
	SDL_BlitSurface(appProperties->surface, NULL, appProperties->screen, NULL);
	SDL_UpdateWindowSurface(appProperties->window);

	// SDL_Delay(1000 / 12);
	float targetFrameTime = 1000 / 24;

	appProperties->frameTicks = SDL_GetTicks() - appProperties->startTicks;
	if (appProperties->frameTicks < targetFrameTime) {
		SDL_Delay(targetFrameTime - appProperties->frameTicks);
	}

	appProperties->delta++;
}

int main(int argc, char *argv[]) {
    char *funkyString = "Starting the funk!";
    printf("%s\n", funkyString);

    Uint32 startTicks, frameTicks;

	struct AppProperties appProperties = { 
		.width = 640, 
		.height = 480,
		.window = SDL_CreateWindow("Funky Dancer", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, 640, 480, 0),
		.renderer = SDL_CreateRenderer(appProperties.window, -1, 0),
		.delta = 0,
		.sceneObjectForRealCount = 0,
		.sceneMeshCount = 0,
		.sceneObjectsCounter2 = 0,
		.frameTicks = 0,
		.startTicks = 0,
		.finishTheFunk = 0
	};

	SDL_SetWindowOpacity(appProperties.window, 1.0);

	if (!appProperties.window) {
        SDL_Quit();
        return 1;
    }

	appProperties.screen = SDL_GetWindowSurface(appProperties.window);
	appProperties.surface = SDL_CreateRGBSurfaceWithFormat(0, 640, 480, 32, SDL_PIXELFORMAT_RGBX8888);
	
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

	// generate the mesh before the loop
	//{
		struct mesh sphereMesh;
		int subdivisions = 8;
		generateSphere(0.3, subdivisions, &sphereMesh);

		struct SceneObject testSphere;
		testSphere.mesh = &sphereMesh;

		testSphere.transform = (struct Transform) {
			.position = (struct Vector3) { .x = 0.0f, .y = 0.0f, .z = 0.0f },
			.rotation = (struct Quaternion) { .w = 1.0, .x = 0.0f, .y = 0.0f, .z = 0.0f },
			.scale = (struct Vector3) { .x = 1.0f, .y = 1.0f, .z = 1.0f },
		};
		testSphere.attachPosition = (struct Vector3) { .x = 0.0f, .y = 0.0f, .z = 0.0f };
		testSphere.childCount = 0;

		appProperties.sceneObjectsForReal[appProperties.sceneObjectForRealCount++] = &testSphere;

		// test rendering a scene object
		struct mesh sceneCubeTest;
		generateCube(1.0, 1.0, 1.0, &sceneCubeTest);

		struct SceneObject testObject;
		testObject.mesh = &sceneCubeTest;

		trs(&sceneCubeTest,
		    0.0, 0.5, 0.0,
		    0, 0, 0,
		    0.3, 1.0, 0.3);

		testObject.transform = (struct Transform) {
			.position = (struct Vector3) { .x = 0.0f, .y = 0.0f, .z = 0.0f },
			.rotation = (struct Quaternion) { .w = 1.0, .x = 0.0f, .y = 0.0f, .z = 0.0f },
			.scale = (struct Vector3) { .x = 1.0f, .y = 1.0f, .z = 1.0f },
		};
		testObject.attachPosition = (struct Vector3) { .x = 0.0f, .y = 0.0f, .z = 0.0f };
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

		testObjectChild.transform = (struct Transform) {
			.position = (struct Vector3) { .x = 0.0f, .y = 1.0f, .z = 0.0f },
			.rotation = (struct Quaternion) { .w = 1.0, .x = 0.0f, .y = 0.0f, .z = 0.0f },
			.scale = (struct Vector3) { .x = 1.0f, .y = 1.0f, .z = 1.0f },
		};
		testObjectChild.attachPosition = (struct Vector3) { .x = 0.0f, .y = 0.0f, .z = 0.0f };
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

		testObjectChildNested.transform = (struct Transform) {
			.position = (struct Vector3) { .x = 1.0f, .y = 0.0f, .z = 0.0f },
			.rotation = (struct Quaternion) { .w = 1.0, .x = 0.0f, .y = 0.0f, .z = 0.0f },
			.scale = (struct Vector3) { .x = 1.0f, .y = 1.0f, .z = 1.0f },
		};
		testObjectChildNested.attachPosition = (struct Vector3) { .x = 0.0f, .y = 0.0f, .z = 0.0f };
		testObjectChildNested.childCount = 0;

		// add the child to the test mesh
		testObjectChild.children[0] = &testObjectChildNested;
		testObjectChild.childCount = 1;

		// Add another sphere at the end
		struct mesh sphereMeshEnd;
		generateSphere(0.4, 8, &sphereMeshEnd);

		trs(&sphereMeshEnd,
		    0.0, 0.0, 0.0,
		    0, 0, 0,
		    1, 1, 1);

		struct SceneObject sphereObjectEnd;
		sphereObjectEnd.mesh = &sphereMeshEnd;

		sphereObjectEnd.transform = (struct Transform) {
			.position = (struct Vector3) { .x = 1.0f, .y = 0.0f, .z = 0.0f },
			.rotation = (struct Quaternion) { .w = 1.0, .x = 0.0f, .y = 0.0f, .z = 0.0f },
			.scale = (struct Vector3) { .x = 1.0f, .y = 1.0f, .z = 1.0f },
		};
		sphereObjectEnd.attachPosition = (struct Vector3) { .x = 0.0f, .y = 0.0f, .z = 0.0f };
		sphereObjectEnd.childCount = 0;

		testObjectChildNested.children[0] = &sphereObjectEnd;
		testObjectChildNested.childCount = 1;

		//printf("meshgen = %i - childcount\n", testObjectChildNested.childCount);
	//}
    // fin!------------------------------------------------------------

    // clone the sceneObject stuff
    for (int i = 0; i < appProperties.sceneObjectForRealCount; i++) {
		recurseChildrenSetup(
		    appProperties.sceneObjects,
			&appProperties.sceneObjectsCounter2,
		    appProperties.sceneObjectsForReal[i]
		);
    }

	// use a different loop for emscripten
#ifdef __EMSCRIPTEN__
	emscripten_set_main_loop_arg(emscriptenLoop, &appProperties, -1, 1);
#else
    while (!appProperties.finishTheFunk) {
		nativeLoop(&appProperties);
    }
#endif

	for (int i = 0; i < appProperties.sceneObjectsCounter2; i++) {
        free(appProperties.sceneObjects[i]->vert);
        free(appProperties.sceneObjects[i]->face);
        printf("clear success\n");
    }

    // free(sphereMeshShift.vert);
    // free(sphereMeshShift.face);

    SDL_Quit();

    return 0;
}