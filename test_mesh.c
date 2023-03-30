void setupTestMesh(struct AppProperties *appProperties) {
	struct Mesh *sphereMesh = malloc(sizeof(struct Mesh));
	int subdivisions = 10;
	generateSphere(0.3, subdivisions, sphereMesh);
	sphereMesh->color = 0xff992255;
	sphereMesh->matcap = metalOrange;
	trs(sphereMesh,
	    1.0, -0.5, 0.0,
	    0, 0, 0,
	    1.0, 1.0, 1.0);

	struct SceneObject *testSphere = malloc(sizeof(struct SceneObject));
	testSphere->mesh = sphereMesh;
	testSphere->color = 0xff992255;
	testSphere->transform = (struct Transform) {
		.position = (struct Vector3) { .x = 0.0f, .y = 0.0f, .z = 0.0f },
		.rotation = (struct Quaternion) { .w = 1.0, .x = 0.0f, .y = 0.0f, .z = 0.0f },
		.scale = (struct Vector3) { .x = 1.0f, .y = 1.0f, .z = 1.0f },
	};
	testSphere->attachPosition = (struct Vector3) { .x = 0.0f, .y = 0.0f, .z = 0.0f };
	testSphere->childCount = 0;

	appProperties->sceneObjectsForReal[appProperties->sceneObjectForRealCount++] = testSphere;

	struct Mesh *sphereMesh2 = malloc(sizeof(struct Mesh));
	generateSphere(0.3, subdivisions, sphereMesh2);
	sphereMesh2->color = 0xff992255;
	sphereMesh2->matcap = metalGreen;
	trs(sphereMesh2,
	    1.0, -0.48, 0.05,
	    0, 0, 0,
	    1.0, 1.0, 1.0);

	struct SceneObject *testSphere2 = malloc(sizeof(struct SceneObject));
	testSphere2->mesh = sphereMesh2;
	testSphere2->color = 0xff992255;
	testSphere2->transform = (struct Transform) {
		.position = (struct Vector3) { .x = 0.0f, .y = 0.0f, .z = 0.0f },
		.rotation = (struct Quaternion) { .w = 1.0, .x = 0.0f, .y = 0.0f, .z = 0.0f },
		.scale = (struct Vector3) { .x = 1.0f, .y = 1.0f, .z = 1.0f },
	};
	testSphere2->attachPosition = (struct Vector3) { .x = 0.0f, .y = 0.0f, .z = 0.0f };
	testSphere2->childCount = 0;

	appProperties->sceneObjectsForReal[appProperties->sceneObjectForRealCount++] = testSphere2;

	struct Mesh *zdirMesh = malloc(sizeof(struct Mesh));
	generateSphere(0.3, 30, zdirMesh);
	zdirMesh->color = 0xff992255;
	zdirMesh->matcap = metalBlue;
	trs(zdirMesh,
	    0.0, 0, 0.5,
	    0, 0, 0,
	    0.2, 0.2, 1.0);

	struct SceneObject *zdirObject = malloc(sizeof(struct SceneObject));
	zdirObject->mesh = zdirMesh;
	zdirObject->color = 0xff992255;
	zdirObject->transform = (struct Transform) {
		.position = (struct Vector3) { .x = 0.0f, .y = 0.0f, .z = 0.0f },
		.rotation = (struct Quaternion) { .w = 1.0, .x = 0.0f, .y = 0.0f, .z = 0.0f },
		.scale = (struct Vector3) { .x = 1.0f, .y = 1.0f, .z = 1.0f },
	};
	zdirObject->attachPosition = (struct Vector3) { .x = 0.0f, .y = 0.0f, .z = 0.0f };
	zdirObject->childCount = 0;

	appProperties->sceneObjectsForReal[appProperties->sceneObjectForRealCount++] = zdirObject;

	struct Mesh *xdirMesh = malloc(sizeof(struct Mesh));
	generateSphere(0.3, 30, xdirMesh);
	xdirMesh->color = 0xff992255;
	xdirMesh->matcap = metalOrange;
	trs(xdirMesh,
	    0.5, 0, 0.0,
	    0, 0, 0,
	    1.0, 0.2, 0.2);

	struct SceneObject *xdirObject = malloc(sizeof(struct SceneObject));
	xdirObject->mesh = xdirMesh;
	xdirObject->color = 0xff992255;
	xdirObject->transform = (struct Transform) {
		.position = (struct Vector3) { .x = 0.0f, .y = 0.0f, .z = 0.0f },
		.rotation = (struct Quaternion) { .w = 1.0, .x = 0.0f, .y = 0.0f, .z = 0.0f },
		.scale = (struct Vector3) { .x = 1.0f, .y = 1.0f, .z = 1.0f },
	};
	xdirObject->attachPosition = (struct Vector3) { .x = 0.0f, .y = 0.0f, .z = 0.0f };
	xdirObject->childCount = 0;

	appProperties->sceneObjectsForReal[appProperties->sceneObjectForRealCount++] = xdirObject;

	struct Mesh *ydirMesh = malloc(sizeof(struct Mesh));
	generateSphere(0.3, 30, ydirMesh);
	ydirMesh->color = 0xff992255;
	ydirMesh->matcap = metalGreen;
	trs(ydirMesh,
	    0.0, 0.5, 0.0,
	    0, 0, 0,
	    0.2, 1.0, 0.2);

	struct SceneObject *ydirObject = malloc(sizeof(struct SceneObject));
	ydirObject->mesh = ydirMesh;
	ydirObject->color = 0xff992255;
	ydirObject->transform = (struct Transform) {
		.position = (struct Vector3) { .x = 0.0f, .y = 0.0f, .z = 0.0f },
		.rotation = (struct Quaternion) { .w = 1.0, .x = 0.0f, .y = 0.0f, .z = 0.0f },
		.scale = (struct Vector3) { .x = 1.0f, .y = 1.0f, .z = 1.0f },
	};
	ydirObject->attachPosition = (struct Vector3) { .x = 0.0f, .y = 0.0f, .z = 0.0f };
	ydirObject->childCount = 0;

	appProperties->sceneObjectsForReal[appProperties->sceneObjectForRealCount++] = ydirObject;

	// test rendering a scene object
	struct Mesh *sceneCubeTest = malloc(sizeof(struct Mesh));
	generateCube(1.0, 1.0, 1.0, sceneCubeTest);
	sceneCubeTest->color = 0x2299ff55;
	sceneCubeTest->matcap = metalGreen;

	struct SceneObject *testObject = malloc(sizeof(struct SceneObject));
	testObject->mesh = sceneCubeTest;
	testObject->color = 0x2299ff55;

	trs(sceneCubeTest,
	    0.0, 0.5, 0.0,
	    0, 0, 0,
	    0.1, 1.0, 0.1);

	testObject->transform = (struct Transform) {
		.position = (struct Vector3) { .x = 1.0f, .y = 0.0f, .z = 0.0f },
		.rotation = (struct Quaternion) { .w = 1.0, .x = 0.0f, .y = 0.0f, .z = 0.0f },
		.scale = (struct Vector3) { .x = 1.0f, .y = 1.0f, .z = 1.0f },
	};
	testObject->attachPosition = (struct Vector3) { .x = 0.0f, .y = 0.0f, .z = 0.0f };
	testObject->childCount = 0;

	appProperties->sceneObjectsForReal[appProperties->sceneObjectForRealCount++] = testObject;

	//// right arm
	struct Mesh *rightArmMesh = malloc(sizeof(struct Mesh));
	rightArmMesh->color = 0x2299ff55;
	rightArmMesh->matcap = metalGreen;
	generateCube(1.0, 1.0, 1.0, rightArmMesh);
	trs(rightArmMesh,
	    -0.5, 0.0, 0.0,
	    0, 0, 0,
	    1, 0.1, 0.1);
	struct SceneObject *rightArmObject = malloc(sizeof(struct SceneObject));
	rightArmObject->mesh = rightArmMesh;
	rightArmObject->color = 0x2299ff55;
	rightArmObject->transform = (struct Transform) {
		.position = (struct Vector3) { .x = 0.0f, .y = 0.0f, .z = 0.0f },
		.rotation = (struct Quaternion) { .w = 1.0, .x = 0.0f, .y = 0.0f, .z = -20.0f },
		.scale = (struct Vector3) { .x = 1.0f, .y = 1.0f, .z = 1.0f },
	};
	rightArmObject->attachPosition = (struct Vector3) { .x = 0.0f, .y = 0.0f, .z = 0.0f };
	rightArmObject->childCount = 0;

	// add the child to the test mesh
	testObject->children[testObject->childCount] = rightArmObject;
	testObject->childCount++;

	//// right forearm arm
	struct Mesh *rightForeArmMesh = malloc(sizeof(struct Mesh));
	rightForeArmMesh->color = 0x2299ff55;
	rightForeArmMesh->matcap = metalGreen;
	generateCube(1.0, 1.0, 1.0, rightForeArmMesh);
	trs(rightForeArmMesh,
	    -0.5, 0.0, 0.0,
	    0, 0, 0,
	    1, 0.3, 0.3);
	struct SceneObject *rightForeArmObject = malloc(sizeof(struct SceneObject));
	rightForeArmObject->mesh = rightForeArmMesh;
	rightForeArmObject->color = 0x2299ff55;
	rightForeArmObject->transform = (struct Transform) {
		.position = (struct Vector3) { .x = -1.0f, .y = 0.0f, .z = 0.0f },
		.rotation = (struct Quaternion) { .w = 1.0, .x = 0.0f, .y = 0.0f, .z = 0.0f },
		.scale = (struct Vector3) { .x = 1.0f, .y = 1.0f, .z = 1.0f },
	};
	rightForeArmObject->attachPosition = (struct Vector3) { .x = 0.0f, .y = 0.0f, .z = 0.0f };
	rightForeArmObject->childCount = 0;

	rightArmObject->children[rightArmObject->childCount] = rightForeArmObject;
	rightArmObject->childCount++;

	printf("%i", rightArmObject->childCount);
//
	//// test left arm mesh
	struct Mesh *leftArmMesh = malloc(sizeof(struct Mesh));
	leftArmMesh->color = 0x2299ff55;
	leftArmMesh->matcap = metalGreen;
	generateCube(1.0, 1.0, 1.0, leftArmMesh);
	trs(leftArmMesh,
	    0.5, 0.0, 0.0,
	    0, 0, 0,
	    1, 0.1, 0.1);
	struct SceneObject *leftArmObject = malloc(sizeof(struct SceneObject));
	leftArmObject->mesh = leftArmMesh;
	leftArmObject->color = 0x2299ff55;
	leftArmObject->transform = (struct Transform) {
		.position = (struct Vector3) { .x = 0.0f, .y = 0.0f, .z = 0.0f },
		.rotation = (struct Quaternion) { .w = 1.0, .x = 0.0f, .y = 0.0f, .z = -20.0f },
		.scale = (struct Vector3) { .x = 1.0f, .y = 1.0f, .z = 1.0f },
	};
	leftArmObject->attachPosition = (struct Vector3) { .x = 0.0f, .y = 0.0f, .z = 0.0f };
	leftArmObject->childCount = 0;

	// add the child to the test mesh
	testObject->children[testObject->childCount] = leftArmObject;
	testObject->childCount++;
//
	//// left forearm arm
	struct Mesh *leftForeArmMesh = malloc(sizeof(struct Mesh));
	leftForeArmMesh->color = 0x2299ff55;
	leftForeArmMesh->matcap = metalGreen;
	generateCube(1.0, 1.0, 1.0, leftForeArmMesh);
	trs(leftForeArmMesh,
	    0.5, 0.0, 0.0,
	    0, 0, 0,
	    1, 0.3, 0.3);
	struct SceneObject *leftForeArmObject = malloc(sizeof(struct SceneObject));
	leftForeArmObject->mesh = leftForeArmMesh;
	leftForeArmObject->color = 0x2299ff55;
	leftForeArmObject->transform = (struct Transform) {
		.position = (struct Vector3) { .x = 1.0f, .y = 0.0f, .z = 0.0f },
		.rotation = (struct Quaternion) { .w = 1.0, .x = 0.0f, .y = 0.0f, .z = 0.0f },
		.scale = (struct Vector3) { .x = 1.0f, .y = 1.0f, .z = 1.0f },
	};
	leftForeArmObject->attachPosition = (struct Vector3) { .x = 0.0f, .y = 0.0f, .z = 0.0f };
	leftForeArmObject->childCount = 0;
	leftArmObject->children[leftArmObject->childCount] = leftForeArmObject;
	leftArmObject->childCount++;


	struct Mesh *leftForeArmMesh2 = malloc(sizeof(struct Mesh));
	leftForeArmMesh2->color = 0x2299ff55;
	leftForeArmMesh2->matcap = metalGreen;
	generateCube(1.0, 1.0, 1.0, leftForeArmMesh2);
	trs(leftForeArmMesh2,
	    0.5, 0.0, 0.0,
	    0, 0, 0,
	    1, 0.3, 0.3);
	struct SceneObject *leftForeArmObject2 = malloc(sizeof(struct SceneObject));
	leftForeArmObject2->mesh = leftForeArmMesh2;
	leftForeArmObject2->color = 0x2299ff55;
	leftForeArmObject2->transform = (struct Transform) {
		.position = (struct Vector3) { .x = 1.0f, .y = 0.0f, .z = 0.0f },
		.rotation = (struct Quaternion) { .w = 1.0, .x = 0.0f, .y = 0.0f, .z = 0.0f },
		.scale = (struct Vector3) { .x = 1.0f, .y = 1.0f, .z = 1.0f },
	};
	leftForeArmObject2->attachPosition = (struct Vector3) { .x = 0.0f, .y = 0.0f, .z = 0.0f };
	leftForeArmObject2->childCount = 0;
	leftForeArmObject->children[leftForeArmObject->childCount] = leftForeArmObject2;
	leftForeArmObject->childCount++;

	//// right leg
	struct Mesh *rightLegMesh = malloc(sizeof(struct Mesh));
	rightLegMesh->color = 0x2299ff55;
	rightLegMesh->matcap = metalGreen;
	generateCube(1.0, 1.0, 1.0, rightLegMesh);
	trs(rightLegMesh,
	    0.0, 0.5, 0.0,
	    0, 0, 0,
	    0.3, 1.0, 0.3);
	struct SceneObject *rightLegObject = malloc(sizeof(struct SceneObject));
	rightLegObject->mesh = rightLegMesh;
	rightLegObject->color = 0x2299ff55;
	rightLegObject->transform = (struct Transform) {
		.position = (struct Vector3) { .x = -0.3f, .y = 1.0f, .z = 0.0f },
		.rotation = (struct Quaternion) { .w = 1.0, .x = 0.0f, .y = 0.0f, .z = 0.0f },
		.scale = (struct Vector3) { .x = 1.0f, .y = 1.0f, .z = 1.0f },
	};
	rightLegObject->attachPosition = (struct Vector3) { .x = 0.0f, .y = 0.0f, .z = 0.0f };
	rightLegObject->childCount = 0;
	testObject->children[testObject->childCount] = rightLegObject;
	testObject->childCount++;

//
	//// left leg
	struct Mesh *leftLegMesh = malloc(sizeof(struct Mesh));
	leftLegMesh->color = 0x2299ff55;
	leftLegMesh->matcap = metalGreen;
	generateCube(1.0, 1.0, 1.0, leftLegMesh);
	trs(leftLegMesh,
	    0.0, 0.5, 0.0,
	    0, 0, 0,
	    0.3, 1.0, 0.3);
	struct SceneObject *leftLegObject = malloc(sizeof(struct SceneObject));
	leftLegObject->mesh = leftLegMesh;
	leftLegObject->color = 0x2299ff55;
	leftLegObject->transform = (struct Transform) {
		.position = (struct Vector3) { .x = 0.3f, .y = 1.0f, .z = 0.0f },
		.rotation = (struct Quaternion) { .w = 1.0, .x = 0.0f, .y = 0.0f, .z = 0.0f },
		.scale = (struct Vector3) { .x = 1.0f, .y = 1.0f, .z = 1.0f },
	};
	leftLegObject->attachPosition = (struct Vector3) { .x = 0.0f, .y = 0.0f, .z = 0.0f };
	leftLegObject->childCount = 0;
	testObject->children[testObject->childCount] = leftLegObject;
	testObject->childCount++;
}