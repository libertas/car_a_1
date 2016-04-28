#include <stdint.h>

#define IMAGE_WHITE (0)
#define IMAGE_GRAY (1)
#define IMAGE_BLACK (2)
#define IMAGE_INF (-1)
#define IMAGE_NIL (-2)

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

int image_bfs(uint8_t **img, uint16_t height, uint16_t width);
int image_filter(uint8_t **img, uint16_t height, uint16_t width);
