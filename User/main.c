#include "../RtosTask/App_Task.h"

/**
 * @brief ������
 *
 * @return int
 */
int main(void)
{
  NVIC_PriorityGroupConfig(NVIC_PriorityGroup_4);
  Delay_init();           // FreeRTOS��ʱ��ʼ��,һ��Ҫ������λ��ʼ��
  TIM2_Init(4999, 14399); // ��ʼ��PWM
  LCD_Init();             // ��Ļ��ʼ��
  LED_GPIO_Config();      // LED�Ƴ�ʼ��
  Key_GPIO_Config();      // ������ʼ��
  USART_Config();         // ���ڳ�ʼ��
  KalMan_Init();          // �˲��㷨��ʼ��
  MPU6050_Init();         // MPU6050��ʼ��
  Hall_Init();            // ������������ʼ��

  /*��������*/
  AppTasks_Create();
  /*����RTOS�������*/
  vTaskStartScheduler();
}
