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
						tim3_pwm_set(50,860);  //���Ҵ��� 
						for(i = 0;i < 10;i++){
								g_pre_centroid_x[i] = 180;   //�̶������֮�󣬰��߻��ǲ����Ļ��������ҹ�
						}
						param_switch(1);  //�л�������(������������)
						sprintf(str_temp,"g:1"); 
						lcd_show_string(100,20,200,100,16,str_temp);
						delay_ms(10000);
						exti_color_disable();  //���ж�
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
			TIM_ClearITPendingBit(TIM2,TIM_FLAG_Update);//��������жϱ�־λ����һֱ�ж�
		}	
}
//���������жϴ�����
//���жϴ��������������������
//���ж����˴����ľ�̬������ԭ���Ǹ��жϴ�������Ҫ�������ɴβŽ�����һ��ָ�ĳЩ������ϣ��һ�ε����꺯��֮��ͱ�������
/* void UART5_IRQHandler(void){ */
		/* static u8 c_now,c_last; */
		/* static u8 cmd_state = 0;   //��ʾ�����ȡ״̬  0 �������ȡ״̬   1 �����ȡ״̬ */
		/* static u8 cmd;  //���� */
		/* static u8 camera_reg_addr,camera_reg_data,camera_reg_type = 0,index1,index2; */
		/* static u8 str_camera_cmd[2][5],str_flash_type[5]; */
		/* static u8 str_param[3][PARAM_NAME_LENGTH];  //�������Ʋ����������� */
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
						/* cmd_state = 1;   //���������ȡ״̬ */
						/* c_last = c_now; */
						/* return;  //�˳����ȴ���һ�����ݵĽ��� */
				/* }else if(c_now == ']'){ */
						/* cmd_state = 0;   //�˳������ȡ״̬�ҿ�ʼִ������ */
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
								/* DCMI_Cmd(DISABLE);  //DCMI�ӿ���FLASHд�г�ͻ�����ԣ�дFLASH֮ǰ�ȹر�DCMI */
								/* delay_ms(1000);   //�ȴ�DCMI�غ� */
								/* FLASH_Unlock();  //����FLASH��д���� */
								/* FLASH_DataCacheCmd(DISABLE); */
								/* FLASH_ClearFlag(FLASH_FLAG_EOP | FLASH_FLAG_OPERR | FLASH_FLAG_WRPERR |  */
																/* FLASH_FLAG_PGAERR | FLASH_FLAG_PGPERR|FLASH_FLAG_PGSERR);  //дFLASH֮ǰ���������־ */
								/* if(FLASH_EraseSector(OV2650_FLASH_SETOR, VoltageRange_3) != FLASH_COMPLETE){  */
										/* lcd_show_string(30,20,200,100,16,"flash erase error!"); */
										/* FLASH_DataCacheCmd(ENABLE); */
										/* FLASH_Lock();//���� */
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
										/* flash_data_temp = (g_ov2640_custom_config[i][1] << 8) | (g_ov2640_custom_config[i][0]);  //��ַ�ڵ�8λ�������ڸ�8λ */
										/* if(FLASH_ProgramWord(flash_write_addr,flash_data_temp) != FLASH_COMPLETE){ */
												/* lcd_show_string(30,40,200,100,16,"flash write error!"); */
												/* FLASH_DataCacheCmd(ENABLE); */
												/* FLASH_Lock();//���� */
												/* DCMI_Cmd(ENABLE); */
												/* return; */
										/* } */
										/* if(g_ov2640_custom_config[i][0] == 0xff && exit_flag == 1){ */
												/* break;  //��������0xff��ʾ��дFLASH���� */
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
								/* FLASH_Lock();//����1 */
								/* DCMI_Cmd(ENABLE); */
								/* break;  //'F'break; */
							/* case 'P': */
								/* ov2640_stop();   //����������������Ϣ�Ƚ����������ʱ�����������DCMI������DMA����ʹ������Խ��,���� DCMI�ӿ���FLASHд�г�ͻ�����ԣ�дFLASH֮ǰ�ȹر�DCMI */
								/* str_param[index1][index2] = 0; */
								/* param_group = atoi((char *)str_param[0]);  //������� */
								/* param_value_temp = atof((char *)str_param[2]);   //��������ֵ */
								/* if(strcmp((char *)str_param[1],"save") == 0){ */
										/* if(param_save_to_flash() == -1 ){ */
												/* lcd_show_string(30,20,200,100,16,"flash erase error!"); */
										/* } */
										/* ov2640_start(); */
										/* return; */
								/* }else if(strcmp((char *)str_param[1],"print") == 0){   //��ӡ���������Ԫ�� */
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
										/* g_exp_lock = ~(g_exp_lock | 0xFE);   //�ع�������� */
								/* } */
								/* if((p_list_node = list_search(&g_param_list,(char *)str_param[1])) != NULL){ */
										/* p_list_node->data->param_value[param_group] = param_value_temp; */
								/* }else{  //��������в����ڴ������Ĳ�����������µĽڵ� */
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
				/* if(cmd_state == 1){   //������������ȡ״̬ */
						/* if(c_last == '['){  //��ȡ���� */
								/* cmd = c_now; */
								/* c_last = c_now; */
								/* return; */
						/* }else{  //��ȡ������� */
								/* switch(cmd){ */
									/* case 'A':  //A������Ϊ����ͷ�������� */
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
