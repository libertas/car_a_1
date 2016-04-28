#include <stdlib.h>

#include "image.h"
#include "utils.h"


int image_bfs(bfs_t **b, uint8_t **img, uint16_t height, uint16_t width)
{
	return 0;
}

int image_filter(uint8_t **img, uint16_t height, uint16_t width)
{
	bfs_t **b = malloc(height * width);
	
	for(uint16_t i = 0; i < height; i++) {
		for(uint16_t j = 0; j < width; j++) {
			b[i][j].color = 0;
			b[i][j].d = -1;
			b[i][j].pi = -2;
		}
	}
	
	image_bfs(b, img, height, width);
	
	return 0;
}
