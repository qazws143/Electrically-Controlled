#include "HCSR04.h"
#include "FreeRTOS.h"
#include "task.h"
#include "delay.h"

capflag HCSR04_Flag;
/**
 * @brief 超声波初始化
 *
 */
void HCSR04_Init(void)
{
    /*GPIO初始化*/
    GPIO_InitTypeDef GPIO_InitStruct;
    RCC_APB2PeriphClockCmd(HCSR04_CLK, ENABLE);
    GPIO_InitStruct.GPIO_Mode = GPIO_Mode_Out_PP; // Trig引脚
    GPIO_InitStruct.GPIO_Pin = HCSR04_TRIG;
    GPIO_InitStruct.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(HCSR04_PORT, &GPIO_InitStruct);

    GPIO_InitStruct.GPIO_Mode = GPIO_Mode_IN_FLOATING; // Echo引脚
    GPIO_InitStruct.GPIO_Pin = HCSR04_ECHO;
    GPIO_InitStruct.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(HCSR04_PORT, &GPIO_InitStruct);

    /*对使用定时器进行初始化*/
    TIM_ICInitTypeDef TIM_ICInitStruct;
    TIM_TimeBaseInitTypeDef TIM_BaseInitStruct;
    NVIC_InitTypeDef NVIC_InitStruct;
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_TIM1, ENABLE);
    TIM_InternalClockConfig(TIM1);

    /*时基单元初始化*/
    TIM_BaseInitStruct.TIM_ClockDivision = 0;                // 时钟不分频
    TIM_BaseInitStruct.TIM_CounterMode = TIM_CounterMode_Up; // 向上计数
    TIM_BaseInitStruct.TIM_Period = 0xFFFF;                  // 自动重装载值
    TIM_BaseInitStruct.TIM_Prescaler = 72 - 1;               // 预分频
    TIM_TimeBaseInit(TIM1, &TIM_BaseInitStruct);

    /*输入捕获初始化*/
    TIM_ICInitStruct.TIM_Channel = TIM_Channel_1;                // 捕获通道
    TIM_ICInitStruct.TIM_ICFilter = 0x0F;                        // 滤波
    TIM_ICInitStruct.TIM_ICPolarity = TIM_ICPolarity_Rising;     // 上升沿捕获
    TIM_ICInitStruct.TIM_ICSelection = TIM_ICSelection_DirectTI; // 映射到通道1
    TIM_ICInitStruct.TIM_ICPrescaler = TIM_ICPSC_DIV1;
    TIM_ICInit(TIM1, &TIM_ICInitStruct);

    /*内部中断初始化*/
    NVIC_InitStruct.NVIC_IRQChannel = TIM1_CC_IRQn; // 设置中断通道为TIM1捕获/比较中断
    NVIC_InitStruct.NVIC_IRQChannelCmd = ENABLE;
    NVIC_InitStruct.NVIC_IRQChannelPreemptionPriority = 5;
    NVIC_InitStruct.NVIC_IRQChannelSubPriority = 0;
    NVIC_Init(&NVIC_InitStruct);

    NVIC_InitStruct.NVIC_IRQChannel = TIM1_UP_IRQn; // 设置中断通道为TIM1的数据溢出更新中断
    NVIC_InitStruct.NVIC_IRQChannelCmd = ENABLE;
    NVIC_InitStruct.NVIC_IRQChannelPreemptionPriority = 6;
    NVIC_InitStruct.NVIC_IRQChannelSubPriority = 0;
    NVIC_Init(&NVIC_InitStruct);
    /*定时器使能*/
    TIM_Cmd(TIM1, ENABLE);
    /*中断使能*/
    TIM_ITConfig(TIM1, TIM_IT_Update | TIM_IT_CC1, ENABLE);
}

/**
 * @brief 开启超声波
 *
 */
void HCSR04_Start(void)
{
    GPIO_SetBits(HCSR04_PORT, HCSR04_TRIG);
    Delay_us(20);
    GPIO_ResetBits(HCSR04_PORT, HCSR04_TRIG);
}
