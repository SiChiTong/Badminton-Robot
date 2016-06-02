
/* Includes ------------------------------------------------------------------*/
#include "includes.h"
#include "math.h"
#define ACC 500
#define Acc_Limit_Enable 0
double RATE = 0.14297486;

int Swe_Acc = 6;//���ٶ�����
int Swe_Acc_d = 50;//���ټ��ٶ�����

//���������ϵ�ǻ����˵ľֲ�����ϵ
double Speed_X;				// mm/s
double Speed_Y;				// mm/s
double Speed_Rotation;		// (rad/s)/100

extern double coeff_x,coeff_y,coeff_r; //�ſ˱Ⱦ��󻯼���ϵ��


struct Swedish_State Swe_FL={3, -1};		//���ֵ�״̬����
struct Swedish_State Swe_FR={2, 1};
struct Swedish_State Swe_BL={1, -1};
struct Swedish_State Swe_BR={4, 1};

/*
 * ������: Swedish_test
 * ��  ��: ���ӵ���
 * ��  ��: ����״̬��Ϣ
 * ��  ��: ��
 * ��  ��: �ⲿ����
 */
void Swedish_test(struct Swedish_State *swe)
{
	rt_uint32_t key_value;
	double speed_pwm=50;

/*->*/swe_test:
	while(1)
	{
		LCD_Clear();
		LCD_SetXY(0,0);
		LCD_WriteString("======Swedish=======");
		LCD_SetXY(0,1);
		LCD_WriteString("1.Speed");
		LCD_SetXY(0,2);
		LCD_WriteString("2.ChangeArg");
		LCD_SetXY(0,3);
		LCD_WriteString("3.ChangePort now:");
		LCD_WriteInt((*swe).port);

		
		if(rt_mb_recv(&Mb_Key, &key_value, RT_WAITING_NO) == RT_EOK)
		{
			switch(key_value)
			{
				case key1:
					goto swe_speed;
				case key2:
					Input_IntValue(&(*swe).arg,"Argument");
					break;
				case key3:
					Input_IntValue(&(*swe).port,"Port");
					break;

				case keyback:
					return;
			}
		}
		Delay_ms(10);
	}
	
/*->*/swe_speed:
	while(1)
	{
		LCD_Clear();
		LCD_SetXY(0,0);
		LCD_WriteString("======Swedish=======");
		LCD_SetXY(0,1);
		LCD_WriteString("Speed_PWM:");
		LCD_WriteDouble(PWM_GetDuty((*swe).port));
		LCD_SetXY(0,2);
		LCD_WriteString("1.SpeedUp");
		LCD_SetXY(0,3);
		LCD_WriteString("2.SpeedDown");
		LCD_SetXY(0,4);
		LCD_WriteString("3.Stop");
		
		if(rt_mb_recv(&Mb_Key, &key_value, RT_WAITING_NO) == RT_EOK)
		{
			switch(key_value)
			{
				case key1:
					if(speed_pwm<100)
						speed_pwm+=1;
					break;
				case key2:
					if(speed_pwm>0)
						speed_pwm-=1;
					break;
				case key3:
					speed_pwm=50;
					break;
				case keyback:
					goto swe_test;
			}
			if(key_value == keyback)
			{
				PWM_SetDuty((*swe).port,50);
				break;
			}
			PWM_SetDuty((*swe).port,speed_pwm);
		}
		Delay_ms(10);
	}
}


/*
 * ������: SetSpeed
 * ��  ��: Ϊ�����˸��ٶ�
 * ��  ��: ����������ϵX�����ٶ�(mm/s)��Y�����ٶ�(mm/s)����ת�ٶ�(rad/s)/100
 * ��  ��: ��
 * ��  ��: �ⲿ����
 */

//void SetSpeed(double fl_speed,double fr_speed,double bl_speed,double br_speed)
void SetSpeed(double speed_x,double speed_y,double speed_rotation)
{
	double fl_speed;
	double fr_speed;
	double bl_speed;
	double br_speed;
	
	double fl_delta;
	double fr_delta;
	double bl_delta;
	double br_delta;
	
	static double fl_last=0;	//�������ֵ��ٶ�
	static double fr_last=0;
	static double bl_last=0;
	static double br_last=0;
	
	double delta_max;			//�ٶȱ仯�����ֵ
	double PWM_Max;				//���ɵ�PWM���Ӧ���ٶ����ֵ
	
	double PWM_FL,PWM_FR,PWM_BL,PWM_BR;
//*********************anti-slip*******************//
	double speed_x_rep,speed_y_rep,speed_rep;
	double rate_x,rate_y;
	
	speed_x_rep=speed_x;
	speed_y_rep=speed_y;
	speed_rep=sqrt(pow(speed_x_rep,2)+pow(speed_y_rep,2));
	
	if(speed_rep!=0)
	{
		rate_x=speed_x_rep/speed_rep;
		rate_y=speed_y_rep/speed_rep;
	}
	else
	{
		rate_x=1;
		rate_y=1;
	}
	
	if(speed_rep-realspe>=0)
	{
		if(speed_rep-realspe>=ACC)
		{
			speed_x=(realspe+ACC)*rate_x;
			speed_y=(realspe+ACC)*rate_y;
		}
		else
		{	
			speed_x=speed_x_rep;
			speed_y=speed_y_rep;
		}	
	}
	else
	{
		if(realspe<=3000)
		{
			if(realspe<2000)
			{
				if(realspe<=100)
					speed_rep=0;
				else
					speed_rep=100;
			}
			else
			speed_rep=2000;
		}
		else 
			speed_rep=3000;
	}
	
	/****************************ͨ���ſ˱Ⱦ�����ٶȷֽ⵽��������******************************/
/*������*/
	coeff_x=coeff_y=0.70684;
	coeff_r=39.07974580567134;
	fl_speed =  coeff_x*speed_x + coeff_y*speed_y - coeff_r*speed_rotation/100 ;
	fr_speed = -coeff_x*speed_x + coeff_y*speed_y + coeff_r*speed_rotation/100 ;
	bl_speed = -coeff_x*speed_x + coeff_y*speed_y - coeff_r*speed_rotation/100 ;
	br_speed =  coeff_x*speed_x + coeff_y*speed_y + coeff_r*speed_rotation/100 ;
/*��ת45��*/
// fl_speed = speed_y - coeff_r*speed_rotation/100;
// fr_speed = speed_y + coeff_r*speed_rotation/100;
// bl_speed = speed_x - coeff_r*speed_rotation/100;
// br_speed = speed_x + coeff_r*speed_rotation/100;

	/****************************�Լ��ٶȽ�������(�������ٶ����Ƹ��ٶ�)******************************/
	
	fl_delta = fl_speed - fl_last;
	fr_delta = fr_speed - fr_last;
	bl_delta = bl_speed - bl_last;
	br_delta = br_speed - br_last;
	
	delta_max=MaxFour(Abs(fl_delta),Abs(fr_delta),Abs(bl_delta),Abs(br_delta));
	
	if((delta_max >= Swe_Acc) && (Acc_Limit_Enable==1))
	{
		fl_last += Swe_Acc*(fl_delta/delta_max);
		fr_last += Swe_Acc*(fr_delta/delta_max);
		bl_last += Swe_Acc*(bl_delta/delta_max);
		br_last += Swe_Acc*(br_delta/delta_max);
	}
	else if((-delta_max <= -Swe_Acc_d) && (Acc_Limit_Enable==1))
	{
		fl_last -= Swe_Acc_d*(fl_delta/delta_max);
		fr_last -= Swe_Acc_d*(fr_delta/delta_max);
		bl_last -= Swe_Acc_d*(bl_delta/delta_max);
		br_last -= Swe_Acc_d*(br_delta/delta_max);
	}
	else
	{
		fl_last = fl_speed;
		fr_last = fr_speed;
		bl_last = bl_speed;
		br_last = br_speed;
	}
	
	/****************************ת��ת��Ϊռ�ձ�*****************************

	             PWM%=(50.0/speed_max)*speed*����ļ��ٱ�+50
	
	 *************************************************************************/	
	fl_speed = (fl_last * Swe_FL.arg)*RATE;
	fr_speed = (fr_last * Swe_FR.arg)*RATE;
	bl_speed = (bl_last * Swe_BL.arg)*RATE;
	br_speed = (br_last * Swe_BR.arg)*RATE;
	
	PWM_FL = (50.0/6000)*14*fl_speed+50;
	PWM_FR = (50.0/6000)*14*fr_speed+50;
	PWM_BL = (50.0/6000)*14*bl_speed+50;
	PWM_BR = (50.0/6000)*14*br_speed+50;
	
	/************************�������ٶ��ٴν�������****************************/
	
	PWM_Max=MaxFour(Abs(PWM_FL-50),Abs(PWM_FR-50),Abs(PWM_BL-50),Abs(PWM_BR-50));
	if(PWM_Max >= 48)
	{
		PWM_FL = 48*(PWM_FL-50)/PWM_Max+50;
		PWM_FR = 48*(PWM_FR-50)/PWM_Max+50;
		PWM_BL = 48*(PWM_BL-50)/PWM_Max+50;
		PWM_BR = 48*(PWM_BR-50)/PWM_Max+50;
	}
	/************************����Ӧ�Ķ˿ڽ��и�ֵ******************************/

  PWM_SetDuty(Swe_FL.port,PWM_FL);
	PWM_SetDuty(Swe_FR.port,PWM_FR);
	PWM_SetDuty(Swe_BL.port,PWM_BL);
	PWM_SetDuty(Swe_BR.port,PWM_BR);
}

/**********************************END OF FILE*********************************/
