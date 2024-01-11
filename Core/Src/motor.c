//
#include "motor.h"

/**
  * @brief  Input the high level duration to a specific motor
  * @param  H_Level_time The duration of a high level, in microseconds
  * @param  motor_num Serial number of the motor. Valid values are 1,2,3,4
  * @retval None
  */
void set_motor(int H_Level_time, uint8_t motor_num) {

    //limit
    if (H_Level_time > 2490) {
        H_Level_time = 2490;
    }
    int CCR_Value;6
    CCR_Value = H_Level_time / 10;//ARR的值设置为了249，所以说CCR的值不可以超过249
    switch (motor_num) {
        case 1: {
            __HAL_TIM_SET_COMPARE(&htim4, TIM_CHANNEL_1, CCR_Value);
            break;
        }
        case 2: {
            __HAL_TIM_SET_COMPARE(&htim4, TIM_CHANNEL_2, CCR_Value);
            break;
        }
        case 3: {
            __HAL_TIM_SET_COMPARE(&htim4, TIM_CHANNEL_3, CCR_Value);
            break;
        }
        case 4: {
            __HAL_TIM_SET_COMPARE(&htim4, TIM_CHANNEL_4, CCR_Value);
            break;
        }
        default:;
    }
}
// Created by Chen on 2024-01-11.
//
