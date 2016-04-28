#include <stdint.h>

struct bfs_struct {
	/*
		0 for white
		1 for gray
		2 for black
	*/
	uint8_t color;
	
	/*
		-1 for INF
		-2 for NIL
	*/
	int32_t d;
	int32_t pi;
};

typedef struct bfs_struct bfs_t;

int image_bfs(bfs_t **b, uint8_t **img, uint16_t height, uint16_t width);
int image_filter(uint8_t **img, uint16_t height, uint16_t width);
