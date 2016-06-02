
// /* Includes ------------------------------------------------------------------*/
// #include "includes.h"
// #include "math.h"

// #define Acc_Limit_Enable 0
// #define Max_Acc 50

// //���������ϵ�ǻ����˵ľֲ�����ϵ
// double Speed_X;				// mm/s
// double Speed_Y;				// mm/s
// double Speed_Rotation;		// (rad/s)/100

// extern double coeff_x,coeff_y,coeff_r; //�ſ˱Ⱦ��󻯼���ϵ��


// struct Mecanum_State Mec_FL={1, -1};		//���ֵ�״̬����
// struct Mecanum_State Mec_FR={2, 1};
// struct Mecanum_State Mec_BL={3, -1};
// struct Mecanum_State Mec_BR={4, 1};

// /*
//  * ������: Mecanum_test
//  * ��  ��: ���ӵ���
//  * ��  ��: ����״̬��Ϣ
//  * ��  ��: ��
//  * ��  ��: �ⲿ����
//  */
// void Mecanum_test(struct Mecanum_State *mec)
// {
// 	rt_uint32_t key_value;
// 	double speed_pwm=50;

// /*->*/mec_test:
// 	while(1)
// 	{
// 		LCD_Clear();
// 		LCD_SetXY(0,0);
// 		LCD_WriteString("======Mecanum=======");
// 		LCD_SetXY(0,1);
// 		LCD_WriteString("1.Speed");
// 		LCD_SetXY(0,2);
// 		LCD_WriteString("2.ChangeArg");
// 		LCD_SetXY(0,3);
// 		LCD_WriteString("3.ChangePort now:");
// 		LCD_WriteInt((*mec).port);
// 		
// 		if(rt_mb_recv(&Mb_Key, &key_value, RT_WAITING_NO) == RT_EOK)
// 		{
// 			switch(key_value)
// 			{
// 				case key1:
// 					goto mec_speed;
// 				case key2:
// 					Input_IntValue(&(*mec).arg,"Argument");
// 					break;
// 				case key3:
// 					Input_IntValue(&(*mec).port,"Port");
// 					break;
// 				case keyback:
// 					return;
// 			}
// 		}
// 		Delay_ms(10);
// 	}
// 	
// /*->*/mec_speed:
// 	while(1)
// 	{
// 		LCD_Clear();
// 		LCD_SetXY(0,0);
// 		LCD_WriteString("======Mecanum=======");
// 		LCD_SetXY(0,1);
// 		LCD_WriteString("Speed_PWM:");
// 		LCD_WriteDouble(PWM_GetDuty((*mec).port));
// 		LCD_SetXY(0,2);
// 		LCD_WriteString("1.SpeedUp");
// 		LCD_SetXY(0,3);
// 		LCD_WriteString("2.SpeedDown");
// 		LCD_SetXY(0,4);
// 		LCD_WriteString("3.Stop");
// 		
// 		if(rt_mb_recv(&Mb_Key, &key_value, RT_WAITING_NO) == RT_EOK)
// 		{
// 			switch(key_value)
// 			{
// 				case key1:
// 					if(speed_pwm<100)
// 						speed_pwm+=1;
// 					break;
// 				case key2:
// 					if(speed_pwm>0)
// 						speed_pwm-=1;
// 					break;
// 				case key3:
// 					speed_pwm=50;
// 					break;
// 				case keyback:
// 					goto mec_test;
// 			}
// 			if(key_value == keyback)
// 			{
// 				PWM_SetDuty((*mec).port,50);
// 				break;
// 			}
// 			PWM_SetDuty((*mec).port,speed_pwm);
// 		}
// 		Delay_ms(10);
// 	}
// }


// /*
//  * ������: SetSpeed
//  * ��  ��: Ϊ�����˸��ٶ�
//  * ��  ��: ����������ϵX�����ٶ�(mm/s)��Y�����ٶ�(mm/s)����ת�ٶ�(rad/s)/100
//  * ��  ��: ��
//  * ��  ��: �ⲿ����
//  */
// //void SetSpeed(double fl_speed,double fr_speed,double bl_speed,double br_speed)
// void SetSpeed(double speed_x,double speed_y,double speed_rotation)
// {
// 	double fl_speed;
// 	double fr_speed;
// 	double bl_speed;
// 	double br_speed;
// 	
// 	double fl_delta;
// 	double fr_delta;
// 	double bl_delta;
// 	double br_delta;
// 	
// 	static double fl_last=0;	//�������ֵ��ٶ�
// 	static double fr_last=0;
// 	static double bl_last=0;
// 	static double br_last=0;
// 	
// 	double delta_max;			//�ٶȱ仯�����ֵ
// 	double PWM_Max;				//���ɵ�PWM���Ӧ���ٶ����ֵ
// 	
// 	double PWM_FL,PWM_FR,PWM_BL,PWM_BR;
// 	
// 	/****************************ͨ���ſ˱Ⱦ�����ٶȷֽ⵽��������******************************/
// 		
// // 	fl_speed =  coeff_x*speed_x + coeff_y*speed_y - coeff_r*speed_rotation/100 ;
// // 	fr_speed = -coeff_x*speed_x + coeff_y*speed_y + coeff_r*speed_rotation/100 ;
// // 	bl_speed = -coeff_x*speed_x + coeff_y*speed_y - coeff_r*speed_rotation/100 ;
// // 	br_speed =  coeff_x*speed_x + coeff_y*speed_y + coeff_r*speed_rotation/100 ;
// fl_speed = speed_y - coeff_r*speed_rotation/100;
// fr_speed = speed_y + coeff_r*speed_rotation/100;
// bl_speed = speed_x - coeff_r*speed_rotation/100;
// br_speed = speed_x + coeff_r*speed_rotation/100;
// 	
// 	
// 	/****************************�Լ��ٶȽ�������(�������ٶ����Ƹ��ٶ�)******************************/
// 	
// 	fl_delta = fl_speed - fl_last;
// 	fr_delta = fr_speed - fr_last;
// 	bl_delta = bl_speed - bl_last;
// 	br_delta = br_speed - br_last;
// 	
// 	delta_max=MaxFour(Abs(fl_delta),Abs(fr_delta),Abs(bl_delta),Abs(br_delta));
// 	
// 	if((delta_max >= Max_Acc) && (Acc_Limit_Enable==1))
// 	{
// 		fl_last += Max_Acc*(fl_delta/delta_max);
// 		fr_last += Max_Acc*(fr_delta/delta_max);
// 		bl_last += Max_Acc*(bl_delta/delta_max);
// 		br_last += Max_Acc*(br_delta/delta_max);
// 	}
// 	else if((delta_max <= -Max_Acc) && (Acc_Limit_Enable==1))
// 	{
// 		fl_last -= Max_Acc*(fl_delta/delta_max);
// 		fr_last -= Max_Acc*(fr_delta/delta_max);
// 		bl_last -= Max_Acc*(bl_delta/delta_max);
// 		br_last -= Max_Acc*(br_delta/delta_max);
// 	}
// 	else
// 	{
// 		fl_last = fl_speed;
// 		fr_last = fr_speed;
// 		bl_last = bl_speed;
// 		br_last = br_speed;
// 	}
// 	
// 	/****************************ת��ת��Ϊռ�ձ�*****************************

// 	             PWM%=(50.0/speed_max)*speed*����ļ��ٱ�+50
// 	
// 	 *************************************************************************/	
// 	fl_speed = fl_last * Mec_FL.arg;
// 	fr_speed = fr_last * Mec_FR.arg;
// 	bl_speed = bl_last * Mec_BL.arg;
// 	br_speed = br_last * Mec_BR.arg;
// 	
// 	PWM_FL = (50.0/6000)*14*fl_speed+50;
// 	PWM_FR = (50.0/6000)*14*fr_speed+50;
// 	PWM_BL = (50.0/6000)*14*bl_speed+50;
// 	PWM_BR = (50.0/6000)*14*br_speed+50;
// 	
// 	/************************�������ٶ��ٴν�������****************************/
// 	
// 	PWM_Max=MaxFour(Abs(PWM_FL-50),Abs(PWM_FR-50),Abs(PWM_BL-50),Abs(PWM_BR-50));
// 	if(PWM_Max >= 48)
// 	{
// 		PWM_FL = 48*(PWM_FL-50)/PWM_Max+50;
// 		PWM_FR = 48*(PWM_FR-50)/PWM_Max+50;
// 		PWM_BL = 48*(PWM_BL-50)/PWM_Max+50;
// 		PWM_BR = 48*(PWM_BR-50)/PWM_Max+50;
// 	}
// 	/************************����Ӧ�Ķ˿ڽ��и�ֵ******************************/

//   PWM_SetDuty(Mec_FL.port,PWM_FL);
// 	PWM_SetDuty(Mec_FR.port,PWM_FR);
// 	PWM_SetDuty(Mec_BL.port,PWM_BL);
// 	PWM_SetDuty(Mec_BR.port,PWM_BR);
// }

// /**********************************END OF FILE*********************************/
