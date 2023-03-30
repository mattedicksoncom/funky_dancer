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
//shouldn't use an enum in this file but live fast, live long
enum MatCap {
	metalOrange = 0,
	metalGreen = 1,
	metalBlue = 2
};

struct Mesh {
	float *vert;
	int vertCount;
	int *face;
	int faceCount;
	uint32_t color;
	enum MatCap matcap;
};

// contains information for controls
// whether local or remote
// camera views used (store cameras seperately though)
// screen to show camera views
struct PlayerView {
	//something
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
	struct Mesh *sceneObjects[500];
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
	unsigned char* colorBuffer;
	unsigned char* matCap1;
	unsigned char* matCaps[3];
};