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