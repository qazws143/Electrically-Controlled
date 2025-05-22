#include "HCSR04.h"
#include "FreeRTOS.h"
#include "task.h"
#include "delay.h"

capflag HCSR04_Flag;
/**
 * @brief ��������ʼ��
 *
 */
void HCSR04_Init(void)
{
    /*GPIO��ʼ��*/
    GPIO_InitTypeDef GPIO_InitStruct;
    RCC_APB2PeriphClockCmd(HCSR04_CLK, ENABLE);
    GPIO_InitStruct.GPIO_Mode = GPIO_Mode_Out_PP; // Trig����
    GPIO_InitStruct.GPIO_Pin = HCSR04_TRIG;
    GPIO_InitStruct.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(HCSR04_PORT, &GPIO_InitStruct);

    GPIO_InitStruct.GPIO_Mode = GPIO_Mode_IN_FLOATING; // Echo����
    GPIO_InitStruct.GPIO_Pin = HCSR04_ECHO;
    GPIO_InitStruct.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(HCSR04_PORT, &GPIO_InitStruct);

    /*��ʹ�ö�ʱ�����г�ʼ��*/
    TIM_ICInitTypeDef TIM_ICInitStruct;
    TIM_TimeBaseInitTypeDef TIM_BaseInitStruct;
    NVIC_InitTypeDef NVIC_InitStruct;
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_TIM1, ENABLE);
    TIM_InternalClockConfig(TIM1);

    /*ʱ����Ԫ��ʼ��*/
    TIM_BaseInitStruct.TIM_ClockDivision = 0;                // ʱ�Ӳ���Ƶ
    TIM_BaseInitStruct.TIM_CounterMode = TIM_CounterMode_Up; // ���ϼ���
    TIM_BaseInitStruct.TIM_Period = 0xFFFF;                  // �Զ���װ��ֵ
    TIM_BaseInitStruct.TIM_Prescaler = 72 - 1;               // Ԥ��Ƶ
    TIM_TimeBaseInit(TIM1, &TIM_BaseInitStruct);

    /*���벶���ʼ��*/
    TIM_ICInitStruct.TIM_Channel = TIM_Channel_1;                // ����ͨ��
    TIM_ICInitStruct.TIM_ICFilter = 0x0F;                        // �˲�
    TIM_ICInitStruct.TIM_ICPolarity = TIM_ICPolarity_Rising;     // �����ز���
    TIM_ICInitStruct.TIM_ICSelection = TIM_ICSelection_DirectTI; // ӳ�䵽ͨ��1
    TIM_ICInitStruct.TIM_ICPrescaler = TIM_ICPSC_DIV1;
    TIM_ICInit(TIM1, &TIM_ICInitStruct);

    /*�ڲ��жϳ�ʼ��*/
    NVIC_InitStruct.NVIC_IRQChannel = TIM1_CC_IRQn; // �����ж�ͨ��ΪTIM1����/�Ƚ��ж�
    NVIC_InitStruct.NVIC_IRQChannelCmd = ENABLE;
    NVIC_InitStruct.NVIC_IRQChannelPreemptionPriority = 5;
    NVIC_InitStruct.NVIC_IRQChannelSubPriority = 0;
    NVIC_Init(&NVIC_InitStruct);

    NVIC_InitStruct.NVIC_IRQChannel = TIM1_UP_IRQn; // �����ж�ͨ��ΪTIM1��������������ж�
    NVIC_InitStruct.NVIC_IRQChannelCmd = ENABLE;
    NVIC_InitStruct.NVIC_IRQChannelPreemptionPriority = 6;
    NVIC_InitStruct.NVIC_IRQChannelSubPriority = 0;
    NVIC_Init(&NVIC_InitStruct);
    /*��ʱ��ʹ��*/
    TIM_Cmd(TIM1, ENABLE);
    /*�ж�ʹ��*/
    TIM_ITConfig(TIM1, TIM_IT_Update | TIM_IT_CC1, ENABLE);
}

/**
 * @brief ����������
 *
 */
void HCSR04_Start(void)
{
    GPIO_SetBits(HCSR04_PORT, HCSR04_TRIG);
    Delay_us(20);
    GPIO_ResetBits(HCSR04_PORT, HCSR04_TRIG);
}
