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
	image_bfs(b, img, height, width);
	
	return 0;
}
