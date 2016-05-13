#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "camera.h"
#include "interpreter.h"
#include "math.h"
#include "tftlcd.h"


char wl_buf[CMD_BUF_LEN] = {0};
char_queue wl_queue;

/*
format:
	(4-bit) [data-length] + (4-bit) [command] + (n-byte) [data] + (byte) [checksum]
	NOTE: sometimes data bytes can be used as command bits, too.
	NOTE: command 0x00 is not usable.
command list:
*/

uint8_t i_threshold = 0;

int run_cmd(char_queue *cmd_queue)
{
	char cmd;
	out_char_queue(cmd_queue, &cmd);

	uint8_t buf = 0;


	switch(cmd) {
		default:

			#ifdef DEBUG_INTPRT
			printf("\nUnknown command:%x\n", cmd);
			#endif

			break;
		
		case 0x10:
			out_char_queue(cmd_queue, (char*)&buf);
			if(buf != i_threshold) {
				i_threshold = buf;
				camera_reg_write(0x9C, i_threshold);
			}
			break;
	}
	
	return 0;
}

int check_cmd_1(char_queue *cmd_queue)
{
	uint32_t fr;
	uint8_t data_len, cmd;
	uint8_t check_sum = 0;
	uint16_t i;
	
	if(cmd_queue->count > 0) {
		fr = (cmd_queue->front + 1) % cmd_queue->max_size;
		cmd = cmd_queue->data[fr];
		data_len = (cmd >> 4);
		cmd &= 0xff;
		
		#ifdef DEBUG_INTPRT
		printf("\ndata-len:%x\ncmd:0x%x\n", data_len, cmd);
		#endif
		
		if(cmd_queue->count >= data_len + 2) {
			for(i = 0; i <= data_len; i++) {
				check_sum += cmd_queue->data[(fr + i) % cmd_queue->max_size];
			}
			if(cmd_queue->data[(fr + i) % cmd_queue->max_size] == check_sum) {
				
				#ifdef DEBUG_INTPRT
				printf("\nrun_cmd()\n");
				#endif
				
				run_cmd(cmd_queue);
				out_char_queue(cmd_queue, (char*) &check_sum);  // remove the check_sum byte
				
				#ifdef DEBUG_INTPRT
				printf("\n0\n");
				#endif
		
				return 0;
			} else {
				
				#ifdef DEBUG_INTPRT
				printf("\n-3\n");
				lcd_show_string(80,120,200,100,24, "-3");
				#endif
				
				out_char_queue(cmd_queue, (char*) &check_sum);  //remove the wrong byte
				
				return -3;
			}
		} else {
			
			#ifdef DEBUG_INTPRT
			printf("\n-2\n");
			lcd_show_string(80,120,200,100,24, "-2");
			#endif
			
			return -2;
		}
	} else {
	
		#ifdef DEBUG_INTPRT
		printf("\n-1\n");
		lcd_show_string(80,120,200,100,24, "-1");
		#endif
		
		return -1;
	}
}

uint8_t check_cmd(void)
{
	uint8_t result;
	result = check_cmd_1(&wl_queue);
	return result;
}


void interpreter_config(void)
{
	init_char_queue(&wl_queue, wl_buf, CMD_BUF_LEN);
}
