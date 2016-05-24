#include "communicate.h"
#include "can.h"

void comm_msg_rcv_callback(CanRxMsg *can_rx_msg);

void comm_init(void)
{
		can_init(comm_msg_rcv_callback);
}

int comm_send(void)
{
		u8 msg_send[5] = {0,1,2,3,4};
		return can_send_msg(COMM_B_ID, msg_send,5);
}

void comm_msg_rcv_callback(CanRxMsg *can_rx_msg)
{
		if(can_rx_msg->StdId == COMM_A_ID){
				uint8_t length;
				char data;
				length = can_rx_msg->DLC;
				for(int i = 0; i <length; i++){
						data = can_rx_msg->Data[i];
						//printf("canok%d\r\n",data);
				}
		}
}
