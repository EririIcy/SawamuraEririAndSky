//
#include "main.h"
#define MPU6050_ADDR 0xD0 // 0x68 << 1
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
// Created by Chen on 2023/12/7.
//
