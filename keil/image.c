#include "camera.h"
#include "image.h"
#include "utils.h"

uint16_t bfs_queue_buf[IMAGE_DEFAULT_HEIGHT * IMAGE_DEFAULT_WIDTH];
bfs_t bfs_buf[IMAGE_DEFAULT_HEIGHT * IMAGE_DEFAULT_WIDTH];

int image_bfs_step(uint16_t u, uint16_t v, bfs_t **b)
{
	uint16_t i, j;
	i = v >> 8;
	j = v & 0xff;
	if(b[i][j].color == IMAGE_WHITE) {
		b[i][j].color = IMAGE_GRAY;
		b[i][j].d = b[u >> 8][u & 0xff].d + 1;
		b[i][j].pi = u;
	}
	
	return 0;
}

int image_bfs(uint8_t **img, uint16_t height, uint16_t width)
{
	queue q;
	uint16_t i, j;
	uint16_t s, u;
	
	for(i = 0; i < height; i++) {
		for(j = 0; j < width; j++) {
			init_queue(&q, bfs_queue_buf, IMAGE_DEFAULT_HEIGHT * IMAGE_DEFAULT_WIDTH);
			
			bfs_t **b = (bfs_t**)bfs_buf;

			for(uint16_t iii = 0; iii < height; iii++) {
				for(uint16_t jjj = 0; jjj < width; jjj++) {
					b[iii][jjj].color = IMAGE_WHITE;
					b[iii][jjj].d = IMAGE_INF;
					b[iii][jjj].pi = IMAGE_NIL;
				}
			}
			
			b[i][j].color = IMAGE_GRAY;
			b[i][j].d = 0;
			b[i][j].pi = IMAGE_NIL;
			
			s = 0;
			s |= (i << 8) | j;
			in_queue(&q, s);
			
			while(0 != q.count) {
				out_queue(&q, &u);
				if((u >> 8) + 1 < IMAGE_DEFAULT_HEIGHT - 1) {
					image_bfs_step(u, u + 0x0100, b);
				}
				if((u >> 8) - 1 > 0) {
					image_bfs_step(u, u - 0x0100, b);
				}
				if((u & 0xff) + 1 < IMAGE_DEFAULT_WIDTH - 1) {
					image_bfs_step(u, u + 0x0001, b);
				}
				if((u & 0xff + 1) > 0) {
					image_bfs_step(u, u - 0x0001, b);
				}
			}
		}
	}
	
	
	
	return 0;
}

int image_filter(uint8_t **img, uint16_t height, uint16_t width)
{
	image_bfs(img, height, width);

	return 0;
}
