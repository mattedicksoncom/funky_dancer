struct mesh {
	float *vert;
	int vertCount;
	int *face;
	int faceCount;
	uint32_t color;
};

struct Vec3f {
	float x;
	float y;
	float z;
};

struct Vec2i {
	int x, y;
};

void swap(int *xp, int *yp) {
	int temp = *xp;
	*xp = *yp;
	*yp = temp;
}

struct Vector3 {
	float x;
	float y;
	float z;
};

struct Quaternion {
	float w;
	float x;
	float y;
	float z;
};

struct Transform {
	struct Vector3 position;
	struct Quaternion rotation;
	struct Vector3 scale;
};

struct SceneObject {
	struct mesh *mesh;
	uint32_t color;
	struct Transform transform;
	struct Vector3 attachPosition;
	struct SceneObject *children[10]; // max 10 for now
	int childCount;
};

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