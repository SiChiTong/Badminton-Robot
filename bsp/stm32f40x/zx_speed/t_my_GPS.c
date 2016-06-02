#include "includes.h"
#define PI 3.1415926

struct Pointfp64
{
	fp64 x;
	fp64 y;
};

struct Gps_State
{   
	struct Pointfp64  	position;	  	  //��ǰ����	
	fp64				distancestep1; 	//����1����ǰ������
	fp64				distancestep2;  //����2����ǰ������
	fp64			  distance1;	  	//����1ǰ������
	fp64			  distance2;		  //����2ǰ������
	fp64  	    angle;        	//��ǰ�Ƕ� 		
	fp64				radian;			    //��ǰ����
	fp64				speed;			    //��ǰ�ٶ�
	fp64				acceleration;	  //��ǰ���ٶ�
	fp64			 	rotationspeed;	//��ǰ��ת�ٶ�
	fp64				rotationacc;	  //��ǰ��ת���ٶ�
};
struct Encoder_Stat
{	
	int64_t Now;
	
	int64_t Last;
	
	int64_t Total;
	
	fp64 Convert1;		//��תϵ��
	
	fp64 Convert2;		//��תϵ��
	
	u8 dir;		//�ı�����ת�����������Ҫ�ķ�������1��������-1��
	
	fp64 Distance;
	
	fp64  radian;
	
	fp64 Radius;
	
};

static struct Pointfp64 my_GPS;
static char thread_my_GPS_stack[1024];
struct rt_thread thread_my_GPS;


struct Encoder_Stat Encoders[2];
struct Gps_State Gps_List[2];
/*
 * ������: rt_thread_entry_my_GPS
 * ��  ��: ��ʵ���ٶ��̵߳���ں���
 * ��  ��: ��
 * ��  ��: ��
 * ��  ��: �ڲ�����
 */
static void rt_thread_entry_my_GPS(void* parameter)
{
	fp64 radian;
	fp64 radianstep = 0.0;						//���Ȳ���ֵ
	fp64 localx;
	fp64 localy;
  
	while(1)
	{
		Delay_ms(5);
		Gps_List[1] = Gps_List[0];
	
		Gps_List[0].angle = MISO_GYRO_RAD.f64_data;//��ԭʼֵ//�Ƕȷ�ΧΪ-180~180

		//ת��Ϊ����ֵ��������-PI~PI
		Gps_List[0].radian=Gps_List[0].angle*PI/180.0;

	
		//ȡ��һʱ��������ʱ�̵Ľ�ƽ������Ϊ��ʱ���ڳ���ĽǶȣ�ǰ�����ж���Ϊ�˽����PI�ֽ��ߴ���ͻ������
		radian=(Gps_List[0].radian+Gps_List[1].radian)/2;
		if(radian>PI)
			radian-=2*PI;
		if(radian<-PI)
			radian+=2*PI;
		
		Encoders[0].radian=MOSI_ENC1_RAD.f64_data;
		Encoders[1].radian=MOSI_ENC2_RAD.f64_data;
		Encoders[0].Radius=MOSI_ENC1_RAD.f64_data;
		Encoders[1].Radius=MOSI_ENC2_RAD.f64_data;
		
	//��û��Ȳ���ֵ
		radianstep=Gps_List[0].radian-Gps_List[1].radian;
		if(radianstep>PI)
			radianstep-=2*PI;
		if(radianstep<-PI)
			radianstep+=2*PI;
		
		
		Encoders[0].Distance=MISO_ENC1_DIS.f64_data;
		Encoders[1].Distance=MISO_ENC2_DIS.f64_data;
	
			
		Gps_List[0].distance1 = Encoders[0].Distance;
		Gps_List[0].distance2 = Encoders[1].Distance;
		
		Gps_List[0].distancestep1 = Gps_List[0].distance1 - Gps_List[1].distance1 - Encoders[0].Radius*radianstep;
		Gps_List[0].distancestep2 = Gps_List[0].distance2 - Gps_List[1].distance2 - Encoders[1].Radius*radianstep; 
	
			
		localx=(-Gps_List[0].distancestep1*cos(Encoders[1].radian)+Gps_List[0].distancestep2*cos(Encoders[0].radian))/sin(Encoders[0].radian+Encoders[1].radian);
		localy=(Gps_List[0].distancestep1*sin(Encoders[1].radian)+Gps_List[0].distancestep2*sin(Encoders[0].radian))/sin(Encoders[0].radian+Encoders[1].radian);
		
		Gps_List[0].position.x+=localx*cos(radian)-localy*sin(radian);
		Gps_List[0].position.y+=localx*sin(radian)+localy*cos(radian);
		my_GPS.x=Gps_List[0].position.x;
		my_GPS.y=Gps_List[0].position.y;
	
	}	
	
}
 

/*
 * ������: rt_thread_my_GPS_init
 * ��  ��: my_GPSˢ���̵߳ĳ�ʼ��
 * ��  ��: ��
 * ��  ��: ��ʼ�����
 * ��  ��: �ⲿ����
 */
int rt_thread_my_GPS_init(void)
{
	rt_err_t result;

    
	result = rt_thread_init(&thread_my_GPS, 
                            "thread_my_GPS",
                            rt_thread_entry_my_GPS,
                            RT_NULL,
                            &thread_my_GPS_stack[0], sizeof(thread_my_GPS_stack),
							5, 5);
    if (result == RT_EOK) 
        rt_thread_startup(&thread_my_GPS);
    else
        return RT_ERROR;

    return RT_EOK;
}

/********************************    END OF FILE    ***************************/
