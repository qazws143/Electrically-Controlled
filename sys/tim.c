#include "tim.h"

/*��ʼ�����*/
void TIM2_Init(u16 arr, u16 psc)
{
    TIM_TimeBaseInitTypeDef TIM_TimeBaseStructure;
    TIM_OCInitTypeDef TIM_OCInitStructure;
    GPIO_InitTypeDef GPIO_InitStructure;
    RCC_APB1PeriphClockCmd(TIM_CLK, ENABLE);
    RCC_APB2PeriphClockCmd(GPIO_CLK, ENABLE);

    /*GPIO��ʼ��*/
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP; // �����������
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_1;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(GPIO_PORT, &GPIO_InitStructure);

    /*��ʱ����ʼ��*/
    TIM_TimeBaseStructure.TIM_Period = arr;                     // �Զ���װ�ؼĴ���
    TIM_TimeBaseStructure.TIM_Prescaler = psc;                  // Ԥ��Ƶ
    TIM_TimeBaseStructure.TIM_ClockDivision = 0;                // ����ʱ�ӷָ�
    TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up; // TIM���ϼ���ģʽ
    TIM_TimeBaseInit(TIM_PORT, &TIM_TimeBaseStructure);         // ����TIM_TimeBaseInitStruct��ָ���Ĳ�����ʼ��TIMx��ʱ�������λ

    /*PWM��ʼ��*/
    TIM_Cmd(TIM_PORT, ENABLE);                                    // ʹ��TIMx����
    TIM_OCInitStructure.TIM_OCMode = TIM_OCMode_PWM1;             // �Ƚ����ģʽ
    TIM_OCInitStructure.TIM_OCPolarity = TIM_OCPolarity_High;     // �������
    TIM_OCInitStructure.TIM_OutputState = TIM_OutputState_Enable; // ���ʹ��
    TIM_OC2Init(TIM_PORT, &TIM_OCInitStructure);                  // ����Ƚ�ͨ��1��ʼ��

    TIM_OC2PreloadConfig(TIM_PORT, TIM_OCPreload_Enable); // ʹ��Ԥװ�ؼĴ���
    TIM_Cmd(TIM_PORT, ENABLE);                            // ʹ��TIM2
}

/**
 * @brief �趨����ٶ�
 *
 * @param TIM ʹ�õĶ�ʱ��
 * @param Channel ��ʱ��ͨ��
 * @param duty ռ�ձ�
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
