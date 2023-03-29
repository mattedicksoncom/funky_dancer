void setupTestMesh(struct AppProperties *appProperties) {
	struct Mesh *sphereMesh = malloc(sizeof(struct Mesh));
	int subdivisions = 80;
	generateSphere(0.3, subdivisions, sphereMesh);
	sphereMesh->color = 0xff992255;
	trs(sphereMesh,
	    0.0, -0.5, 0.0,
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

	// test rendering a scene object
	struct Mesh *sceneCubeTest = malloc(sizeof(struct Mesh));
	generateCube(1.0, 1.0, 1.0, sceneCubeTest);
	sceneCubeTest->color = 0x2299ff55;

	struct SceneObject *testObject = malloc(sizeof(struct SceneObject));
	testObject->mesh = sceneCubeTest;
	testObject->color = 0x2299ff55;

	trs(sceneCubeTest,
	    0.0, 0.5, 0.0,
	    0, 0, 0,
	    0.3, 1.0, 0.3);

	testObject->transform = (struct Transform) {
		.position = (struct Vector3) { .x = 0.0f, .y = 0.0f, .z = 0.0f },
		.rotation = (struct Quaternion) { .w = 1.0, .x = 0.0f, .y = 0.0f, .z = 0.0f },
		.scale = (struct Vector3) { .x = 1.0f, .y = 1.0f, .z = 1.0f },
	};
	testObject->attachPosition = (struct Vector3) { .x = 0.0f, .y = 0.0f, .z = 0.0f };
	testObject->childCount = 0;

	appProperties->sceneObjectsForReal[appProperties->sceneObjectForRealCount++] = testObject;

	//// right arm
	struct Mesh *rightArmMesh = malloc(sizeof(struct Mesh));
	rightArmMesh->color = 0x2299ff55;
	generateCube(1.0, 1.0, 1.0, rightArmMesh);
	trs(rightArmMesh,
	    -0.5, 0.0, 0.0,
	    0, 0, 0,
	    1, 0.3, 0.3);
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
	generateCube(1.0, 1.0, 1.0, leftArmMesh);
	trs(leftArmMesh,
	    0.5, 0.0, 0.0,
	    0, 0, 0,
	    1, 0.3, 0.3);
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
//
	
	//struct mesh *rightForeArmMesh = malloc(sizeof(struct mesh));
	//rightForeArmMesh->color = 0x2299ff55;
	//generateCube(1.0, 1.0, 1.0, rightForeArmMesh);
	//trs(rightForeArmMesh,
	    //-0.5, 0.0, 0.0,
	    //0, 0, 0,
	    //1, 0.3, 0.3);
	//struct SceneObject *rightForeArmObject = malloc(sizeof(struct SceneObject));
	//rightForeArmObject->mesh = rightForeArmMesh;
	//rightForeArmObject->color = 0x2299ff55;
	//rightForeArmObject->transform = (struct Transform) {
		//.position = (struct Vector3) { .x = -1.0f, .y = 0.0f, .z = 0.0f },
		//.rotation = (struct Quaternion) { .w = 1.0, .x = 0.0f, .y = 0.0f, .z = 0.0f },
		//.scale = (struct Vector3) { .x = 1.0f, .y = 1.0f, .z = 1.0f },
	//};
	//rightForeArmObject->attachPosition = (struct Vector3) { .x = 0.0f, .y = 0.0f, .z = 0.0f };
	//rightForeArmObject->childCount = 0;
	//rightArmObject->children[rightArmObject->childCount] = rightForeArmObject;
	//rightArmObject->childCount++;
	//struct SceneObject rightForeArmObject = (struct SceneObject) {
		//.mesh = &rightForeArmMesh,
		//.color = 0x2299ff55,
		//.transform = (struct Transform) {
			//.position = (struct Vector3) { .x = -1.0f, .y = 0.0f, .z = 0.0f },
			//.rotation = (struct Quaternion) { .w = 1.0, .x = 0.0f, .y = 0.0f, .z = 0.0f },
			//.scale = (struct Vector3) { .x = 1.0f, .y = 1.0f, .z = 1.0f },
		//},
		//.attachPosition = (struct Vector3) { .x = 0.0f, .y = 0.0f, .z = 0.0f },
		//.childCount = 0
	//};
	//rightArmObject.children[rightArmObject.childCount] = &rightForeArmObject;
	//rightArmObject.childCount++;
//
	//// right leg
	struct Mesh *rightLegMesh = malloc(sizeof(struct Mesh));
	rightLegMesh->color = 0x2299ff55;
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