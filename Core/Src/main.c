/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.c
  * @brief          : Main program body
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2023 STMicroelectronics.
  * All rights reserved.
  *
  * This software is licensed under terms that can be found in the LICENSE file
  * in the root directory of this software component.
  * If no LICENSE file comes with this software, it is provided AS-IS.
  *
  ******************************************************************************
  */
/* USER CODE END Header */
/* Includes ------------------------------------------------------------------*/
#include "main.h"
#include "i2c.h"
#include "spi.h"
#include "tim.h"
#include "usart.h"
#include "gpio.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#include "lcd_init.h"
#include "lcd.h"
#include "delay.h"
#include "MPU6050.h"
#include "BMP280.h"
#include "CH455_Library/CH455/ch455.h"
#include "pid.h"
#include "motor.h"

#include <math.h>
#include <string.h>
/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */

/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */

/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/

/* USER CODE BEGIN PV */
static int keyInterrupt1 = 0; //PG6
static int keyInterrupt2 = 0; //PG7
int remote_control_flag=0;

int16_t acceleration_x = 0;  //������ٶ�
int16_t acceleration_y = 0;
int16_t acceleration_z = 0;

int16_t gyro_x = 0;  //����Ǽ��ٶ�
int16_t gyro_y = 0;
int16_t gyro_z = 0;

float pitch, roll, yaw;
float Angle[3] = {0};//���սǶ�,IMU_Update�е��á�

int pwm_duty = 750;//��λ��1us
uint8_t uartRxBuffer[3];//���ڽ��ջ���

uint16_t IC_Val1 = 0;           // input capture value1
int16_t IC_Val2 = 0;           // input capture value2
uint16_t Difference = 0;        // the difference between two captured values
uint8_t Is_First_Captured = 0;  // 0-��δ�����һ��ֵ, 1-�Ѿ������˵�һ��ֵ

extern PID pid_fei;
float Pd_1 = 40;
float Id_1 = 0;
float Dd_1 = 5;
/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
/* USER CODE BEGIN PFP */


void HAL_GPIO_EXTI_Callback(uint16_t GPIO_PIN) {
    //��ȡ�������룬������TIM2���жϻص���������ɶ�duty���޸�
    if (GPIO_PIN == GPIO_PIN_6) {
        keyInterrupt1 = 1;

        __HAL_GPIO_EXTI_CLEAR_FLAG(GPIO_PIN_6);
    }
    if (GPIO_PIN == GPIO_PIN_7) {
        keyInterrupt2 = 1;
        __HAL_GPIO_EXTI_CLEAR_FLAG(GPIO_PIN_7);
    }
}

/*ͨ���ⲿ�ж����޸�ռ�ձ�*/
void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim) {
    if (htim->Instance == TIM2) {
        if (keyInterrupt1 == 1) {
            pwm_duty+=50;
            HAL_GPIO_TogglePin(GPIOB, GPIO_PIN_14);
//            __HAL_TIM_SET_COMPARE(&htim4, TIM_CHANNEL_1, pwm_duty);
//            __HAL_TIM_SET_COMPARE(&htim4, TIM_CHANNEL_2, pwm_duty);
            keyInterrupt1 = 0;
        }
        if (keyInterrupt2 == 1) {
            pwm_duty-=50;
            HAL_GPIO_TogglePin(GPIOB, GPIO_PIN_14);
//            __HAL_TIM_SET_COMPARE(&htim4, TIM_CHANNEL_1, pwm_duty);
//            __HAL_TIM_SET_COMPARE(&htim4, TIM_CHANNEL_2, pwm_duty);
            keyInterrupt2 = 0;
        }
    }
}

void USART1_IRQHandler(void) {
    /* USER CODE BEGIN USART1_IRQn 0 */
    HAL_UART_IRQHandler(&huart1);
    remote_control_flag=1;
    if (HAL_UART_Receive_IT(&huart1, (uint8_t *) uartRxBuffer, 3) == HAL_OK) {
        HAL_UART_Transmit(&huart1, uartRxBuffer, sizeof(uartRxBuffer), 50);
        while (__HAL_UART_GET_FLAG(&huart1, UART_FLAG_TC) != SET);

        if (uartRxBuffer[2] == 69) {
            HAL_GPIO_WritePin(GPIOA, GPIO_PIN_0, GPIO_PIN_SET);
        } else {
            HAL_GPIO_WritePin(GPIOA, GPIO_PIN_0, GPIO_PIN_RESET);
        }
        // memset(uartRxBuffer,0,sizeof(uartRxBuffer));
    }
    /* USER CODE END USART1_IRQn 0 */
    //  HAL_UART_IRQHandler(&huart1);
    /* USER CODE BEGIN USART1_IRQn 1 */

    /* USER CODE END USART1_IRQn 1 */
}
/*���ڽ�����ת����ԭ��*/
int16_t getOriginalNum(int16_t num) {
    if (num & 0x8000)//�ж����λ�Ƿ�Ϊ1��1��ʾ������һ������
    {
        num = ~num + 1;//ת����ԭ�����ʽ
    }
    return num;
}
/*���ڽ������������ȡ��������ת���ɿ���ֱ�������������ʾ������*/
uint8_t Decode(uint8_t raw_data)
{
    switch (raw_data) {
        case 69: {
            return 1;
        }
        case 70: {
            return 2;
        }
        case 71:
        {
            return 3;
        }
        case 68:
        {
            return 4;
        }
        case 64:
        {
            return 5;
        }
        case 67:
        {
            return 6;
        }
        case 7:
        {
            return 7;
        }
        case 21:
        {
            return 8;
        }
        case 9:
        {
            return 9;
        }
        case 25:
        {
            return 0;
        }
        case 22: {
            return '*';
        }
        case 13:
        {
            return '#';
        }
        case 90:
        {
            return 90;
        }
        case 82:
        {
            return 80;
        }
        case 28:
        {
            return 'o';
        }
        default:return 3;
    }
}

/* USER CODE END PFP */

/* Private user code ---------------------------------------------------------*/
/* USER CODE BEGIN 0 */

/* USER CODE END 0 */

/**
  * @brief  The application entry point.
  * @retval int
  */
int main(void)
{
  /* USER CODE BEGIN 1 */

  /* USER CODE END 1 */

  /* MCU Configuration--------------------------------------------------------*/

  /* Reset of all peripherals, Initializes the Flash interface and the Systick. */
  HAL_Init();

  /* USER CODE BEGIN Init */

  /* USER CODE END Init */

  /* Configure the system clock */
  SystemClock_Config();

  /* USER CODE BEGIN SysInit */

  /* USER CODE END SysInit */

  /* Initialize all configured peripherals */
  MX_GPIO_Init();
  MX_SPI5_Init();
  MX_TIM2_Init();
  MX_TIM4_Init();
  MX_TIM3_Init();
  MX_I2C2_Init();
  MX_USART1_UART_Init();
  MX_TIM8_Init();
  /* USER CODE BEGIN 2 */

    HAL_TIM_Base_Start_IT(&htim2);
    HAL_TIM_Base_Start_IT(&htim3);
    HAL_TIM_Base_Start(&htim8);
    HAL_TIM_PWM_Start(&htim4, TIM_CHANNEL_1);
    HAL_TIM_PWM_Start(&htim4, TIM_CHANNEL_2);
    HAL_TIM_IC_Start_IT(&htim8, TIM_CHANNEL_4);
    //HAL_TIM_Base_Start(&htim2);
    __HAL_UART_ENABLE_IT(&huart1, UART_IT_RXNE);

    delay_init(100);

    LCD_Init();
    CH455_init();
    MPU6050_Init(&hi2c2);
    Bmp_Init();
   // Positional_PID_Init(&pid_fei,Pd_1,Id_1,Dd_1,50,10000);
    pid_init();
    //MPU_Init();
    LCD_Fill(0, 0, LCD_H, LCD_W, WHITE);
    LCD_ShowString(10, 0, "H_time:", BLUE, WHITE, 32, 0);
    LCD_ShowString(10, 32, "Pres:", BLUE, WHITE, 32, 0);
    LCD_ShowString(10, 64, "pitch:", BLUE, WHITE, 32, 0);
    LCD_ShowString(10, 96, "roll:", BLUE, WHITE, 32, 0);
    LCD_ShowString(10, 128, "yaw:", BLUE, WHITE, 32, 0);
    LCD_ShowString(10, 160, "UART_R:", BLUE, WHITE, 32, 0);
    LCD_ShowString(10, 192, "H:", BLUE, WHITE, 32, 0);

    LCD_ShowString(190, 192, "cm", RED, WHITE, 32, 0);
    LCD_ShowString(190, 0, "us", RED, WHITE, 32, 0);
    LCD_ShowString(190, 32, "kpa", RED, WHITE, 32, 0);

    HAL_GPIO_WritePin(GPIOI, GPIO_PIN_1, GPIO_PIN_RESET);
    HAL_UART_Transmit(&huart1, "Hello", sizeof("Hello"), 20);


  /* USER CODE END 2 */

  /* Infinite loop */
  /* USER CODE BEGIN WHILE */

    double pressure;
    while (1) {
    /* USER CODE END WHILE */

    /* USER CODE BEGIN 3 */
        MPU6050_ReadAccel(&hi2c2, &acceleration_x, &acceleration_y, &acceleration_z);
        MPU6050_ReadGyro(&hi2c2, &gyro_x, &gyro_y, &gyro_z);
        IMU_Update(gyro_x, gyro_y, gyro_z, acceleration_x, acceleration_y, acceleration_z, &Angle[0]);
        pressure = BMP280_Get_Pressure();
        pitch = Angle[0];
        roll = Angle[1];
        yaw = Angle[2];
        LCD_ShowIntNum(120, 0, pwm_duty, 4, RED, WHITE, 32);
        LCD_ShowFloatNum1(90, 32, pressure/1000.0, 5, RED, WHITE, 32);
        LCD_ShowIntNum(130, 160, uartRxBuffer[2], 2, RED, WHITE, 32);
        LCD_ShowFloatNum1(80, 192, Difference * 0.00064 / 2.0 * 340, 5, RED, WHITE, 32);//���뵥λ��cm
        //LCD_ShowFloatNum1(120, 200, pid_fei.output, 6, RED, WHITE, 16);
        MPU6050_LCD_PrintAngle(pitch, roll, yaw);

        //�������ʾ���ڽ��յ�������
        CH455_Display(1, 0);
        CH455_Display(2, 0);
        CH455_Display(3,0);
        CH455_Display(4, Decode(uartRxBuffer[2]));

        //������ģ��Ŀ���
        HAL_GPIO_WritePin(GPIOI,GPIO_PIN_1,GPIO_PIN_SET);//����һ��Trig�ź�
        delay_us(10);
        HAL_GPIO_WritePin(GPIOI,GPIO_PIN_1,GPIO_PIN_RESET);

        HAL_Delay(200);
//        MPU6050_LCD_PrintAccel(acceleration_x, acceleration_y, acceleration_z);
//        MPU6050_LCD_PrintGyro(gyro_x, gyro_y, gyro_z);
    }

  /* USER CODE END 3 */
}

/**
  * @brief System Clock Configuration
  * @retval None
  */
void SystemClock_Config(void)
{
  RCC_OscInitTypeDef RCC_OscInitStruct = {0};
  RCC_ClkInitTypeDef RCC_ClkInitStruct = {0};

  /** Configure the main internal regulator output voltage
  */
  __HAL_RCC_PWR_CLK_ENABLE();
  __HAL_PWR_VOLTAGESCALING_CONFIG(PWR_REGULATOR_VOLTAGE_SCALE1);

  /** Initializes the RCC Oscillators according to the specified parameters
  * in the RCC_OscInitTypeDef structure.
  */
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSI;
  RCC_OscInitStruct.HSIState = RCC_HSI_ON;
  RCC_OscInitStruct.HSICalibrationValue = RCC_HSICALIBRATION_DEFAULT;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
  RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSI;
  RCC_OscInitStruct.PLL.PLLM = 8;
  RCC_OscInitStruct.PLL.PLLN = 180;
  RCC_OscInitStruct.PLL.PLLP = RCC_PLLP_DIV2;
  RCC_OscInitStruct.PLL.PLLQ = 4;
  if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
  {
    Error_Handler();
  }

  /** Activate the Over-Drive mode
  */
  if (HAL_PWREx_EnableOverDrive() != HAL_OK)
  {
    Error_Handler();
  }

  /** Initializes the CPU, AHB and APB buses clocks
  */
  RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK|RCC_CLOCKTYPE_SYSCLK
                              |RCC_CLOCKTYPE_PCLK1|RCC_CLOCKTYPE_PCLK2;
  RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
  RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
  RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV4;
  RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV2;

  if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_5) != HAL_OK)
  {
    Error_Handler();
  }
}

/* USER CODE BEGIN 4 */

/* USER CODE END 4 */

/**
  * @brief  This function is executed in case of error occurrence.
  * @retval None
  */
void Error_Handler(void)
{
  /* USER CODE BEGIN Error_Handler_Debug */
    /* User can add his own implementation to report the HAL error return state */
    __disable_irq();
    while (1) {
    }
  /* USER CODE END Error_Handler_Debug */
}

#ifdef  USE_FULL_ASSERT
/**
  * @brief  Reports the name of the source file and the source line number
  *         where the assert_param error has occurred.
  * @param  file: pointer to the source file name
  * @param  line: assert_param error line source number
  * @retval None
  */
void assert_failed(uint8_t *file, uint32_t line)
{
  /* USER CODE BEGIN 6 */
  /* User can add his own implementation to report the file name and line number,
     ex: printf("Wrong parameters value: file %s on line %d\r\n", file, line) */
  /* USER CODE END 6 */
}
#endif /* USE_FULL_ASSERT */
