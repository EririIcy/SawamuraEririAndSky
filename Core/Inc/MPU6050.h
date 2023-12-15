//
// Created by Chen on 2023/12/7.
//

#ifndef __MPU6050_H
#define __MPU6050_H

#include "mpuiic.h"
#include "eMPL_header/inv_mpu.h"
#include "eMPL_header/inv_mpu_dmp_motion_driver.h"

/*
硬件I2C模式
需要：
1.I2C
    I2C
    (默认设置)
    标准模式
    时钟频率100kHz
    地址长度7bit
    不用填写设备地址
取消下方注释
*/

// extern I2C_HandleTypeDef hi2c2;
// #define MPU6050_I2C_Handle hi2c2
// #define MPU6050_Hardware_I2C

/*
软件I2C模式
需要：
1.GPIO 2个
    均为开漏输出（上不上拉取决于外部电路）
    最高等级
取消下方注释,按照自己的管脚更改即可
*/

#define MPU6050_Software_I2C

#ifdef MPU6050_Software_I2C
#define I2C_Group_SCL GPIOH // I2C的时钟GPIO组号
#define I2C_SCL GPIO_PIN_4  // I2C时钟的GPIO端口号

#define I2C_Group_SDA GPIOH // I2C的数据GPIO组号
#define I2C_SDA GPIO_PIN_5  // I2C数据的GPIO端口号

#define I2C_Write_SCL(x) HAL_GPIO_WritePin(I2C_Group_SCL, I2C_SCL, x)
#define I2C_Write_SDA(x) HAL_GPIO_WritePin(I2C_Group_SDA, I2C_SDA, x)

#define I2C_Read_SCL() HAL_GPIO_ReadPin(I2C_Group_SCL, I2C_SCL)
#define I2C_Read_SDA() HAL_GPIO_ReadPin(I2C_Group_SDA, I2C_SDA)
#endif
//如果AD0脚(9脚)接地,IIC地址为0X68(不包含最低位).
//如果接V3.3,则IIC地址为0X69(不包含最低位).
#define MPU_ADDR 0X68

void MPU6050_Init(I2C_HandleTypeDef *I2cHandle);

// 读取 MPU6050 数据
void MPU6050_ReadAccel(I2C_HandleTypeDef *I2cHandle, int16_t *Accel_X, int16_t *Accel_Y, int16_t *Accel_Z);

void MPU6050_ReadGyro(I2C_HandleTypeDef *I2cHandle, int16_t *Gyro_X, int16_t *Gyro_Y, int16_t *Gyro_Z);

void MPU6050_LCD_PrintAccel(int16_t acceleration_x, int16_t acceleration_y, int16_t acceleration_z);

void MPU6050_LCD_PrintGyro(int16_t gyro_x, int16_t gyro_y, int16_t gyro_z);

void MPU6050_LCD_PrintAngle(float pitch, float roll, float yaw);

void IMU_Update(short gyrox, short gyroy, short gyroz, short accx, short accy, short accz, float *Angle);

#endif
