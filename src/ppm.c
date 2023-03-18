#include "ppm.h"

void save_ppm(const char * filename, Canvas * canvas){
	assert(filename != NULL && canvas != NULL && canvas->pixels);

	size_t w = canvas->w;
	size_t h = canvas->h;

	FILE * f;
	if((f = fopen(filename, "wb")) == NULL){
		fprintf(stderr, "Error: %s\n", strerror(errno));
		fclose(f);
		return;
	}

	if(fprintf(f, "%s\n %zu %zu\n%d\n", PPM_MAGIC, w, h, 0xff) < 0){
		fprintf(stderr, "Error: Failed writing to file %s\n", filename);
		fclose(f);
		return;
	}

	for(size_t y = 0; y < h; y++){
		for(size_t x = 0; x < w; x++){
			uint32_t pixel = canvas->pixels[y * w + x];
			uint8_t color[3];
			color[0] = (pixel >> 24) & 0xff;
			color[1] = (pixel >> 16) & 0xff;
			color[2] = (pixel >> 8) & 0xff;

			fwrite(color, 3, 1, f);
			if(ferror(f)){
				fprintf(stderr, "Error: Failed writing to file %s\n", filename);
				fclose(f);
				return;
			}
		}
		
	}
}
