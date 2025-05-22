#include "App_Task.h"

// ���ڽ���bin�ļ�,��A������0x20005000����������������ͻ
u8 receive_buff[buff_size] __attribute__((at(0X20005000)));

/*����������*/
static TaskHandle_t IMUTask_Create;
static TaskHandle_t LengTask_Create;
static TaskHandle_t MotorFwdTask_Create;
static TaskHandle_t MotorSerTask_Create;
static TaskHandle_t IAP_Task_Create;
/*�����ź���*/
static SemaphoreHandle_t LCD_mutex;
static SemaphoreHandle_t MotorGear;
static SemaphoreHandle_t IAP;
static QueueHandle_t LengQueue;

/*����ǰ��*/
static void IMU_Task(void *pvParameters);
static void Leng_Task(void *pvParameters);
static void MotorFwd_Task(void *pvParameters);
static void MotorSer_Task(void *pvParameters);
static void IAP_Task(void *pvParameters);

/*ȫ�ֱ���*/
u8 Gear = 5; // ��ʼ��λΪ0

/**
 * @brief ��������Ӧ������
 */
void AppTasks_Create(void)
{
    HCSR04_Init();                                              // ��������ʼ��
    DMA_Config((u32)&USART1->DR, (u32)receive_buff, buff_size); // DMA��ʼ��
    /*������Ϣ����*/
    LengQueue = xQueueCreate(1, sizeof(u16));
    /*������λ�����ź���*/
    MotorGear = xSemaphoreCreateCounting(5, 5);
    /*��������������LCD��ʾ��Դ*/
    LCD_mutex = xSemaphoreCreateMutex();
    /*������ֵ�ź���*/
    IAP = xSemaphoreCreateBinary();
    /*������̬��������*/
    xTaskCreate(IMU_Task, "IMU_Task", 64, NULL, 2, &IMUTask_Create);
    /*�����������*/
    xTaskCreate(Leng_Task, "Leng_Task", 64, NULL, 1, &LengTask_Create);
    /*���������ת����*/
    xTaskCreate(MotorFwd_Task, "MotorFwd_Task", 64, NULL, 3, &MotorFwdTask_Create);
    /*���������ת����*/
    xTaskCreate(MotorSer_Task, "MotorSer_Task", 64, NULL, 4, &MotorSerTask_Create);
    /*����IAP��������*/
    xTaskCreate(IAP_Task, "IAP_Task", 512, NULL, 5, &IAP_Task_Create);
}

/**
 * @brief IAP��������
 *
 * @param pvParameters
 */
static void IAP_Task(void *pvParameters)
{
    u16 ReceiveLength;
    while (1)
    {
        xSemaphoreTake(IAP, portMAX_DELAY);
        printf("ִ�е�����\r\n"); // ������
        ReceiveLength = GetReceivedDataLength();
        if (receive_buff[0]) // ���Ƿ���յ���APP����
        {
            printf("�ѽ��յ�APP����Ϊ%d\n", ReceiveLength);
            if (((*(volatile uint32_t *)(0X20005000 + 4)) & 0xFF000000) == 0x08000000)
            {
                xSemaphoreTake(LCD_mutex, portMAX_DELAY);                   // ��ȡ������
                Show_Str(0, 120, BLUE, WHITE, "Firmware updating!", 16, 0); /*��ӡ��Ϣ��LCD��Ļ�ϴ������ڸ���*/
                xSemaphoreGive(LCD_mutex);                                  // �ͷŻ�����
                FLASH_ErasePage(FLASH_APP1_ADDR);
                IAP_Write(FLASH_APP1_ADDR, receive_buff, buff_size); // ����FLASH����
                printf("�̼��������!\r\n");
                printf("��ʼִ��flash�û�����\r\n");
                if (((*(volatile uint32_t *)(FLASH_APP1_ADDR + 4)) & 0xFF000000) == 0x08000000)
                {
                    IAP_LOAD(FLASH_APP1_ADDR); // ִ��FLASH APP����
                }
                else
                {
                    FLASH_ErasePage(FLASH_APP1_ADDR);
                    memset(receive_buff, 0, buff_size); // �������
                    DMA_Enable();                       // ����һ��DMA���䣬�����´θ���
                    printf("��FLASHӦ�ó���,�޷�ִ��!\r\n");
                }
            }
            else
            {
                memset(receive_buff, 0, buff_size); // �������
                FLASH_ErasePage(FLASH_APP1_ADDR);
                DMA_Enable(); // ����һ��DMA���䣬�����´θ���
                printf("error!�̼��ж�ʧ��");
                xSemaphoreTake(LCD_mutex, portMAX_DELAY);                   // ��ȡ������
                Show_Str(0, 120, BLUE, WHITE, "Firmware updating!", 16, 0); /*��ӡ��Ϣ��LCD��Ļ�ϴ������ڸ���*/
                xSemaphoreGive(LCD_mutex);                                  // �ͷŻ�����
            }
        }
    }
}

/**
 * @brief �����ת
 *
 * @param pvParameters
 */
static void MotorFwd_Task(void *pvParameters)
{
    while (1)
    {
        if (Key_Scan(KEY1_INT_GPIO_PORT, KEY1_INT_GPIO_PIN) == KEY_ON)
        {
            xSemaphoreTake(MotorGear, 0);
            if (--Gear < 1)
                Gear = 1;
            xSemaphoreTake(LCD_mutex, portMAX_DELAY); // ��ȡ������
            Show_Str(0, 100, BLUE, WHITE, "Gear:", 16, 0);
            LCD_ShowNum(40, 100, 5 - Gear, 2, 16);
            xSemaphoreGive(LCD_mutex); // �ͷŻ�����
        }
        Delay_ms(20);
    }
}

/**
 * @brief �����ת
 *
 * @param pvParameters
 */
static void MotorSer_Task(void *pvParameters)
{
    while (1)
    {
        if (Key_Scan(KEY2_INT_GPIO_PORT, KEY2_INT_GPIO_PIN) == KEY_ON)
        {
            xSemaphoreGive(MotorGear);
            if (++Gear > 5)
                Gear = 5;
            xSemaphoreTake(LCD_mutex, portMAX_DELAY); // ��ȡ������
            Show_Str(0, 100, BLUE, WHITE, "Gear:", 16, 0);
            LCD_ShowNum(40, 100, 5 - Gear, 2, 16);
            xSemaphoreGive(LCD_mutex); // �ͷŻ�����
        }
        Delay_ms(20);
    }
}

/**
 * @brief �������
 *
 * @param pvParameters
 */
static void Leng_Task(void *pvParameters)
{
    while (1)
    {
        HCSR04_Start(); // ��������
        xQueueReceive(LengQueue, &HCSR04_Flag.capValue, pdMS_TO_TICKS(100));
        float distance = HCSR04_Flag.capValue * 0.034 / 2;
        float res = Move_Filter(distance);        // �����˲�����
        xSemaphoreTake(LCD_mutex, portMAX_DELAY); // ��ȡ������
        Show_Str(0, 60, BLUE, WHITE, "Len:    cm", 16, 0);
        LCD_ShowNum(40, 60, res, 3, 16);
        xSemaphoreGive(LCD_mutex); // �ͷŻ�����
        Delay_ms(100);
    }
}

/**
 * @brief ��̬��������
 */
static void IMU_Task(void *pvParameters)
{
    while (1)
    {
        //  ��ȡ����������
        IMU(Acel, Gyro, &pitch, &roll, &yaw);
        // ������ʾ�߼�
        int row = roll * 100;
        if (row < 0)
        {
            row = -row;
            xSemaphoreTake(LCD_mutex, portMAX_DELAY); // ��ȡ������
            Show_Str(0, 40, BLUE, WHITE, "angle:-  .  d", 16, 0);
            LCD_ShowNum(56, 40, row / 100, 2, 16);
            LCD_ShowNum(80, 40, row % 100, 2, 16);
            xSemaphoreGive(LCD_mutex); // �ͷŻ�����
        }
        else
        {
            xSemaphoreTake(LCD_mutex, portMAX_DELAY); // ��ȡ������
            Show_Str(0, 40, BLUE, WHITE, "angle:  .  d", 16, 0);
            LCD_ShowNum(48, 40, row / 100, 2, 16);
            LCD_ShowNum(72, 40, row % 100, 2, 16);
            xSemaphoreGive(LCD_mutex); // �ͷŻ�����
        }
        // ����쳣״̬
        if (row > 3000)
        {
            LED1_ON;
            xSemaphoreTake(LCD_mutex, portMAX_DELAY); // ��ȡ������
            Show_Str(0, 20, BLUE, WHITE, "Caution!", 16, 0);
            xSemaphoreGive(LCD_mutex); // �ͷŻ�����
        }
        else
        {
            LED1_OFF;
            xSemaphoreTake(LCD_mutex, portMAX_DELAY); // ��ȡ������
            LCD_Fill(0, 0, 128, 35, WHITE);
            xSemaphoreGive(LCD_mutex); // �ͷŻ�����
        }
        Delay_ms(100);
    }
}

/**
 * @brief �����������
 *
 */
void TIM1_UP_IRQHandler(void)
{
    if (TIM_GetITStatus(TIM1, TIM_IT_Update)) // ���������������
    {
        HCSR04_Flag.capPeriods++; // �����������
        TIM_ClearITPendingBit(TIM1, TIM_IT_Update);
    }
}

/**
 * @brief ��ʱ���жϷ���,��ⷵ��trig��������󷵻ص��ź�
 *
 */
void TIM1_CC_IRQHandler(void)
{
    BaseType_t pxHigherPriorityTaskWoken = pdFALSE;
    if (TIM_GetITStatus(TIM1, TIM_IT_CC1) != RESET)
    {
        if (HCSR04_Flag.capStartFlag == 0) // �ϱ��ؼ��
        {
            TIM_SetCounter(TIM1, 0); // ��ʱ������
            HCSR04_Flag.capPeriods = 0;
            HCSR04_Flag.capValue = 0;
            TIM_OC1PolarityConfig(TIM1, TIM_ICPolarity_Falling); // ��⵽�ϱ��غ�,�������Ը�Ϊ�±��ؼ��
            HCSR04_Flag.capStartFlag = 1;
        }
        else // �±��ؼ��
        {
            HCSR04_Flag.capValue = TIM_GetCapture1(TIM1); // ��ȡecho���յ��źŵ�ʱ��

            /*�������*һ���������ڵ���+�ߵ�ƽʱ��,Ϊ�˷�ֹ������������������*/
            HCSR04_Flag.capValue = HCSR04_Flag.capPeriods * (0xff + 1) + (HCSR04_Flag.capValue + 1);

            /*�����ʱ�䴫�����㺯��*/
            xQueueSendFromISR(LengQueue, &HCSR04_Flag.capValue, &pxHigherPriorityTaskWoken);

            TIM_OC1PolarityConfig(TIM1, TIM_ICPolarity_Rising); // ��⵽�±��غ�,�������Ը�Ϊ�ϱ��ؼ��
            HCSR04_Flag.capStartFlag = 0;                       // ��ʼ��־��Ϊ0,Ϊ��һ�β�����׼��
        }
    }
    portYIELD_FROM_ISR(pxHigherPriorityTaskWoken);
    TIM_ClearITPendingBit(TIM1, TIM_IT_CC1);
}

/**
 * @brief DMA�ж�,���ڽ���App����
 *
 */
void DMA1_Channel5_IRQHandler(void)
{
    BaseType_t xHigherPriorityTaskWoken = pdFALSE; /*�ͷŶ�ֵ�ź��������������ʼ��ΪpdFalse*/
    // ���DMA��������ж�
    if (DMA_GetITStatus(DMA1_IT_TC5))
    {
        xSemaphoreGiveFromISR(IAP, &xHigherPriorityTaskWoken);
        // ����жϱ�־
        DMA_ClearITPendingBit(DMA1_IT_TC5);
    }
    /* �����Ҫ�Ļ�����һ�������л� */
    portYIELD_FROM_ISR(xHigherPriorityTaskWoken);
}