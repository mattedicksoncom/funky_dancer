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
	struct Transform transform;
	struct Vector3 attachPosition;
	struct SceneObject *children[10]; // max 10 for now
	int childCount;
};