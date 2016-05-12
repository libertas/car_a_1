#include <stdint.h>

#include "utils.h"

#define CMD_BUF_LEN 64

extern char_queue handler_queue;
extern char_queue wl_queue;

void interpreter_config(void);
uint8_t check_cmd(void);
int check_cmd_1(char_queue *cmd_queue);
int run_cmd(char_queue *cmd_queue);
