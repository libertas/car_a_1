/**
  ******************************************************************************
  * @file    Project/STM32F4xx_StdPeriph_Templates/stm32f4xx_it.c 
  * @author  MCD Application Team
  * @version V1.4.0
  * @date    04-August-2014
  * @brief   Main Interrupt Service Routines.
  *          This file provides template for all exceptions handler and 
  *          peripherals interrupt service routine.
  ******************************************************************************
  * @attention
  *
  * <h2><center>&copy; COPYRIGHT 2014 STMicroelectronics</center></h2>
  *
  * Licensed under MCD-ST Liberty SW License Agreement V2, (the "License");
  * You may not use this file except in compliance with the License.
  * You may obtain a copy of the License at:
  *
  *        http://www.st.com/software_license_agreement_liberty_v2
  *
  * Unless required by applicable law or agreed to in writing, software 
  * distributed under the License is distributed on an "AS IS" BASIS, 
  * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
  * See the License for the specific language governing permissions and
  * limitations under the License.
  *
  ******************************************************************************
  */

/* Includes ------------------------------------------------------------------*/
#include "stm32f4xx_it.h"
#include "main.h"
#include "global.h"
#include "tftlcd.h"
#include "stdlib.h"
#include "camera.h"
#include "parameter.h"

/** @addtogroup Template_Project
  * @{
  */

/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
/* Private function prototypes -----------------------------------------------*/
/* Private functions ---------------------------------------------------------*/

/******************************************************************************/
/*            Cortex-M4 Processor Exceptions Handlers                         */
/******************************************************************************/

/**
  * @brief  This function handles NMI exception.
  * @param  None
  * @retval None
  */
	char str_temp[100];
	int g_light_sensor_left = 0,g_light_sensor_right = 0;
	int g_color_right_flag = 0;
void NMI_Handler(void)
{
}

/**
  * @brief  This function handles Hard Fault exception.
  * @param  None
  * @retval None
  */
void HardFault_Handler(void)
{
  /* Go to infinite loop when Hard Fault exception occurs */
  while (1)
  {
  }
}

/**
  * @brief  This function handles Memory Manage exception.
  * @param  None
  * @retval None
  */
void MemManage_Handler(void)
{
  /* Go to infinite loop when Memory Manage exception occurs */
  while (1)
  {
  }
}

/**
  * @brief  This function handles Bus Fault exception.
  * @param  None
  * @retval None
  */
void BusFault_Handler(void)
{
  /* Go to infinite loop when Bus Fault exception occurs */
  while (1)
  {
  }
}

/**
  * @brief  This function handles Usage Fault exception.
  * @param  None
  * @retval None
  */
void UsageFault_Handler(void)
{
  /* Go to infinite loop when Usage Fault exception occurs */
  while (1)
  {
  }
}

/**
  * @brief  This function handles SVCall exception.
  * @param  None
  * @retval None
  */
void SVC_Handler(void)
{
}

/**
  * @brief  This function handles Debug Monitor exception.
  * @param  None
  * @retval None
  */
void DebugMon_Handler(void)
{
}




/**
  * @brief  This function handles PendSVC exception.
  * @param  None
  * @retval None
  */
void PendSV_Handler(void)
{
}

#include "interpreter.h"
void UART5_IRQHandler(void)
{
	char data;

	if(USART_GetITStatus(UART5, USART_IT_RXNE) != RESET)
	{
		data = USART_ReceiveData(UART5);
		
		in_char_queue(&wl_queue, data);
	}
}


/* void EXTI3_IRQHandler(void){ */
		/* if(EXTI_GetITStatus(EXTI_Line3) != RESET){ */
				/* int i; */
				/* if(GPIO_ReadInputDataBit(GPIOD,GPIO_Pin_3) == 0){ */
						/* DMA2_Stream1->CR &= ~(uint32_t)DMA_SxCR_EN; //??DMA */
						/* while(DMA2_Stream1->CR&0X01);//??DMA2_Stream1??? */
						/* DMA2->LIFCR|=0X3D<<6;	 //????1??????? */
						/* DMA2_Stream1->NDTR = 20000; */
						/* DMA2_Stream1->FCR=0X0000021;//?????? */
						/* DMA2_Stream1->M0AR=(uint32_t)(g_camera_image);//(g_camera_image)(0x60000000|0x0C000000|0x00000080);   //????????? */
						/* g_camera_line_cnt = 0; */
						/* DMA2_Stream1->CR |= (uint32_t)DMA_SxCR_EN;  //??DMA */
						/* g_camera_irq_flag = 1; */
						/* g_camera_cols_cnt++; */
				/* } */
				/* EXTI_ClearITPendingBit(EXTI_Line3); */
		/* } */
/* } */

void EXTI9_5_IRQHandler(void){
		static int color_sensor_left_cnt = 0,color_sensor_right_cnt = 0;
		static int light_sensor_left_cnt = 0,light_sensor_right_cnt = 0;
//		if(EXTI_GetITStatus(EXTI_Line6) != RESET){
//			//	delay_ms(500);
//				if(GPIO_ReadInputDataBit(GPIOF,GPIO_Pin_6) == 1){
////						sprintf(str_temp,"1:%d",color_sensor_left_cnt++);
////						lcd_show_string(100,20,200,100,16,str_temp);

//				}else if(GPIO_ReadInputDataBit(GPIOF,GPIO_Pin_6) == 0){
//			//			delay_ms(1500);
////						sprintf(str_temp,"0:%d",++color_sensor_right_cnt);
////						lcd_show_string(150,20,200,100,16,str_temp);
//						if(color_sensor_right_cnt == 3){
//								g_color_right_flag = 1;
//						}
//				}
//				EXTI_ClearITPendingBit(EXTI_Line6);
//		}
		if(EXTI_GetITStatus(EXTI_Line7) != RESET){
				int i;
				if(GPIO_ReadInputDataBit(GPIOF,GPIO_Pin_7) == 1){
						tim3_pwm_set(50,860);  //往右打死 
						for(i = 0;i < 10;i++){
								g_pre_centroid_x[i] = 180;   //固定弯拐完之后，白线还是不见的话，继续右拐
						}
						param_switch(1);  //切换参数组(过河流参数组)
						sprintf(str_temp,"g:1"); 
						lcd_show_string(100,20,200,100,16,str_temp);
						delay_ms(10000);
						exti_color_disable();  //关中断
				}
				EXTI_ClearITPendingBit(EXTI_Line7);
		}
		
//		if(EXTI_GetITStatus(EXTI_Line8) != RESET){
//				delay_ms(300);
//				if(GPIO_ReadInputDataBit(GPIOF,GPIO_Pin_8) == 1){
//						g_light_sensor_right = 1;
//				}else if(GPIO_ReadInputDataBit(GPIOF,GPIO_Pin_8) == 0){
//						g_light_sensor_right = 0;
//				}
//				EXTI_ClearITPendingBit(EXTI_Line8);
//		}
}  


void TIM2_IRQHandler(void){
	  if( TIM_GetITStatus(TIM2, TIM_IT_Update) != RESET ) 
  	{
			TIM_ClearITPendingBit(TIM2,TIM_FLAG_Update);//必须清除中断标志位否则一直中断
		}	
}
//蓝牙串口中断处理函数
//该中断处理函数是用来解析命令的
//其中定义了大量的静态变量，原因是该中断处理函数需要调用若干次才解析完一条指令，某些变量不希望一次调用完函数之后就被撤销掉
/* void UART5_IRQHandler(void){ */
		/* static u8 c_now,c_last; */
		/* static u8 cmd_state = 0;   //表示命令读取状态  0 非命令读取状态   1 命令读取状态 */
		/* static u8 cmd;  //命令 */
		/* static u8 camera_reg_addr,camera_reg_data,camera_reg_type = 0,index1,index2; */
		/* static u8 str_camera_cmd[2][5],str_flash_type[5]; */
		/* static u8 str_param[3][PARAM_NAME_LENGTH];  //解析控制参数的命令用 */
		/* static float param_value_temp; */
		/* static float param_value_array[PARAM_GROUP_LENGTH] = {0}; */
		/* static int param_group; */
		/* static list_node *p_list_node; */
		/* static u8 flash_type; */
		/* static u8 exit_flag = 0; */
		/* static u32 flash_data_temp = 0,flash_write_addr; */
		/* static int i; */
		/* if(USART_GetITStatus(UART5,USART_IT_RXNE) != RESET){  // */
				/* USART_ClearITPendingBit(UART5,USART_IT_RXNE); */
				/* c_now = USART_ReceiveData(UART5); */
				/* if(c_now == '['){ */
						/* cmd_state = 1;   //进入命令读取状态 */
						/* c_last = c_now; */
						/* return;  //退出，等待下一个数据的接收 */
				/* }else if(c_now == ']'){ */
						/* cmd_state = 0;   //退出命令读取状态且开始执行命令 */
						/* switch(cmd){ */
							/* case 'A': */
								/* str_camera_cmd[index1][index2] = 0; */
								/* camera_reg_addr = atoi((char *)str_camera_cmd[0]); */
								/* camera_reg_data = atoi((char *)str_camera_cmd[1]); */
								/* sprintf(str_temp,"addr:0x%x data:%d",camera_reg_addr,camera_reg_data); */
								/* lcd_show_string(30,60,200,100,16,str_temp); */
								/* camera_reg_write(camera_reg_addr,camera_reg_data); */
								/* if(camera_reg_addr == 0xff){ */
									/* camera_reg_type = camera_reg_data; */
								/* }else if(camera_reg_addr != 0xff){ */
									/* ov2640_save_data_toram(g_ov2640_custom_config,camera_reg_addr,camera_reg_data,camera_reg_type); */
								/* } */
								/* break; */
							/* case 'F': */
								/* DCMI_Cmd(DISABLE);  //DCMI接口与FLASH写有冲突，所以，写FLASH之前先关闭DCMI */
								/* delay_ms(1000);   //等待DCMI关好 */
								/* FLASH_Unlock();  //解锁FLASH，写数据 */
								/* FLASH_DataCacheCmd(DISABLE); */
								/* FLASH_ClearFlag(FLASH_FLAG_EOP | FLASH_FLAG_OPERR | FLASH_FLAG_WRPERR |  */
																/* FLASH_FLAG_PGAERR | FLASH_FLAG_PGPERR|FLASH_FLAG_PGSERR);  //写FLASH之前，先清除标志 */
								/* if(FLASH_EraseSector(OV2650_FLASH_SETOR, VoltageRange_3) != FLASH_COMPLETE){  */
										/* lcd_show_string(30,20,200,100,16,"flash erase error!"); */
										/* FLASH_DataCacheCmd(ENABLE); */
										/* FLASH_Lock();//上锁 */
										/* DCMI_Cmd(ENABLE); */
										/* return; */
								/* }else{ */
										/* lcd_show_string(30,20,200,100,16,"flash erase finish"); */
								/* } */
								/* flash_write_addr = OV2640_FLASH_ADDR_START; */
								/* i = 0; */
								/* exit_flag = 0; */
								/* while(1){ */
										/* flash_data_temp = 0; */
										/* flash_data_temp = (g_ov2640_custom_config[i][1] << 8) | (g_ov2640_custom_config[i][0]);  //地址在低8位，数据在高8位 */
										/* if(FLASH_ProgramWord(flash_write_addr,flash_data_temp) != FLASH_COMPLETE){ */
												/* lcd_show_string(30,40,200,100,16,"flash write error!"); */
												/* FLASH_DataCacheCmd(ENABLE); */
												/* FLASH_Lock();//上锁 */
												/* DCMI_Cmd(ENABLE); */
												/* return; */
										/* } */
										/* if(g_ov2640_custom_config[i][0] == 0xff && exit_flag == 1){ */
												/* break;  //连续两次0xff表示烧写FLASH结束 */
										/* }else if(g_ov2640_custom_config[i][0] == 0xff){ */
												/* exit_flag = 1; */
										/* }else{ */
												/* exit_flag = 0; */
										/* } */
										/* flash_write_addr += 4; */
										/* i++; */
								/* } */
								/* lcd_show_string(30,40,200,100,16,"flash write finish!"); */
								/* FLASH_DataCacheCmd(ENABLE); */
								/* FLASH_Lock();//上锁1 */
								/* DCMI_Cmd(ENABLE); */
								/* break;  //'F'break; */
							/* case 'P': */
								/* ov2640_stop();   //由于用蓝牙传送信息比较慢，在这段时间如果还开着DCMI，可能DMA传输使得数组越界,而且 DCMI接口与FLASH写有冲突，所以，写FLASH之前先关闭DCMI */
								/* str_param[index1][index2] = 0; */
								/* param_group = atoi((char *)str_param[0]);  //解析组别 */
								/* param_value_temp = atof((char *)str_param[2]);   //解析参数值 */
								/* if(strcmp((char *)str_param[1],"save") == 0){ */
										/* if(param_save_to_flash() == -1 ){ */
												/* lcd_show_string(30,20,200,100,16,"flash erase error!"); */
										/* } */
										/* ov2640_start(); */
										/* return; */
								/* }else if(strcmp((char *)str_param[1],"print") == 0){   //打印参数链表的元素 */
										/* list_print(UART5,&g_param_list,param_group); */
										/* ov2640_start(); */
										/* return; */
								/* }else if(strcmp((char *)str_param[1],"load") == 0){ */
										/* param_ld_from_flash(); */
										/* ov2640_start(); */
										/* return; */
								/* }else if(strcmp((char *)str_param[1],"reset") == 0){ */
										/* param_list_reset(); */
										/* ov2640_start(); */
										/* return; */
								/* }else if(strcmp((char *)str_param[1],"switch") == 0){ */
										/* param_switch(&g_control_param,param_group); */
										/* return; */
								/* }else if(strcmp((char *)str_param[1],"exp_lock") == 0){ */
										/* g_exp_lock = ~(g_exp_lock | 0xFE);   //曝光度锁开关 */
								/* } */
								/* if((p_list_node = list_search(&g_param_list,(char *)str_param[1])) != NULL){ */
										/* p_list_node->data->param_value[param_group] = param_value_temp; */
								/* }else{  //如果链表中不存在传过来的参数，则插入新的节点 */
										/* memset(param_value_array,0,sizeof(float)*PARAM_GROUP_LENGTH); */
										/* param_value_array[param_group] = param_value_temp; */
										/* list_insert(&g_param_list,list_get_length(&g_param_list)+1,(char *)str_param[1],param_value_array); */
								/* } */
								/* if(strcmp((char *)str_param[1],"threshold") == 0){ */
										/* camera_reg_write(0x9c,param_value_temp); */
								/* }else if(strcmp((char *)str_param[1],"adj_exp1") == 0){ */
										/* camera_reg_write(0x9C,param_value_temp); */
								/* }else if(strcmp((char *)str_param[1],"adj_exp2") == 0){ */
										/* camera_reg_write(0x9C,param_value_temp); */
								/* } */
								/* param_update_all(); */
								/* ov2640_start(); */
								/* break;   //'P'break */
								
						/* } */
				/* } */
				/* if(cmd_state == 1){   //如果处于命令读取状态 */
						/* if(c_last == '['){  //读取命令 */
								/* cmd = c_now; */
								/* c_last = c_now; */
								/* return; */
						/* }else{  //读取命令参数 */
								/* switch(cmd){ */
									/* case 'A':  //A被定义为摄像头配置命令 */
										/* if(c_last == 'A'){ */
												/* index1 = 0; */
												/* index2 = 0; */
												/* c_last = c_now; */
												/* return; */
										/* } */
										/* if(c_now == '#'){ */
												/* str_camera_cmd[index1][index2] = 0; */
												/* index1++; */
												/* index2 = 0; */
												/* c_last = c_now; */
												
										/* }else if(c_now != '#'){ */
												/* str_camera_cmd[index1][index2++] = c_now; */
												/* c_last = c_now; */
										/* } */
										/* break; */
										/* case 'P': */
											/* if(c_last == 'P'){ */
													/* index1 = 0; */
													/* index2 = 0; */
													/* c_last = c_now; */
													/* return; */
											/* } */
											/* if(c_now == '#'){ */
													/* str_param[index1][index2] = 0; */
													/* index1++; */
													/* index2 = 0; */
													/* c_last = c_now; */
											/* }else if(c_now != '#'){ */
													/* str_param[index1][index2++] = c_now; */
													/* c_last = c_now; */
											/* } */
											/* break; */
									
								/* } */
						/* } */
				/* } */
		/* } */
/* } */

/**
  * @brief  This function handles SysTick Handler.
  * @param  None
  * @retval None
  */
//void SysTick_Handler(void)
//{
//  TimingDelay_Decrement();
//}

/******************************************************************************/
/*                 STM32F4xx Peripherals Interrupt Handlers                   */
/*  Add here the Interrupt Handler for the used peripheral(s) (PPP), for the  */
/*  available peripheral interrupt handler's name please refer to the startup */
/*  file (startup_stm32f4xx.s).                                               */
/******************************************************************************/

/**
  * @brief  This function handles PPP interrupt request.
  * @param  None
  * @retval None
  */
/*void PPP_IRQHandler(void)
{
}*/

/**
  * @}
  */ 


/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
