#include "Hall.h"

void Hall_Init(void)
{
    GPIO_InitTypeDef GPIO_InitStruct;
    ADC_InitTypeDef ADC_InitStruct;
    RCC_APB2PeriphClockCmd(HALL_CLK, ENABLE);
    RCC_ADCCLKConfig(RCC_PCLK2_Div8);

    /*GPIO初始化*/
    GPIO_InitStruct.GPIO_Mode = GPIO_Mode_AIN;
    GPIO_InitStruct.GPIO_Pin = HALL_PIN;
    GPIO_InitStruct.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(HALL_PORT, &GPIO_InitStruct);

    /*ADC初始化*/
    ADC_DeInit(HALL_ADC);                                            // 复位ADC1
    ADC_InitStruct.ADC_ContinuousConvMode = ENABLE;                  // 单次转换模式
    ADC_InitStruct.ADC_DataAlign = ADC_DataAlign_Right;              // 对齐方式选择右对齐
    ADC_InitStruct.ADC_ExternalTrigConv = ADC_ExternalTrigConv_None; // 外部触发源选择软件触发
    ADC_InitStruct.ADC_Mode = ADC_Mode_Independent;                  // ADC1工作在独立模式
    ADC_InitStruct.ADC_NbrOfChannel = 1;                             // 选择ADC通道数量
    ADC_InitStruct.ADC_ScanConvMode = DISABLE;                       // 模数转换工作在单通道模式
    ADC_Init(HALL_ADC, &ADC_InitStruct);

    ADC_RegularChannelConfig(HALL_ADC, ADC_Channel_11, 1, ADC_SampleTime_55Cycles5); // 配置 ADC 通道转换顺序和采样时间

    ADC_Cmd(HALL_ADC, ENABLE);
    // 初始化ADC 校准寄存器
    ADC_ResetCalibration(HALL_ADC);
    // 等待校准寄存器初始化完成
    while (ADC_GetResetCalibrationStatus(HALL_ADC))
        ;
    // ADC开始校准
    ADC_StartCalibration(HALL_ADC);
    // 等待校准完成
    while (ADC_GetCalibrationStatus(HALL_ADC))
        ;

    ADC_SoftwareStartConvCmd(HALL_ADC, ENABLE); // 进行软件转化
}

u16 Get_ADC(void)
{
    while (!ADC_GetFlagStatus(ADC1, ADC_FLAG_EOC))
        ;                                    // 等待转换结束
    return ADC_GetConversionValue(HALL_ADC); // 获得电压采集结果
}