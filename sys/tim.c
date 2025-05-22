#include "tim.h"

/*初始化电机*/
void TIM2_Init(u16 arr, u16 psc)
{
    TIM_TimeBaseInitTypeDef TIM_TimeBaseStructure;
    TIM_OCInitTypeDef TIM_OCInitStructure;
    GPIO_InitTypeDef GPIO_InitStructure;
    RCC_APB1PeriphClockCmd(TIM_CLK, ENABLE);
    RCC_APB2PeriphClockCmd(GPIO_CLK, ENABLE);

    /*GPIO初始化*/
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP; // 复用推挽输出
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_1;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(GPIO_PORT, &GPIO_InitStructure);

    /*定时器初始化*/
    TIM_TimeBaseStructure.TIM_Period = arr;                     // 自动重装载寄存器
    TIM_TimeBaseStructure.TIM_Prescaler = psc;                  // 预分频
    TIM_TimeBaseStructure.TIM_ClockDivision = 0;                // 设置时钟分割
    TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up; // TIM向上计数模式
    TIM_TimeBaseInit(TIM_PORT, &TIM_TimeBaseStructure);         // 根据TIM_TimeBaseInitStruct中指定的参数初始化TIMx的时间基数单位

    /*PWM初始化*/
    TIM_Cmd(TIM_PORT, ENABLE);                                    // 使能TIMx外设
    TIM_OCInitStructure.TIM_OCMode = TIM_OCMode_PWM1;             // 比较输出模式
    TIM_OCInitStructure.TIM_OCPolarity = TIM_OCPolarity_High;     // 输出极性
    TIM_OCInitStructure.TIM_OutputState = TIM_OutputState_Enable; // 输出使能
    TIM_OC2Init(TIM_PORT, &TIM_OCInitStructure);                  // 输出比较通道1初始化

    TIM_OC2PreloadConfig(TIM_PORT, TIM_OCPreload_Enable); // 使能预装载寄存器
    TIM_Cmd(TIM_PORT, ENABLE);                            // 使能TIM2
}

/**
 * @brief 设定电机速度
 *
 * @param TIM 使用的定时器
 * @param Channel 定时器通道
 * @param duty 占空比
 */
void Set_Servo(TIM_TypeDef *TIM, u8 Channel, u8 duty)
{
    u16 pulse = (u16)(duty * 5000 / 100);
    switch (Channel)
    {
    case TIM_Channel_1:
        TIM->CCR1 = pulse;
        break;
    case TIM_Channel_2:
        TIM->CCR2 = pulse;
        break;
    case TIM_Channel_3:
        TIM->CCR3 = pulse;
        break;
    case TIM_Channel_4:
        TIM->CCR4 = pulse;
        break;
    default:
        break;
    }
}
