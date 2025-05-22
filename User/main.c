#include "../RtosTask/App_Task.h"

/**
 * @brief 主函数
 *
 * @return int
 */
int main(void)
{
  NVIC_PriorityGroupConfig(NVIC_PriorityGroup_4);
  Delay_init();           // FreeRTOS延时初始化,一定要放在首位初始化
  TIM2_Init(4999, 14399); // 初始化PWM
  LCD_Init();             // 屏幕初始化
  LED_GPIO_Config();      // LED灯初始化
  Key_GPIO_Config();      // 按键初始化
  USART_Config();         // 串口初始化
  KalMan_Init();          // 滤波算法初始化
  MPU6050_Init();         // MPU6050初始化
  Hall_Init();            // 霍尔传感器初始化

  /*创建函数*/
  AppTasks_Create();
  /*启动RTOS任务调度*/
  vTaskStartScheduler();
}
