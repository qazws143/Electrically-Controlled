#include "Hall.h"

void Hall_Init(void)
{
    GPIO_InitTypeDef GPIO_InitStruct;
    ADC_InitTypeDef ADC_InitStruct;
    RCC_APB2PeriphClockCmd(HALL_CLK, ENABLE);
    RCC_ADCCLKConfig(RCC_PCLK2_Div8);

    /*GPIO��ʼ��*/
    GPIO_InitStruct.GPIO_Mode = GPIO_Mode_AIN;
    GPIO_InitStruct.GPIO_Pin = HALL_PIN;
    GPIO_InitStruct.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(HALL_PORT, &GPIO_InitStruct);

    /*ADC��ʼ��*/
    ADC_DeInit(HALL_ADC);                                            // ��λADC1
    ADC_InitStruct.ADC_ContinuousConvMode = ENABLE;                  // ����ת��ģʽ
    ADC_InitStruct.ADC_DataAlign = ADC_DataAlign_Right;              // ���뷽ʽѡ���Ҷ���
    ADC_InitStruct.ADC_ExternalTrigConv = ADC_ExternalTrigConv_None; // �ⲿ����Դѡ���������
    ADC_InitStruct.ADC_Mode = ADC_Mode_Independent;                  // ADC1�����ڶ���ģʽ
    ADC_InitStruct.ADC_NbrOfChannel = 1;                             // ѡ��ADCͨ������
    ADC_InitStruct.ADC_ScanConvMode = DISABLE;                       // ģ��ת�������ڵ�ͨ��ģʽ
    ADC_Init(HALL_ADC, &ADC_InitStruct);

    ADC_RegularChannelConfig(HALL_ADC, ADC_Channel_11, 1, ADC_SampleTime_55Cycles5); // ���� ADC ͨ��ת��˳��Ͳ���ʱ��

    ADC_Cmd(HALL_ADC, ENABLE);
    // ��ʼ��ADC У׼�Ĵ���
    ADC_ResetCalibration(HALL_ADC);
    // �ȴ�У׼�Ĵ�����ʼ�����
    while (ADC_GetResetCalibrationStatus(HALL_ADC))
        ;
    // ADC��ʼУ׼
    ADC_StartCalibration(HALL_ADC);
    // �ȴ�У׼���
    while (ADC_GetCalibrationStatus(HALL_ADC))
        ;

    ADC_SoftwareStartConvCmd(HALL_ADC, ENABLE); // �������ת��
}

u16 Get_ADC(void)
{
    while (!ADC_GetFlagStatus(ADC1, ADC_FLAG_EOC))
        ;                                    // �ȴ�ת������
    return ADC_GetConversionValue(HALL_ADC); // ��õ�ѹ�ɼ����
}