#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>

#include "raw_tga_loader.c"

int main(int argc, char *argv[]) {
	if (argc < 2) {
		printf("Usage: %s <yournamehere>\n", argv[0]);
		return 1;
	}

	char fileName[256];
	snprintf(fileName, sizeof(fileName), "./matcaps/%s.tga", argv[1]);
	const char* filename = fileName;

	// load image
	struct TGAHeader header;
	unsigned char* imageData = load_tga(filename, &header);

	// Save the file
	char matcap_output[256];
	//FILE* file = fopen("./matcaps/matcap_test.c", "w");
	snprintf(matcap_output, sizeof(matcap_output), "./matcaps/%s.c", argv[1]);
	FILE* file = fopen(matcap_output, "w");

	if (file == NULL) {
		printf("Error: Unable to open the output file.\n");
		return 1;
	}

	size_t arraySize = header.width * header.height * (header.bitsPerPixel / 8);
	//fprintf(file, "unsigned char* testMatcap = {");
	fprintf(file, "unsigned char %sMatcap[] = {", argv[1]);
	for (size_t i = 0; i < arraySize; ++i) {
		fprintf(file, "%d", imageData[i]);
		if (i < arraySize - 1) {
			fprintf(file, ",");
		}
	}
	fprintf(file, "};\n");

	fclose(file);

	return 0;
}