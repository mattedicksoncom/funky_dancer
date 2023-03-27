#include <stdbool.h>
#include <stdio.h>
#include <SDL2/SDL.h>
#include <math.h>

#include "common_structs.c"
#include "mat4.c"
#include "math_stuff.c"
#include "pixel_drawing.c"
#include "mesh_generators.c"
#include "matcap_test.c"

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

uint32_t multiplyColorByFloat(uint32_t color, float factor) {
	uint8_t r = (color >> 24) & 0xFF;
	uint8_t g = (color >> 16) & 0xFF;
	uint8_t b = (color >> 8) & 0xFF;

	r = (uint8_t)(r * factor);
	g = (uint8_t)(g * factor);
	b = (uint8_t)(b * factor);

	uint32_t newColor = (r << 24) | (g << 16) | (b << 8);
	return newColor;
}

void draw_scene(char* pixels,
                int width, int height, struct mesh *sphereMesh_ptr,
                struct OrthographicCamera3D *camera_ptr, 
                float *depthBuffer, char *matCap1) {
    struct mesh sphereMesh = *sphereMesh_ptr;
	uint32_t color = sphereMesh.color;
	struct Vec3f light_dir = { .x = 0, .y = 0, .z = -1 };

	for (int i = 0; i < sphereMesh.faceCount; i++) { 
		//std::vector<int> face = model->face(i); 
		struct Vec3f screen_coords[3]; 
		struct Vec3f screen_normal_coords[3]; 
		struct Vec3f world_coords[3]; 
		int faceIndex = i * 3;

		for (int j = 0; j < 3; j++) {
			int a = sphereMesh.face[faceIndex + j];

			struct Vec3f v0 = {
				.x = sphereMesh.vert[a * 3],
				.y = sphereMesh.vert[a * 3 + 1],
				.z = sphereMesh.vert[a * 3 + 2]
			};

			// fix this!!!
			struct Vec3f screenPoint_1 = WorldToScreen3D(*camera_ptr, v0);

			float horiMult = 100;
			float vertMult = 100;
			float horiOffset = 640 / 2;
			float vertOffset = 480 / 2;

			float screenX0 = (screenPoint_1.x * horiMult) + 320;
			float screenY0 = (screenPoint_1.y * vertMult) + 240;


			screen_coords[j] = (struct Vec3f){
				.x = screenX0,
				.y = screenY0,
				.z = screenPoint_1.z
			};
			screen_normal_coords[j] = (struct Vec3f){
				.x = screenPoint_1.x,
				.y = screenPoint_1.y,
				.z = screenPoint_1.z
			};
			world_coords[j] = v0; 
		} 

		//struct Vec3f n = vec3f_CrossProduct(
			//vec3f_subtract(world_coords[2], world_coords[0]),
			//vec3f_subtract(world_coords[1], world_coords[0])
		//);
		struct Vec3f n = vec3f_CrossProduct(
			vec3f_subtract(screen_normal_coords[2], screen_normal_coords[0]),
			vec3f_subtract(screen_normal_coords[1], screen_normal_coords[0])
		);

		vec3f_normalize(&n);
		vec3f_normalize(&light_dir);
		float intensity = vec3f_dotProduct(n, light_dir);

		uint32_t newColor = multiplyColorByFloat(color, intensity);

		// attempt to use the matcap
		float u = 0.5f + (n.x * 0.5f);
		float v = 0.5f - (n.y * 0.5f);

		// get the matcap pixel pos
		int imageDimensions = 64;
		int x = (int)(u * imageDimensions) % imageDimensions;
		int y = (int)(v * imageDimensions) % imageDimensions;
		int pixelIndex = (x + y * imageDimensions) * 4; // Assumes the matcap data has 4 channels (RGBA)
		
		unsigned char color[4];
		color[0] = matCap1[pixelIndex];
		color[1] = matCap1[pixelIndex + 1];
		color[2] = matCap1[pixelIndex + 2];
		color[3] = matCap1[pixelIndex + 3];
		Uint32 pixelColor = 0;

		// maybe make a function for this, i hate bitshift
		pixelColor |= (Uint32)color[0] << 24; // r
		pixelColor |= (Uint32)color[1] << 16; // g
		pixelColor |= (Uint32)color[2] << 8;  // b

		if (intensity > 0) {
			drawTriangle(
				screen_coords,
				pixels,
				depthBuffer,
				pixelColor
				//0x2299ff55 * intensity
			);
		}
	}

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

			struct Vec3f screenPoint_1 = WorldToScreen3D(*camera_ptr, v0);
			struct Vec3f screenPoint_2 = WorldToScreen3D(*camera_ptr, v1);

			float horiMult = 100;
			float vertMult = 100;
			float horiOffset = 640 / 2;
			float vertOffset = 480 / 2;
			float screenX0 = screenPoint_1.x * horiMult + horiOffset;
			float screenY0 = screenPoint_1.y * vertMult + vertOffset;
			float screenX1 = screenPoint_2.x * horiMult + horiOffset;
			float screenY1 = screenPoint_2.y * vertMult + vertOffset;

			//line(screenX0, screenY0, screenX1, screenY1, pixels, 0x2299ff55);
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

	newMesh->color = originalMesh->color;
}

void cloneMeshToScene_NOMALLOC(struct mesh *originalMesh, struct mesh *newMesh) {
    memcpy(newMesh->vert, originalMesh->vert, sizeof(float) * originalMesh->vertCount * 3);
    newMesh->vertCount = originalMesh->vertCount;

    memcpy(newMesh->face, originalMesh->face, sizeof(int) * originalMesh->faceCount * 3);
    newMesh->faceCount = originalMesh->faceCount;

	newMesh->color = originalMesh->color;
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
	struct SceneObject *currentSceneObject,
	struct Vec3f transformRotationStack[],
	struct Vec3f transformPositionStack[],
	struct SceneObject sceneObjectStack[],
	int transformStackDepth
) {
    int sceneObjectsCounter = *sceneObjectsCounter_ptr;

#ifdef __EMSCRIPTEN__
#else
	freeMem(sceneObjects, sceneObjectsCounter); // add back in! breaks emscripten for some reason
#endif

	testCloneMesh(sceneObjects, sceneObjectsCounter, currentSceneObject->mesh);
	struct mesh *currentMesh = sceneObjects[sceneObjectsCounter];

	struct Transform currentTransform = currentSceneObject->transform;
	struct Quaternion currentRotation = currentTransform.rotation;

	// rotate first
	eularRotate(currentMesh,
	            currentRotation.x + delta * 0.2,
	            currentRotation.y,
	            currentRotation.z + delta* 0.2);

	// then move into position
	trs(currentMesh,
	    currentTransform.position.x, currentTransform.position.y, currentTransform.position.z,
	    0, 0, 0,
	    1, 1, 1);

	// go backwards through transform stack
    for (int i = transformStackDepth - 1; i >= 0; i--) {
        // rotate first
		eularRotate(currentMesh,
		            transformRotationStack[i].x + 0,
		            transformRotationStack[i].y,
		            transformRotationStack[i].z);

        // then move into position
		trs(currentMesh,
		    transformPositionStack[i].x, transformPositionStack[i].y, transformPositionStack[i].z,
		    0, 0, 0,
		    1, 1, 1);
    }

    // Update transform stacks
	transformRotationStack[transformStackDepth] = (struct Vec3f){
		.x = currentTransform.rotation.x + delta * 0.2,
		.y = currentTransform.rotation.y,
		.z = currentTransform.rotation.z + delta* 0.2
	};
	transformPositionStack[transformStackDepth] = (struct Vec3f){
		.x = currentTransform.position.x,
		.y = currentTransform.position.y,
		.z = currentTransform.position.z
	};

    iterateCounter(sceneObjectsCounter_ptr);

	for (int j = 0; j < currentSceneObject->childCount; j++) {
		recurseChildren(delta * 2,
		                sceneObjects,
		                sceneObjectsCounter_ptr,
		                currentSceneObject->children[j],
		                transformRotationStack,
		                transformPositionStack,
		                sceneObjectStack,
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
	struct Vec3f transformPositionStack[500];
	struct Vec3f transformRotationStack[500];
	int meshCount;
	int sceneMeshCount;
	int sceneObjectForRealCount;
	int sceneObjectsCounter2;
	int frameTicks;
	int startTicks;
	int finishTheFunk;
	float* depthBuffer;
	char* matCap1;
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

	//SDL_SetRenderDrawColor(appProperties->renderer, 0, 0, 0, 255);
	//SDL_RenderClear(appProperties->renderer);

	int sceneObjectsCounter = 0;

	SDL_Event event;

	while (SDL_PollEvent(&event)) {
		interactionHandler(&event, appProperties);
	}

	// attempt to render the scene meshes ----------------------------------
	for (int i = 0; i < appProperties->sceneObjectForRealCount; i++) {
		//struct Vec3f cumulativeRotation[500] = {{.x = 0, .y = 0, .z = 0}};
		//struct Vec3f cumulativeTransform[500] = {{.x = 0, .y = 0, .z = 0}};

		appProperties->transformRotationStack[0] = (struct Vec3f){.x = 0, .y = 0, .z = 0};
		appProperties->transformPositionStack[0] = (struct Vec3f){.x = 0, .y = sin(appProperties->delta * 0.01) * 0.5, .z = 0};

		struct SceneObject sceneObjectStack[1];
		recurseChildren(
			appProperties->delta * 2,
			appProperties->sceneObjects,
			&sceneObjectsCounter,
			appProperties->sceneObjectsForReal[i],
			appProperties->transformRotationStack,
			appProperties->transformPositionStack,
			sceneObjectStack,
			1
		);
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
	//float *depthBuffer = (float *)malloc(640 * 480 * sizeof(float));
	SDL_LockTexture(appProperties->texture, NULL, (void **)&pixels, &pitch);

	// work out a better way to clear the texture(eventually may not be need though)
	for (int i = 0; i < 640 * 480; ++i) {
		appProperties->depthBuffer[i] = 0x00000000;
		pixels[i] = 0x00000000;
	}
	for (int i = 640 * 480; i < 640 * 480 * 4; ++i) {
		pixels[i] = 0x00000000;
	}
	

	for (int i = 0; i < sceneObjectsCounter; i++) {
		draw_scene(pixels, 640, 480, appProperties->sceneObjects[i], &appProperties->camera, appProperties->depthBuffer, appProperties->matCap1);
	}

	SDL_UnlockTexture(appProperties->texture);

	SDL_RenderCopy(appProperties->renderer, appProperties->texture, NULL, NULL);
	SDL_RenderPresent(appProperties->renderer);

	//free(depthBuffer);

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

	SDL_FillRect(appProperties->surface, NULL, 0xffffffff);

	int sceneObjectsCounter = 0;

	// attempt to render the scene meshes ----------------------------------
	for (int i = 0; i < appProperties->sceneObjectForRealCount; i++) {
		struct Vec3f cumulativeRotation[500] = {{.x = 0, .y = 0, .z = 0}};
		struct Vec3f cumulativeTransform[500] = {{.x = 0, .y = 0, .z = 0}};
		struct SceneObject sceneObjectStack[500];
		recurseChildren(
			appProperties->delta * 2,
			appProperties->sceneObjects,
			&sceneObjectsCounter,
			appProperties->sceneObjectsForReal[i],
			appProperties->transformRotationStack,
			appProperties->transformPositionStack,
			sceneObjectStack,
			1
		);
	}

	SDL_LockSurface(appProperties->surface);

	char *pixels = appProperties->surface->pixels;

	float depthBuffer[640 * 480] = { 0 };
	for (int i = 0; i < sceneObjectsCounter; i++) {
		draw_scene(pixels, 640, 480, appProperties->sceneObjects[i], &appProperties->camera, depthBuffer, appProperties->matCap1);
	}

	SDL_UnlockSurface(appProperties->surface);

	// copy to window
	SDL_BlitSurface(appProperties->surface, NULL, appProperties->screen, NULL);
	SDL_UpdateWindowSurface(appProperties->window);

	// SDL_Delay(1000 / 12);
	float targetFrameTime = 1000 / 144;

	appProperties->frameTicks = SDL_GetTicks() - appProperties->startTicks;
	if (appProperties->frameTicks < targetFrameTime) {
		SDL_Delay(targetFrameTime - appProperties->frameTicks);
	}

	appProperties->delta++;
}

int main(int argc, char *argv[]) {
    char *funkyString = "Starting the funk! updating";
    printf("%s\n", funkyString);

    Uint32 startTicks, frameTicks;

	unsigned char* imageData = testMatcap;
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
		.finishTheFunk = 0,
		.matCap1 = imageData
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
		.position = {.x = 0.0, .y = 0.0, .z = -20.0},
		.rotation = {.x = 0.0f, .y = 0.0f, .z = 0.0f},
		.q_rotation = {.w = 0.0f, .x = 0.0f, .y = 0.0f, .z = 0.0f},
	};

	// generate the mesh before the loop
	//{
		struct mesh sphereMesh;
		int subdivisions = 80;
		generateSphere(0.3, subdivisions, &sphereMesh);
		sphereMesh.color = 0xff992255;
		trs(&sphereMesh,
		    0.0, -0.5, 0.0,
		    0, 0, 0,
		    1.0, 1.0, 1.0);
		struct SceneObject testSphere = (struct SceneObject) {
			.mesh = &sphereMesh,
			.color = 0xff992255,
			.transform = (struct Transform) {
				.position = (struct Vector3) { .x = 0.0f, .y = 0.0f, .z = 0.0f },
				.rotation = (struct Quaternion) { .w = 1.0, .x = 0.0f, .y = 0.0f, .z = 0.0f },
				.scale = (struct Vector3) { .x = 1.0f, .y = 1.0f, .z = 1.0f },
			},
			.attachPosition = (struct Vector3) { .x = 0.0f, .y = 0.0f, .z = 0.0f },
			.childCount = 0
		};
		appProperties.sceneObjectsForReal[appProperties.sceneObjectForRealCount++] = &testSphere;



		// test rendering a scene object
		struct mesh sceneCubeTest;
		generateCube(1.0, 1.0, 1.0, &sceneCubeTest);
		sceneCubeTest.color = 0x2299ff55;

		struct SceneObject testObject;
		testObject.mesh = &sceneCubeTest;
		testObject.color = 0x2299ff55;

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

		// test left arm mesh
		struct mesh leftArmMesh;
		leftArmMesh.color = 0x2299ff55;
		generateCube(1.0, 1.0, 1.0, &leftArmMesh);
		trs(&leftArmMesh,
		    0.5, 0.0, 0.0,
		    0, 0, 0,
		    1, 0.3, 0.3);
		struct SceneObject leftArmObject;
		leftArmObject.mesh = &leftArmMesh;
		leftArmObject.color = 0x2299ff55;
		leftArmObject.transform = (struct Transform) {
			.position = (struct Vector3) { .x = 0.0f, .y = 0.0f, .z = 0.0f },
			.rotation = (struct Quaternion) { .w = 1.0, .x = 0.0f, .y = 0.0f, .z = -20.0f },
			.scale = (struct Vector3) { .x = 1.0f, .y = 1.0f, .z = 1.0f },
		};
		leftArmObject.attachPosition = (struct Vector3) { .x = 0.0f, .y = 0.0f, .z = 0.0f };
		leftArmObject.childCount = 0;

		// add the child to the test mesh
		testObject.children[testObject.childCount] = &leftArmObject;
		testObject.childCount++;

		// left forearm arm
		struct mesh leftForeArmMesh;
		leftForeArmMesh.color = 0x2299ff55;
		generateCube(1.0, 1.0, 1.0, &leftForeArmMesh);
		trs(&leftForeArmMesh,
		    0.5, 0.0, 0.0,
		    0, 0, 0,
		    1, 0.3, 0.3);
		struct SceneObject leftForeArmObject = (struct SceneObject) {
			.mesh = &leftForeArmMesh,
			.color = 0x2299ff55,
			.transform = (struct Transform) {
				.position = (struct Vector3) { .x = 1.0f, .y = 0.0f, .z = 0.0f },
				.rotation = (struct Quaternion) { .w = 1.0, .x = 0.0f, .y = 0.0f, .z = 0.0f },
				.scale = (struct Vector3) { .x = 1.0f, .y = 1.0f, .z = 1.0f },
			},
			.attachPosition = (struct Vector3) { .x = 0.0f, .y = 0.0f, .z = 0.0f },
			.childCount = 0
		};
		leftArmObject.children[leftArmObject.childCount] = &leftForeArmObject;
		leftArmObject.childCount++;

		// right arm
		struct mesh rightArmMesh;
		rightArmMesh.color = 0x2299ff55;
		generateCube(1.0, 1.0, 1.0, &rightArmMesh);
		trs(&rightArmMesh,
		    -0.5, 0.0, 0.0,
		    0, 0, 0,
		    1, 0.3, 0.3);
		struct SceneObject rightArmObject = (struct SceneObject) {
			.mesh = &rightArmMesh,
			.color = 0x2299ff55,
			.transform = (struct Transform) {
				.position = (struct Vector3) { .x = 0.0f, .y = 0.0f, .z = 0.0f },
				.rotation = (struct Quaternion) { .w = 1.0, .x = 0.0f, .y = 0.0f, .z = 20.0f },
				.scale = (struct Vector3) { .x = 1.0f, .y = 1.0f, .z = 1.0f },
			},
			.attachPosition = (struct Vector3) { .x = 0.0f, .y = 0.0f, .z = 0.0f },
			.childCount = 0
		};
		testObject.children[testObject.childCount] = &rightArmObject;
		testObject.childCount++;

		// right forearm arm
		struct mesh rightForeArmMesh;
		rightForeArmMesh.color = 0x2299ff55;
		generateCube(1.0, 1.0, 1.0, &rightForeArmMesh);
		trs(&rightForeArmMesh,
		    -0.5, 0.0, 0.0,
		    0, 0, 0,
		    1, 0.3, 0.3);
		struct SceneObject rightForeArmObject = (struct SceneObject) {
			.mesh = &rightForeArmMesh,
			.color = 0x2299ff55,
			.transform = (struct Transform) {
				.position = (struct Vector3) { .x = -1.0f, .y = 0.0f, .z = 0.0f },
				.rotation = (struct Quaternion) { .w = 1.0, .x = 0.0f, .y = 0.0f, .z = 0.0f },
				.scale = (struct Vector3) { .x = 1.0f, .y = 1.0f, .z = 1.0f },
			},
			.attachPosition = (struct Vector3) { .x = 0.0f, .y = 0.0f, .z = 0.0f },
			.childCount = 0
		};
		rightArmObject.children[rightArmObject.childCount] = &rightForeArmObject;
		rightArmObject.childCount++;

		// right leg
		struct mesh rightLegMesh;
		rightLegMesh.color = 0x2299ff55;
		generateCube(1.0, 1.0, 1.0, &rightLegMesh);
		trs(&rightLegMesh,
		    0.0, 0.5, 0.0,
		    0, 0, 0,
		    0.3, 1.0, 0.3);
		struct SceneObject rightLegObject = (struct SceneObject) {
			.mesh = &rightLegMesh,
			.color = 0x2299ff55,
			.transform = (struct Transform) {
				.position = (struct Vector3) { .x = -0.3f, .y = 1.0f, .z = 0.0f },
				.rotation = (struct Quaternion) { .w = 1.0, .x = 0.0f, .y = 0.0f, .z = 0.0f },
				.scale = (struct Vector3) { .x = 1.0f, .y = 1.0f, .z = 1.0f },
			},
			.attachPosition = (struct Vector3) { .x = 0.0f, .y = 0.0f, .z = 0.0f },
			.childCount = 0
		};
		testObject.children[testObject.childCount] = &rightLegObject;
		testObject.childCount++;

		// left leg
		struct mesh leftLegMesh;
		leftLegMesh.color = 0x2299ff55;
		generateCube(1.0, 1.0, 1.0, &leftLegMesh);
		trs(&leftLegMesh,
		    0.0, 0.5, 0.0,
		    0, 0, 0,
		    0.3, 1.0, 0.3);
		struct SceneObject leftLegObject = (struct SceneObject) {
			.mesh = &leftLegMesh,
			.color = 0x2299ff55,
			.transform = (struct Transform) {
				.position = (struct Vector3) { .x = 0.3f, .y = 1.0f, .z = 0.0f },
				.rotation = (struct Quaternion) { .w = 1.0, .x = 0.0f, .y = 0.0f, .z = 0.0f },
				.scale = (struct Vector3) { .x = 1.0f, .y = 1.0f, .z = 1.0f },
			},
			.attachPosition = (struct Vector3) { .x = 0.0f, .y = 0.0f, .z = 0.0f },
			.childCount = 0
		};
		testObject.children[testObject.childCount] = &leftLegObject;
		testObject.childCount++;
    // fin!------------------------------------------------------------

    // clone the sceneObject stuff
    for (int i = 0; i < appProperties.sceneObjectForRealCount; i++) {
		recurseChildrenSetup(
		    appProperties.sceneObjects,
			&appProperties.sceneObjectsCounter2,
		    appProperties.sceneObjectsForReal[i]
		);
    }

	appProperties.depthBuffer = (float *)malloc(640 * 480 * sizeof(float));

	// use a different loop for emscripten
#ifdef __EMSCRIPTEN__
	emscripten_set_main_loop_arg(emscriptenLoop, &appProperties, 144, 1);
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