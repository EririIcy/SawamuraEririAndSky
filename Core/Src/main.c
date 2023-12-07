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
/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */

/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */
#define MPU6050_ADDR 0xD0 // 0x68 << 1
/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/

/* USER CODE BEGIN PV */
static int keyInterrupt1 = 0; //PG6
static int keyInterrupt2 = 0; //PG7
static int pwm_duty = 300;

int16_t acceleration_x=0;
int16_t acceleration_y=0;
int16_t acceleration_z=0;
/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
/* USER CODE BEGIN PFP */


// 初始化 I2C 和 MPU6050
void MPU6050_Init(I2C_HandleTypeDef *I2cHandle) {
    uint8_t data;
    // 唤醒 MPU6050
    data = 0;
    HAL_I2C_Mem_Write(I2cHandle, MPU6050_ADDR, 0x6B, 1, &data, 1, 1000);

    // 设置采样率
    data = 0x07;
    HAL_I2C_Mem_Write(I2cHandle, MPU6050_ADDR, 0x19, 1, &data, 1, 1000);

    // 设置加速度传感器量程
    data = 0x00; // +/- 2g
    HAL_I2C_Mem_Write(I2cHandle, MPU6050_ADDR, 0x1C, 1, &data, 1, 1000);

    // 设置陀螺仪量程
    data = 0x00; // +/- 250度/秒
    HAL_I2C_Mem_Write(I2cHandle, MPU6050_ADDR, 0x1B, 1, &data, 1, 1000);
}

// 读取 MPU6050 数据
void MPU6050_ReadAccel(I2C_HandleTypeDef *I2cHandle, int16_t *Accel_X, int16_t *Accel_Y, int16_t *Accel_Z) {
    uint8_t Rec_Data[6];

    // 读取加速度计的 X, Y, Z 轴的值
    HAL_I2C_Mem_Read(I2cHandle, MPU6050_ADDR, 0x3B, 1, Rec_Data, 6, 1000);

    *Accel_X = (int16_t) (Rec_Data[0] << 8 | Rec_Data[1]);
    *Accel_Y = (int16_t) (Rec_Data[2] << 8 | Rec_Data[3]);
    *Accel_Z = (int16_t) (Rec_Data[4] << 8 | Rec_Data[5]);
}

void HAL_GPIO_EXTI_Callback(uint16_t GPIO_PIN) {
    if (GPIO_PIN == GPIO_PIN_6) {
        keyInterrupt1 = 1;

        __HAL_GPIO_EXTI_CLEAR_FLAG(GPIO_PIN_6);
    }
    if (GPIO_PIN == GPIO_PIN_7) {
        keyInterrupt2 = 1;
        __HAL_GPIO_EXTI_CLEAR_FLAG(GPIO_PIN_7);
    }
}

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
    LCD_Fill(0, 0, LCD_H, LCD_W, WHITE);
    MPU6050_Init(&hi2c2);
    /* USER CODE END 2 */

    /* Infinite loop */
    /* USER CODE BEGIN WHILE */
    while (1) {
        /* USER CODE END WHILE */

        /* USER CODE BEGIN 3 */
        MPU6050_ReadAccel(&hi2c2,&acceleration_x,&acceleration_y,&acceleration_z);


        LCD_ShowString(0, 55, "pwm_duty:", RED, WHITE, 16, 0);
        LCD_ShowFloatNum1(80, 55, pwm_duty, 5, RED, WHITE, 16);
        HAL_Delay(200);
        LCD_ShowIntNum(80, 70, acceleration_x, 5, RED, WHITE, 16);
        LCD_ShowIntNum(80, 90, acceleration_y, 5, RED, WHITE, 16);
        LCD_ShowIntNum(80, 110, acceleration_z, 5, RED, WHITE, 16);
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
    if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK) {
        Error_Handler();
    }

    /** Activate the Over-Drive mode
    */
    if (HAL_PWREx_EnableOverDrive() != HAL_OK) {
        Error_Handler();
    }

    /** Initializes the CPU, AHB and APB buses clocks
    */
    RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK | RCC_CLOCKTYPE_SYSCLK
                                  | RCC_CLOCKTYPE_PCLK1 | RCC_CLOCKTYPE_PCLK2;
    RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
    RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
    RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV4;
    RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV2;

    if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_5) != HAL_OK) {
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
