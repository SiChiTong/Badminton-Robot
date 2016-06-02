#include "includes.h"

ALIGN(RT_ALIGN_SIZE)
static char thread_arm_stack[2048];
struct rt_thread thread_arm;


/*
 * ������: rt_thread_entry_arm
 * ��  ��: �Զ������ϲ㶯������ں���
 * ��  ��: ��
 * ��  ��: ��
 * ��  ��: �ڲ�����
 */
static void rt_thread_entry_arm(void* parameter)
{
    rt_uint32_t msg;

    while (1)
    {
			if(rt_mb_recv(&Mb_Arm, &msg, RT_WAITING_FOREVER) == RT_EOK)
			{
			switch(msg)
			{
				case 1:
					armaction1();
					break;
				case 2:
					armaction2();
					break;
				case 3:
					armaction3();
					break;
				case 4:
					armaction4();
					break;
				case 5:
					armaction5();
					break;
				case 6:
					armaction6();
					break;
				case 7:
					armaction7();
					break;
			}
		}
    }
}

/*
 * ������: rt_thread_arm_init
 * ��  ��: �Զ������ϲ㶯���ĳ�ʼ��
 * ��  ��: ��
 * ��  ��: ��ʼ�����
 * ��  ��: �ⲿ����
 */
int rt_thread_arm_init(void)
{
    rt_err_t result;
    
	result = rt_thread_init(&thread_arm, 
                            "thread_arm",
                            rt_thread_entry_arm,
                            RT_NULL,
                            &thread_arm_stack[0], sizeof(thread_arm_stack),
							8, 5);
    if (result == RT_EOK)
        rt_thread_startup(&thread_arm);
    else
        return RT_ERROR;

    return RT_EOK;
}

/********************************    END OF FILE    ***************************/
