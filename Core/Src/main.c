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
#include "gpio.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#include "lcd_init.h"
#include "lcd.h"
#include "delay.h"
#include "MPU6050.h"

#include <math.h>
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
static int pwm_duty = 300;

int16_t acceleration_x = 0;  //三轴加速度
int16_t acceleration_y = 0;
int16_t acceleration_z = 0;

int16_t gyro_x = 0;  //三轴角加速度
int16_t gyro_y = 0;
int16_t gyro_z = 0;

float pitch, roll, yaw;
float Angle[3] = {0};//最终角度,IMU_Update中调用。
/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);

/* USER CODE BEGIN PFP */


void HAL_GPIO_EXTI_Callback(uint16_t GPIO_PIN) {
    //读取按键输入，而后在TIM2的中断回调函数中完成对duty的修改
    if (GPIO_PIN == GPIO_PIN_6) {
        keyInterrupt1 = 1;

        __HAL_GPIO_EXTI_CLEAR_FLAG(GPIO_PIN_6);
    }
    if (GPIO_PIN == GPIO_PIN_7) {
        keyInterrupt2 = 1;
        __HAL_GPIO_EXTI_CLEAR_FLAG(GPIO_PIN_7);
    }
}

/*通过外部中断来修改占空比*/
void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim) {
    if (htim->Instance == TIM2) {
        if (keyInterrupt1 == 1) {
            pwm_duty++;
            HAL_GPIO_TogglePin(GPIOB, GPIO_PIN_14);
            __HAL_TIM_SET_COMPARE(&htim4, TIM_CHANNEL_1, pwm_duty);
            __HAL_TIM_SET_COMPARE(&htim4, TIM_CHANNEL_2, pwm_duty);
            keyInterrupt1 = 0;
        }
        if (keyInterrupt2 == 1) {
            pwm_duty--;
            HAL_GPIO_TogglePin(GPIOB, GPIO_PIN_14);
            __HAL_TIM_SET_COMPARE(&htim4, TIM_CHANNEL_1, pwm_duty);
            __HAL_TIM_SET_COMPARE(&htim4, TIM_CHANNEL_2, pwm_duty);
            keyInterrupt2 = 0;
        }
    }
}

/*用于将补码转化成原码*/
int16_t getOriginalNum(int16_t num) {
    if (num & 0x8000)//判断最高位是否为1，1表示该数是一个负数
    {
        num = ~num + 1;//转化成原码的形式
    }
    return num;
}



/* USER CODE END PFP */

/* Private user code ---------------------------------------------------------*/
/* USER CODE BEGIN 0 */

/* USER CODE END 0 */

/**
  * @brief  The application entry point.
  * @retval int
  */
int main(void) {
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
    /* USER CODE BEGIN 2 */

    HAL_TIM_Base_Start_IT(&htim2);
    HAL_TIM_Base_Start_IT(&htim3);
    HAL_TIM_PWM_Start(&htim4, TIM_CHANNEL_1);
    HAL_TIM_PWM_Start(&htim4, TIM_CHANNEL_2);
    HAL_TIM_Base_Start(&htim2);


    delay_init(100);
    LCD_Init();

    MPU6050_Init(&hi2c2);
    //MPU_Init();
    LCD_Fill(0, 0, LCD_H, LCD_W, WHITE);
    LCD_ShowString(0, 55, "pwm_duty:", RED, WHITE, 16, 0);
    /* USER CODE END 2 */

    /* Infinite loop */
    /* USER CODE BEGIN WHILE */
    while (1) {
        /* USER CODE END WHILE */

        /* USER CODE BEGIN 3 */
        MPU6050_ReadAccel(&hi2c2, &acceleration_x, &acceleration_y, &acceleration_z);
        MPU6050_ReadGyro(&hi2c2, &gyro_x, &gyro_y, &gyro_z);
        IMU_Update(gyro_x, gyro_y, gyro_z, acceleration_x, acceleration_y, acceleration_z, &Angle[0]);

        pitch = Angle[0];
        roll = Angle[1];
        yaw = Angle[2];
        LCD_ShowFloatNum1(80, 55, pwm_duty, 5, RED, WHITE, 16);
        HAL_Delay(50);
//        MPU6050_LCD_PrintAccel(acceleration_x, acceleration_y, acceleration_z);
//        MPU6050_LCD_PrintGyro(gyro_x, gyro_y, gyro_z);
        MPU6050_LCD_PrintAngle(pitch, roll, yaw);

    }

    /* USER CODE END 3 */
}

/**
  * @brief System Clock Configuration
  * @retval None
  */
void SystemClock_Config(void) {
    RCC_OscInitTypeDef RCC_OscInitStruct = {0};
    RCC_ClkInitTypeDef RCC_ClkInitStruct = {0};

    /** Configure the main internal regulator output voltage
    */
    __HAL_RCC_PWR_CLK_ENABLE();
    __HAL_PWR_VOLTAGESCALING_CONFIG(PWR_REGULATOR_VOLTAGE_SCALE3);

    /** Initializes the RCC Oscillators according to the specified parameters
    * in the RCC_OscInitTypeDef structure.
    */
    RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSE;
    RCC_OscInitStruct.HSEState = RCC_HSE_ON;
    RCC_OscInitStruct.PLL.PLLState = RCC_PLL_NONE;
    if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK) {
        Error_Handler();
    }

    /** Initializes the CPU, AHB and APB buses clocks
    */
    RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK | RCC_CLOCKTYPE_SYSCLK
                                  | RCC_CLOCKTYPE_PCLK1 | RCC_CLOCKTYPE_PCLK2;
    RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_HSE;
    RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
    RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV4;
    RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV2;

    if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_0) != HAL_OK) {
        Error_Handler();
    }
}

/* USER CODE BEGIN 4 */

/* USER CODE END 4 */

/**
  * @brief  This function is executed in case of error occurrence.
  * @retval None
  */
void Error_Handler(void) {
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
