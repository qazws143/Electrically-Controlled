#include "App_Task.h"

// 用于接收bin文件,将A区放在0x20005000避免与主程序发生冲突
u8 receive_buff[buff_size] __attribute__((at(0X20005000)));

/*创建任务句柄*/
static TaskHandle_t IMUTask_Create;
static TaskHandle_t LengTask_Create;
static TaskHandle_t MotorFwdTask_Create;
static TaskHandle_t MotorSerTask_Create;
static TaskHandle_t IAP_Task_Create;
/*创建信号量*/
static SemaphoreHandle_t LCD_mutex;
static SemaphoreHandle_t MotorGear;
static SemaphoreHandle_t IAP;
static QueueHandle_t LengQueue;

/*函数前置*/
static void IMU_Task(void *pvParameters);
static void Leng_Task(void *pvParameters);
static void MotorFwd_Task(void *pvParameters);
static void MotorSer_Task(void *pvParameters);
static void IAP_Task(void *pvParameters);

/*全局变量*/
u8 Gear = 5; // 初始挡位为0

/**
 * @brief 创建所有应用任务
 */
void AppTasks_Create(void)
{
    HCSR04_Init();                                              // 超声波初始化
    DMA_Config((u32)&USART1->DR, (u32)receive_buff, buff_size); // DMA初始化
    /*创建信息队列*/
    LengQueue = xQueueCreate(1, sizeof(u16));
    /*创建挡位计数信号量*/
    MotorGear = xSemaphoreCreateCounting(5, 5);
    /*创建互斥量保护LCD显示资源*/
    LCD_mutex = xSemaphoreCreateMutex();
    /*创建二值信号量*/
    IAP = xSemaphoreCreateBinary();
    /*创建姿态解算任务*/
    xTaskCreate(IMU_Task, "IMU_Task", 64, NULL, 2, &IMUTask_Create);
    /*创建测距任务*/
    xTaskCreate(Leng_Task, "Leng_Task", 64, NULL, 1, &LengTask_Create);
    /*创建电机正转任务*/
    xTaskCreate(MotorFwd_Task, "MotorFwd_Task", 64, NULL, 3, &MotorFwdTask_Create);
    /*创建电机反转任务*/
    xTaskCreate(MotorSer_Task, "MotorSer_Task", 64, NULL, 4, &MotorSerTask_Create);
    /*创建IAP升级任务*/
    xTaskCreate(IAP_Task, "IAP_Task", 512, NULL, 5, &IAP_Task_Create);
}

/**
 * @brief IAP升级功能
 *
 * @param pvParameters
 */
static void IAP_Task(void *pvParameters)
{
    u16 ReceiveLength;
    while (1)
    {
        xSemaphoreTake(IAP, portMAX_DELAY);
        printf("执行到这里\r\n"); // 调试用
        ReceiveLength = GetReceivedDataLength();
        if (receive_buff[0]) // 看是否接收到了APP代码
        {
            printf("已接收到APP代码为%d\n", ReceiveLength);
            if (((*(volatile uint32_t *)(0X20005000 + 4)) & 0xFF000000) == 0x08000000)
            {
                xSemaphoreTake(LCD_mutex, portMAX_DELAY);                   // 获取互斥量
                Show_Str(0, 120, BLUE, WHITE, "Firmware updating!", 16, 0); /*打印信息在LCD屏幕上代表正在更新*/
                xSemaphoreGive(LCD_mutex);                                  // 释放互斥量
                FLASH_ErasePage(FLASH_APP1_ADDR);
                IAP_Write(FLASH_APP1_ADDR, receive_buff, buff_size); // 更新FLASH代码
                printf("固件更新完成!\r\n");
                printf("开始执行flash用户代码\r\n");
                if (((*(volatile uint32_t *)(FLASH_APP1_ADDR + 4)) & 0xFF000000) == 0x08000000)
                {
                    IAP_LOAD(FLASH_APP1_ADDR); // 执行FLASH APP代码
                }
                else
                {
                    FLASH_ErasePage(FLASH_APP1_ADDR);
                    memset(receive_buff, 0, buff_size); // 清空数组
                    DMA_Enable();                       // 开启一次DMA传输，用于下次更新
                    printf("非FLASH应用程序,无法执行!\r\n");
                }
            }
            else
            {
                memset(receive_buff, 0, buff_size); // 清空数组
                FLASH_ErasePage(FLASH_APP1_ADDR);
                DMA_Enable(); // 开启一次DMA传输，用于下次更新
                printf("error!固件判断失败");
                xSemaphoreTake(LCD_mutex, portMAX_DELAY);                   // 获取互斥量
                Show_Str(0, 120, BLUE, WHITE, "Firmware updating!", 16, 0); /*打印信息在LCD屏幕上代表正在更新*/
                xSemaphoreGive(LCD_mutex);                                  // 释放互斥量
            }
        }
    }
}

/**
 * @brief 电机正转
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
            xSemaphoreTake(LCD_mutex, portMAX_DELAY); // 获取互斥量
            Show_Str(0, 100, BLUE, WHITE, "Gear:", 16, 0);
            LCD_ShowNum(40, 100, 5 - Gear, 2, 16);
            xSemaphoreGive(LCD_mutex); // 释放互斥量
        }
        Delay_ms(20);
    }
}

/**
 * @brief 电机反转
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
            xSemaphoreTake(LCD_mutex, portMAX_DELAY); // 获取互斥量
            Show_Str(0, 100, BLUE, WHITE, "Gear:", 16, 0);
            LCD_ShowNum(40, 100, 5 - Gear, 2, 16);
            xSemaphoreGive(LCD_mutex); // 释放互斥量
        }
        Delay_ms(20);
    }
}

/**
 * @brief 距离测量
 *
 * @param pvParameters
 */
static void Leng_Task(void *pvParameters)
{
    while (1)
    {
        HCSR04_Start(); // 触发测量
        xQueueReceive(LengQueue, &HCSR04_Flag.capValue, pdMS_TO_TICKS(100));
        float distance = HCSR04_Flag.capValue * 0.034 / 2;
        float res = Move_Filter(distance);        // 进行滤波处理
        xSemaphoreTake(LCD_mutex, portMAX_DELAY); // 获取互斥量
        Show_Str(0, 60, BLUE, WHITE, "Len:    cm", 16, 0);
        LCD_ShowNum(40, 60, res, 3, 16);
        xSemaphoreGive(LCD_mutex); // 释放互斥量
        Delay_ms(100);
    }
}

/**
 * @brief 姿态解算任务
 */
static void IMU_Task(void *pvParameters)
{
    while (1)
    {
        //  读取传感器数据
        IMU(Acel, Gyro, &pitch, &roll, &yaw);
        // 处理显示逻辑
        int row = roll * 100;
        if (row < 0)
        {
            row = -row;
            xSemaphoreTake(LCD_mutex, portMAX_DELAY); // 获取互斥量
            Show_Str(0, 40, BLUE, WHITE, "angle:-  .  d", 16, 0);
            LCD_ShowNum(56, 40, row / 100, 2, 16);
            LCD_ShowNum(80, 40, row % 100, 2, 16);
            xSemaphoreGive(LCD_mutex); // 释放互斥量
        }
        else
        {
            xSemaphoreTake(LCD_mutex, portMAX_DELAY); // 获取互斥量
            Show_Str(0, 40, BLUE, WHITE, "angle:  .  d", 16, 0);
            LCD_ShowNum(48, 40, row / 100, 2, 16);
            LCD_ShowNum(72, 40, row % 100, 2, 16);
            xSemaphoreGive(LCD_mutex); // 释放互斥量
        }
        // 检测异常状态
        if (row > 3000)
        {
            LED1_ON;
            xSemaphoreTake(LCD_mutex, portMAX_DELAY); // 获取互斥量
            Show_Str(0, 20, BLUE, WHITE, "Caution!", 16, 0);
            xSemaphoreGive(LCD_mutex); // 释放互斥量
        }
        else
        {
            LED1_OFF;
            xSemaphoreTake(LCD_mutex, portMAX_DELAY); // 获取互斥量
            LCD_Fill(0, 0, 128, 35, WHITE);
            xSemaphoreGive(LCD_mutex); // 释放互斥量
        }
        Delay_ms(100);
    }
}

/**
 * @brief 计算溢出次数
 *
 */
void TIM1_UP_IRQHandler(void)
{
    if (TIM_GetITStatus(TIM1, TIM_IT_Update)) // 计算数据溢出次数
    {
        HCSR04_Flag.capPeriods++; // 溢出计数增加
        TIM_ClearITPendingBit(TIM1, TIM_IT_Update);
    }
}

/**
 * @brief 定时器中断服务,检测返回trig发送脉冲后返回的信号
 *
 */
void TIM1_CC_IRQHandler(void)
{
    BaseType_t pxHigherPriorityTaskWoken = pdFALSE;
    if (TIM_GetITStatus(TIM1, TIM_IT_CC1) != RESET)
    {
        if (HCSR04_Flag.capStartFlag == 0) // 上边沿检测
        {
            TIM_SetCounter(TIM1, 0); // 计时器清零
            HCSR04_Flag.capPeriods = 0;
            HCSR04_Flag.capValue = 0;
            TIM_OC1PolarityConfig(TIM1, TIM_ICPolarity_Falling); // 检测到上边沿后,将捕获极性改为下边沿检测
            HCSR04_Flag.capStartFlag = 1;
        }
        else // 下边沿检测
        {
            HCSR04_Flag.capValue = TIM_GetCapture1(TIM1); // 获取echo接收到信号的时间

            /*溢出次数*一个完整周期的数+高电平时间,为了防止过长检测距离带来的误差*/
            HCSR04_Flag.capValue = HCSR04_Flag.capPeriods * (0xff + 1) + (HCSR04_Flag.capValue + 1);

            /*将检测时间传到计算函数*/
            xQueueSendFromISR(LengQueue, &HCSR04_Flag.capValue, &pxHigherPriorityTaskWoken);

            TIM_OC1PolarityConfig(TIM1, TIM_ICPolarity_Rising); // 检测到下边沿后,将捕获极性改为上边沿检测
            HCSR04_Flag.capStartFlag = 0;                       // 开始标志置为0,为下一次捕获做准备
        }
    }
    portYIELD_FROM_ISR(pxHigherPriorityTaskWoken);
    TIM_ClearITPendingBit(TIM1, TIM_IT_CC1);
}

/**
 * @brief DMA中断,用于接收App程序
 *
 */
void DMA1_Channel5_IRQHandler(void)
{
    BaseType_t xHigherPriorityTaskWoken = pdFALSE; /*释放二值信号量所需参数，初始化为pdFalse*/
    // 检查DMA传输完成中断
    if (DMA_GetITStatus(DMA1_IT_TC5))
    {
        xSemaphoreGiveFromISR(IAP, &xHigherPriorityTaskWoken);
        // 清除中断标志
        DMA_ClearITPendingBit(DMA1_IT_TC5);
    }
    /* 如果需要的话进行一次任务切换 */
    portYIELD_FROM_ISR(xHigherPriorityTaskWoken);
}