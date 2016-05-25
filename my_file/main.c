#include <string.h>

#include "stm32f4xx_it.h"
#include "main.h"
#include "configuration.h"
#include "camera.h"
#include "image.h"
#include "global.h"
#include "tftlcd.h"
#include "tracking.h"
#include "parameter.h"
#include "cmd.h"
#include "mpu6050.h"
#include "interpreter.h"
#include "can.h"
#include "communicate.h"

u8 g_exp_lock = 0;     //�ع������
float g_pre_centroid_x[10] = {0};  //����ʮ�����ڵ�����

typedef union{
    u8 u8_array[4];
    float f32_data;
}data_convert;

int main(){
    //���ݶ�����
	char str_tmp[20];
    int i,j,scanline,timeout,erro_n,k;
    int sensor_color_status;
    int hummock_cnt = 0;    //ɽ�ڼ���
    int sensor_color_antishake = 0;  //��ɫ����������
    int signal_turn_right = 0;   //����ת���ź�
    int signal_fturn_right = 0,signal_fturn_left = 0;  //ǿ����ת��ǿ����ת
    float centroid_x,centroid_y;
    float coder_turns = 0;   //������ת��Ȧ��
    float gyro[3];
    u8 return_flag = 0;
    u8 downhill_exp1_flag = 0,downhill_exp2_flag;   //�����ع������FLAG
    u32 sum_x,sum_y,sum_area; 
    int tim2_cnt = 0,tim2_cnt_last = 0;   //���������������
    int control_cnt = 0;    //���ƴ���
    float tim_ms;
    u16 lcd_id;
    char str_temp[100];
    image *ov7725_image;
    u8 up_flag,white_flag,black_flag;
    u32 up_point,down_point;  //�����ص㣬�½��ص�
	u8 **imgd;
    param_struct *param;
    GPIO_TypeDef *gpio_g;
    u32 *gpiog_idr;
    data_convert data_temp;
    u8 can_send_array[8];
/*********�������ݶ������ķָ���**********************/	

/*************�����Ǹ��ֳ�ʼ��********************/
    rcc_configuration();
    gpio_config();
    USART_Configuration();
	interpreter_config();
    comm_init();
/**********************�����ӡ������ӭ��Ϣ********************/
    uprintf(DEBUG_USARTx,"\n\n\n********************************************************\n");
    uprintf(DEBUG_USARTx,"               Welcome to BUPT ROBOCON!\n");
    uprintf(DEBUG_USARTx,"********************************************************\n");
    exti_config();  //�ⲿ�ж�����
    NVIC_Configuration();
    // cmd_init();   //��ʼ���������
    exti_color_disable(); 
    fsmc_config();
    tim3_config();
    //tim3_pwm_set(100,1800);  // ��ʼ�����
    lcd_init();
    delay_ms(2000);   //���ú�LCD֮��Ҫ��һ����ٸ���ʾ����ʾ����
    lcd_id = lcd_reg_read(LCD_REG_0);
    lcd_clear(0xFFFF);
    lcd_show_string(30,100,200,100,16,"hello! world!");
//	lcd_draw_line(0,320,10000,1);
    
    TIM2_Configuration();   //TIM2Ϊ�������ִ������
    tim4_config();   //��������������
    param_init(&param);   //��ʼ�������飬���Ҵ�FLSH�϶�ȡ����Ĳ������ڴ�
   // param_switch(0);   //С���տ�ʼ�Ĳ�������0
	

    //��������ͷ
    erro_n = camera_init(&ov7725_image);   //��ʼ������ͷ
    if(erro_n < 0){
        sprintf(str_temp,"camera init error:%d\n",erro_n);
        lcd_show_string(30,100,200,100,16,str_temp);
        uprintf(DEBUG_USARTx,str_temp);
        while(1);
    }
	camera_reg_write(0x9C, 220);
    camera_start();   //����ͷ��ʼ�ɼ�����
    //����MPU6050
//    erro_n = mpu6050_fast_init(gyro);
//    if(erro_n < 0){
//        sprintf(str_temp,"mpu6050 init error:%d\n",erro_n);
//        lcd_show_string(30,100,200,100,16,str_temp);
//        uprintf(DEBUG_USARTx,str_temp);
//        while(1);
//    }
//    mpu6050_set_gyro_adj(MPU6050_GYRO_X,3000);  //���õ���ֵ�������ã�
//    mpu6050_cycleread_start();
/************************�����Ǹ��ֳ�ʼ��****************/
/********************�����ǿ������ڵ�ѭ����*********************/
    while(1){
        tim2_cnt = TIM2->CNT;   //�������������
        coder_turns += -(TIM4->CNT - 4000.f)/CODER_PERIOD;   //����������������Ȧ��
        TIM4->CNT = 4000;
        sprintf(str_temp,"coder:%3.2f       ",coder_turns);   //��ʾ������ת��Ȧ��
        lcd_show_string(30,80,200,100,16,str_temp);
/***********************��������ɽ�ڵĲ��ִ���************************/	
        if(GPIO_ReadInputDataBit(GPIOF,SENSOR_COLOR) == 1 && sensor_color_status == 0){
            if(sensor_color_antishake == 3){   //������3���������ں���ɫ��������ȡ��ֵ���ɲ��䣬���ж�Ϊ�ȶ�
                hummock_cnt++;  //ɽ������һ
                sensor_color_status = 1;
                sprintf(str_temp,"h_cnt:%d",hummock_cnt);
                lcd_show_string(150,20,200,100,16,str_temp);
            }
            sensor_color_antishake++;  //����
        }else if(GPIO_ReadInputDataBit(GPIOF,SENSOR_COLOR) == 0 && sensor_color_status == 0){
            sensor_color_antishake = 0;
        }
        if(GPIO_ReadInputDataBit(GPIOF,SENSOR_COLOR) == 0 && sensor_color_status == 1){
            if(sensor_color_antishake == 3){   //������3���������ں���ɫ��������ȡ��ֵ���ɲ��䣬���ж�Ϊ�ȶ�
                sensor_color_status = 0;
                if(hummock_cnt == 2){
                    exti_color_enable();   //�����ж�(������ɽ�ڴ�����)
                }
                if(hummock_cnt == 3){   //��ʱС������ɽ����ǡ�ý��������
                    signal_turn_right = 1;  //��������ת����ź�
                }
                if(hummock_cnt == 4){   //��ʼ����
                    param_switch(2);   //�л����ڶ������
                    coder_turns = 0;    //����������
                }
            }
            sensor_color_antishake++;  //����
        }else if(GPIO_ReadInputDataBit(GPIOF,SENSOR_COLOR) == 1 && sensor_color_status == 1){
        		sensor_color_antishake = 0;
        }
		
/***********************��������ɽ�ڵĲ��ִ���************************/	
		
/**********************����Զ�ֵͼ������Ⱦ����������ߵ�����******************/
		//imgd = (u8**)ov2640_image->array;
		//// image_filter(imgd, IMAGE_DEFAULT_HEIGHT, IMAGE_DEFAULT_WIDTH);
		//
        //sum_x = 0;
        //sum_y = 0;
        //sum_area = 0;
        //for(i = 0;i < ov2640_image->height;i++){
        //scanline = i*(ov2640_image->width);
        //lcd_set_cursor(0,320 - i);
        //lcd_ram_write_prepare();
        //    for(j = 0;j < ov2640_image->width;j++){
        //        if(((((u8*)imgd)[(scanline + j)/8] << ((scanline + j)%8)) & 0x80) == 0x80){
        //            lcd_ram_write(0x0);  //��Ⱦ��ֵ��ͼ��
        //        }else{ 
        //            lcd_ram_write(0xffff);   //��Ⱦ��ֵ��ͼ��
        //            //��������
        //            sum_x = sum_x + j;
        //            sum_y = sum_y + i;
        //            sum_area++;
        //            //���㷽��
        //        }
        //    }
        //}
        ////��������
        //if(sum_area != 0){
        //    centroid_x = ((float)sum_x)/(float)sum_area;
        //    centroid_y = ((float)sum_y)/(float)sum_area;
        //    //����ȫ�ַ���
        //}else{
        //    centroid_x = 0;
        //    centroid_y = 0;
        //}
		
		check_cmd();
		sprintf(str_tmp, "threshold:%3d", i_threshold);
		lcd_show_string(40,120,200,100,24, str_tmp);

        sum_area = 0;
        sum_x = 0;
        for(i = 0;i < ov7725_image->height;i++){
            scanline = i*(ov7725_image->width);
            up_flag = 0;
            white_flag = 0;
            black_flag = 0;
            lcd_set_cursor(0,320 - i);
            lcd_ram_write_prepare();
            for(j = 0;j < ov7725_image->width;j++){
                if((((ov7725_image->array)[(scanline + j)/8] << ((scanline + j)%8)) & 0x80) == 0x80){ //��ɫ��
                    black_flag = 1;
                    if(white_flag == 1){   //�ж�Ϊ�½�����
                        down_point = j;   //��¼�½��ص�
                        if(up_flag == 1){   //����½���֮ǰ���������أ���������֮������ǰ���
                            if((down_point - up_point > WHITE_LINE_WIDTH_MIN) && (down_point - up_point < WHITE_LINE_WIDTH_MAX)){
                                //�˿�ȷ���ǰ���
                                lcd_set_cursor(up_point,320 - i);
                                lcd_ram_write_prepare();
                                for(k = up_point;k < down_point;k++){
                                    sum_area++;
                                    lcd_ram_write(0xffff);   //��Ⱦ��ֵ��ͼ��
                                    sum_x += k;
                                }
                                lcd_set_cursor(j,320 - i);
                                lcd_ram_write_prepare();
                            }else{
                                lcd_ram_write(0x0);  //��Ⱦ��ֵ��ͼ��
                            }
                        }else{
                            lcd_ram_write(0x0);  //��Ⱦ��ֵ��ͼ��
                        }
                    }else{
                        lcd_ram_write(0x0);  //��Ⱦ��ֵ��ͼ��
                    }
                    white_flag = 0;
                }else{ //��ɫ��
                    white_flag = 1;
                    if(black_flag == 1){   //�ж�Ϊ������
                        up_flag = 1;
                        up_point = j;  //��¼�����ص�
                    }
                    black_flag = 0;
                    lcd_ram_write(0x0);  //��Ⱦ��ֵ��ͼ��
                }
            }
        }
        if(sum_area == 0){
            centroid_x = 0;
        }else{
            centroid_x = sum_x/sum_area;
        }

        /**********************������󳵷��Ͱ��ߵ�����****************/
        uint8_t check_byte = 0x80;
        data_temp.f32_data = centroid_x;
        can_send_array[0] = check_byte;
        for(i = 0; i < 4; i++){
            can_send_array[i + 1] = data_temp.u8_array[i];
            check_byte += data_temp.u8_array[i];
        }
        can_send_msg(COMM_B_ID, can_send_array, 5);
        data_temp.f32_data = centroid_y;
        for(i = 0; i < 4; i++){
            can_send_array[i] = data_temp.u8_array[i];
            check_byte += data_temp.u8_array[i];
        }
        can_send_array[4] = check_byte;
        can_send_msg(COMM_B_ID, can_send_array, 5);
        /**********************������󳵷��Ͱ��ߵ�����****************/


        sprintf(str_temp,"x:%3.2f y:%3.2f",centroid_x,centroid_y);
        lcd_show_string(30,40,200,100,16,str_temp);
        tim2_cnt = TIM2->CNT - tim2_cnt;  //�����������
        if(tim2_cnt < 0){
            tim2_cnt = tim2_cnt_last;   //�п��ܷ�����ʱ�����أ���ʱ��õ�ʱ���Ǹ��ģ�����
        }
        tim2_cnt_last = tim2_cnt;
        tim_ms = tim2_cnt * 0.0595;  //��ʱ����������16800��1S���һ�����ڣ����ԣ���ʱ����ÿ����ʱ״̬��1000/16800=0.0595 ms
        sprintf(str_temp,"dt:%3.2fms         ",tim_ms);
        lcd_show_string(30,100,200,100,16,str_temp);
        if(return_flag != 1){  //����ڸÿ��������ڣ�����ͷ��ʧ�˰��ߣ����ڻص�����״̬�У������ӿ��ƴ���
            control_cnt++;  //���ƴ�����һ
        }
    }
/*******************���ϵĴ��붼���ڿ������ڵ�ѭ��������*************************/
}


