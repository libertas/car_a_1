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

u8 g_exp_lock = 0;     //曝光度锁定
float g_pre_centroid_x[10] = {0};  //保存十个周期的质心

typedef union{
    u8 u8_array[4];
    float f32_data;
}data_convert;

int main(){
    //数据定义区
	char str_tmp[20];
    int i,j,scanline,timeout,erro_n,k;
    int sensor_color_status;
    int hummock_cnt = 0;    //山岗计数
    int sensor_color_antishake = 0;  //颜色传感器防抖
    int signal_turn_right = 0;   //向右转的信号
    int signal_fturn_right = 0,signal_fturn_left = 0;  //强制右转和强制左转
    float centroid_x,centroid_y;
    float coder_turns = 0;   //编码器转的圈数
    float gyro[3];
    u8 return_flag = 0;
    u8 downhill_exp1_flag = 0,downhill_exp2_flag;   //下坡曝光度设置FLAG
    u32 sum_x,sum_y,sum_area; 
    int tim2_cnt = 0,tim2_cnt_last = 0;   //用作计算控制周期
    int control_cnt = 0;    //控制次数
    float tim_ms;
    u16 lcd_id;
    char str_temp[100];
    image *ov7725_image;
    u8 up_flag,white_flag,black_flag;
    u32 up_point,down_point;  //上升沿点，下降沿点
	u8 **imgd;
    param_struct *param;
    GPIO_TypeDef *gpio_g;
    u32 *gpiog_idr;
    data_convert data_temp;
    u8 can_send_array[8];
/*********这是数据定义区的分割线**********************/	

/*************以下是各种初始化********************/
    rcc_configuration();
    gpio_config();
    USART_Configuration();
	interpreter_config();
    comm_init();
/**********************下面打印开机欢迎信息********************/
    uprintf(DEBUG_USARTx,"\n\n\n********************************************************\n");
    uprintf(DEBUG_USARTx,"               Welcome to BUPT ROBOCON!\n");
    uprintf(DEBUG_USARTx,"********************************************************\n");
    exti_config();  //外部中断设置
    NVIC_Configuration();
    // cmd_init();   //初始化命令程序
    exti_color_disable(); 
    fsmc_config();
    tim3_config();
    //tim3_pwm_set(100,1800);  // 初始化舵机
    lcd_init();
    delay_ms(2000);   //配置好LCD之后要等一会儿再给显示器显示数据
    lcd_id = lcd_reg_read(LCD_REG_0);
    lcd_clear(0xFFFF);
    lcd_show_string(30,100,200,100,16,"hello! world!");
//	lcd_draw_line(0,320,10000,1);
    
    TIM2_Configuration();   //TIM2为计算程序执行周期
    tim4_config();   //正交编码器所用
    param_init(&param);   //初始化参数组，而且从FLSH上读取保存的参数到内存
   // param_switch(0);   //小车刚开始的参数组是0
	

    //配置摄像头
    erro_n = camera_init(&ov7725_image);   //初始化摄像头
    if(erro_n < 0){
        sprintf(str_temp,"camera init error:%d\n",erro_n);
        lcd_show_string(30,100,200,100,16,str_temp);
        uprintf(DEBUG_USARTx,str_temp);
        while(1);
    }
	camera_reg_write(0x9C, 220);
    camera_start();   //摄像头开始采集数据
    //配置MPU6050
//    erro_n = mpu6050_fast_init(gyro);
//    if(erro_n < 0){
//        sprintf(str_temp,"mpu6050 init error:%d\n",erro_n);
//        lcd_show_string(30,100,200,100,16,str_temp);
//        uprintf(DEBUG_USARTx,str_temp);
//        while(1);
//    }
//    mpu6050_set_gyro_adj(MPU6050_GYRO_X,3000);  //设置调整值（调试用）
//    mpu6050_cycleread_start();
/************************以上是各种初始化****************/
/********************以下是控制周期的循环体*********************/
    while(1){
        tim2_cnt = TIM2->CNT;   //计算程序周期用
        coder_turns += -(TIM4->CNT - 4000.f)/CODER_PERIOD;   //计算正交编码器的圈数
        TIM4->CNT = 4000;
        sprintf(str_temp,"coder:%3.2f       ",coder_turns);   //显示码盘所转的圈数
        lcd_show_string(30,80,200,100,16,str_temp);
/***********************以下是数山岗的部分代码************************/	
        if(GPIO_ReadInputDataBit(GPIOF,SENSOR_COLOR) == 1 && sensor_color_status == 0){
            if(sensor_color_antishake == 3){   //经过了3个控制周期后，颜色传感器读取的值依旧不变，则判定为稳定
                hummock_cnt++;  //山岗数加一
                sensor_color_status = 1;
                sprintf(str_temp,"h_cnt:%d",hummock_cnt);
                lcd_show_string(150,20,200,100,16,str_temp);
            }
            sensor_color_antishake++;  //防抖
        }else if(GPIO_ReadInputDataBit(GPIOF,SENSOR_COLOR) == 0 && sensor_color_status == 0){
            sensor_color_antishake = 0;
        }
        if(GPIO_ReadInputDataBit(GPIOF,SENSOR_COLOR) == 0 && sensor_color_status == 1){
            if(sensor_color_antishake == 3){   //经过了3个控制周期后，颜色传感器读取的值依旧不变，则判定为稳定
                sensor_color_status = 0;
                if(hummock_cnt == 2){
                    exti_color_enable();   //开启中断(第三个山岗打死弯)
                }
                if(hummock_cnt == 3){   //此时小车走完山岗三恰好进入河流段
                    signal_turn_right = 1;  //设置向右转弯的信号
                }
                if(hummock_cnt == 4){   //开始下坡
                    param_switch(2);   //切换到第二组参数
                    coder_turns = 0;    //编码器清零
                }
            }
            sensor_color_antishake++;  //防抖
        }else if(GPIO_ReadInputDataBit(GPIOF,SENSOR_COLOR) == 1 && sensor_color_status == 1){
        		sensor_color_antishake = 0;
        }
		
/***********************以上是数山岗的部分代码************************/	
		
/**********************下面对二值图进行渲染，而且算白线的质心******************/
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
        //            lcd_ram_write(0x0);  //渲染二值化图像
        //        }else{ 
        //            lcd_ram_write(0xffff);   //渲染二值化图像
        //            //计算质心
        //            sum_x = sum_x + j;
        //            sum_y = sum_y + i;
        //            sum_area++;
        //            //计算方差
        //        }
        //    }
        //}
        ////计算质心
        //if(sum_area != 0){
        //    centroid_x = ((float)sum_x)/(float)sum_area;
        //    centroid_y = ((float)sum_y)/(float)sum_area;
        //    //计算全局方差
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
                if((((ov7725_image->array)[(scanline + j)/8] << ((scanline + j)%8)) & 0x80) == 0x80){ //黑色的
                    black_flag = 1;
                    if(white_flag == 1){   //判断为下降升沿
                        down_point = j;   //记录下降沿点
                        if(up_flag == 1){   //如果下降沿之前，有上升沿，则两个沿之间可能是白线
                            if((down_point - up_point > WHITE_LINE_WIDTH_MIN) && (down_point - up_point < WHITE_LINE_WIDTH_MAX)){
                                //此刻确定是白线
                                lcd_set_cursor(up_point,320 - i);
                                lcd_ram_write_prepare();
                                for(k = up_point;k < down_point;k++){
                                    sum_area++;
                                    lcd_ram_write(0xffff);   //渲染二值化图像
                                    sum_x += k;
                                }
                                lcd_set_cursor(j,320 - i);
                                lcd_ram_write_prepare();
                            }else{
                                lcd_ram_write(0x0);  //渲染二值化图像
                            }
                        }else{
                            lcd_ram_write(0x0);  //渲染二值化图像
                        }
                    }else{
                        lcd_ram_write(0x0);  //渲染二值化图像
                    }
                    white_flag = 0;
                }else{ //白色的
                    white_flag = 1;
                    if(black_flag == 1){   //判断为上升沿
                        up_flag = 1;
                        up_point = j;  //记录上升沿点
                    }
                    black_flag = 0;
                    lcd_ram_write(0x0);  //渲染二值化图像
                }
            }
        }
        if(sum_area == 0){
            centroid_x = 0;
        }else{
            centroid_x = sum_x/sum_area;
        }

        /**********************下面给大车发送白线的质心****************/
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
        /**********************上面给大车发送白线的质心****************/


        sprintf(str_temp,"x:%3.2f y:%3.2f",centroid_x,centroid_y);
        lcd_show_string(30,40,200,100,16,str_temp);
        tim2_cnt = TIM2->CNT - tim2_cnt;  //计算程序周期
        if(tim2_cnt < 0){
            tim2_cnt = tim2_cnt_last;   //有可能发生计时器重载，此时算得的时间是负的，舍弃
        }
        tim2_cnt_last = tim2_cnt;
        tim_ms = tim2_cnt * 0.0595;  //计时器的周期是16800，1S完成一个周期，所以，计时器的每个计时状态是1000/16800=0.0595 ms
        sprintf(str_temp,"dt:%3.2fms         ",tim_ms);
        lcd_show_string(30,100,200,100,16,str_temp);
        if(return_flag != 1){  //如果在该控制周期内，摄像头丢失了白线，处于回调方向状态中，则不增加控制次数
            control_cnt++;  //控制次数加一
        }
    }
/*******************以上的代码都属于控制周期的循环体里面*************************/
}


