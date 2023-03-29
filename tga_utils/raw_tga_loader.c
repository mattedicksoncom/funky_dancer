struct TGAHeader {
	unsigned char idLength;
	unsigned char colorMapType;
	unsigned char dataTypeCode;
	unsigned short colorMapOrigin;
	unsigned short colorMapLength;
	unsigned char colorMapDepth;
	unsigned short xOrigin;
	unsigned short yOrigin;
	unsigned short width;
	unsigned short height;
	unsigned char bitsPerPixel;
	unsigned char imageDescriptor;
};

void read_tga_header(FILE* file, struct TGAHeader* header) {
	fread(&header->idLength, 1, 1, file);
	fread(&header->colorMapType, 1, 1, file);
	fread(&header->dataTypeCode, 1, 1, file);

	unsigned short tempShort;

	fread(&tempShort, 2, 1, file);
	header->colorMapOrigin = tempShort;

	fread(&tempShort, 2, 1, file);
	header->colorMapLength = tempShort;

	fread(&header->colorMapDepth, 1, 1, file);

	fread(&tempShort, 2, 1, file);
	header->xOrigin = tempShort;

	fread(&tempShort, 2, 1, file);
	header->yOrigin = tempShort;

	fread(&tempShort, 2, 1, file);
	header->width = tempShort;

	fread(&tempShort, 2, 1, file);
	header->height = tempShort;

	fread(&header->bitsPerPixel, 1, 1, file);
	fread(&header->imageDescriptor, 1, 1, file);
}

unsigned char* load_tga(const char* filename, struct TGAHeader* header) {
	FILE* file = fopen(filename, "rb");
	if (file == NULL) {
		printf("Error: Unable to open the TGA file.\n");
		return NULL;
	}

	// read the header
	//fread(header, sizeof(struct TGAHeader), 1, file);
	read_tga_header(file, header);

	// check it's uncompressed, otherwise too complex for me
	if (header->dataTypeCode != 2 || (header->bitsPerPixel != 24 && header->bitsPerPixel != 32)) {
		//printf("Error: dataTypeCode === %i\n", header->dataTypeCode);
		//printf("Error: bitsPerPixel === %i\n", header->bitsPerPixel);
		printf("Error: Only uncompressed RGB or RGBA TGA files are supported.\n");
		printf("TGA Header:\n");
		printf("  ID Length: %d\n", header->idLength);
		printf("  Color Map Type: %d\n", header->colorMapType);
		printf("  Data Type Code: %d\n", header->dataTypeCode);
		printf("  Color Map Origin: %d\n", header->colorMapOrigin);
		printf("  Color Map Length: %d\n", header->colorMapLength);
		printf("  Color Map Depth: %d\n", header->colorMapDepth);
		printf("  X Origin: %d\n", header->xOrigin);
		printf("  Y Origin: %d\n", header->yOrigin);
		printf("  Width: %d\n", header->width);
		printf("  Height: %d\n", header->height);
		printf("  Bits Per Pixel: %d\n", header->bitsPerPixel);
		printf("  Image Descriptor: %d\n", header->imageDescriptor);
		fclose(file);
		return NULL;
	}

	// header malloc
	size_t imageSize = header->width * header->height * header->bitsPerPixel / 8;
	unsigned char* imageData = (unsigned char*)malloc(imageSize);

	////read the data
	fread(imageData, 1, imageSize, file);
	fclose(file);

	// swap the R and B, tga creators are madmen
	for (size_t i = 0; i < imageSize; i += header->bitsPerPixel / 8) {
		unsigned char temp = imageData[i];
		imageData[i] = imageData[i + 2];
		imageData[i + 2] = temp;
	}

	return imageData;
}