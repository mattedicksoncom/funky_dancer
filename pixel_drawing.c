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
			if (x >= 0 && x < 480 && y >= 0 && y < 640) {
				unsigned int *row = (unsigned int *)(pixels + pitch * x);
				row[y] = color;
			}
		} else {
			if (x >= 0 && x < 640 && y >= 0 && y < 480) {
				unsigned int *row = (unsigned int *)(pixels + pitch * y);
				row[x] = color;
			}
		}
		error2 += derror2;
		if (error2 > dx) {
			y += (y1 > y0 ? 1 : -1);
			error2 -= dx * 2;
		}
	}
}

void drawTriangle(struct Vec2i *pts, char *pixels, unsigned int color) {
	int imageWidth = 640 - 1;
	int imageHeight = 480 - 1;
	int pitch = sizeof(char) * 640 * 4;

	struct Vec2i boundingBoxMin = {imageWidth, imageHeight};
	struct Vec2i boundingBoxMax = {0, 0};
	struct Vec2i clamp = {imageWidth, imageHeight};

	for (int i = 0; i < 3; i++) {
		boundingBoxMin.x = fmax(0, fmin(boundingBoxMin.x, pts[i].x));
		boundingBoxMin.y = fmax(0, fmin(boundingBoxMin.y, pts[i].y));

		boundingBoxMax.x = fmin(clamp.x, fmax(boundingBoxMax.x, pts[i].x));
		boundingBoxMax.y = fmin(clamp.y, fmax(boundingBoxMax.y, pts[i].y));
	}

	struct Vec2i pixelCoord;
	for (pixelCoord.x = boundingBoxMin.x; pixelCoord.x <= boundingBoxMax.x; pixelCoord.x++) {
		for (pixelCoord.y = boundingBoxMin.y; pixelCoord.y <= boundingBoxMax.y; pixelCoord.y++) {
			struct Vec3f bc_screen = barycentric(pts, pixelCoord);
			if (bc_screen.x < 0 || bc_screen.y < 0 || bc_screen.z < 0) {
				continue;
			}
			unsigned int *row = (unsigned int *)(pixels + pitch * pixelCoord.y);
			row[pixelCoord.x] = color;
		}
	}
}