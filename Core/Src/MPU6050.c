//
#include "i2c.h"
#include "lcd.h"

#define MPU6050_ADDR 0xD0 // 0x68 << 1
#define MPU6050_PWR_MGMT_1 0x6B // 电源管理寄存器 1

uint8_t restart = 0x80; //准备发送复位命令的数值


/* 初始化 I2C 和 MPU6050*/
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


/* 读取 MPU6050 数据*/
void MPU6050_ReadAccel(I2C_HandleTypeDef *I2cHandle, int16_t *Accel_X, int16_t *Accel_Y, int16_t *Accel_Z) {
    uint8_t Rec_Data[6];

    // 读取加速度计的 X, Y, Z 轴的值
    HAL_I2C_Mem_Read(I2cHandle, MPU6050_ADDR, 0x3B, 1, Rec_Data, 6, 1000);

    *Accel_X = (int16_t) (Rec_Data[0] << 8 | Rec_Data[1]);  //发送的是两个8位的数据，高位左移8位，低位不动，然后两个数据相加即可得到完整的16位数据
    *Accel_Y = (int16_t) (Rec_Data[2] << 8 | Rec_Data[3]);
    *Accel_Z = (int16_t) (Rec_Data[4] << 8 | Rec_Data[5]);
}


void MPU6050_ReadGyro(I2C_HandleTypeDef *I2cHandle, int16_t *Gyro_X, int16_t *Gyro_Y, int16_t *Gyro_Z) {
    uint8_t Rec_Data[6];

    // 读取陀螺仪的 X, Y, Z 轴的值
    HAL_I2C_Mem_Read(I2cHandle, MPU6050_ADDR, 0x43, 1, Rec_Data, 6, 1000);

    // 将接收到的数据组合成实际的陀螺仪读数
    *Gyro_X = (int16_t) (Rec_Data[0] << 8 | Rec_Data[1]);
    *Gyro_Y = (int16_t) (Rec_Data[2] << 8 | Rec_Data[3]);
    *Gyro_Z = (int16_t) (Rec_Data[4] << 8 | Rec_Data[5]);
}

/*打印数据*/
void MPU6050_LCD_PrintAccel(int16_t acceleration_x, int16_t acceleration_y, int16_t acceleration_z) {
    LCD_ShowString(50, 70, "x:", RED, WHITE, 16, 0);
    LCD_ShowString(50, 90, "y:", RED, WHITE, 16, 0);
    LCD_ShowString(50, 110, "z:", RED, WHITE, 16, 0);
    LCD_ShowString(140, 70, "m/s^2", RED, WHITE, 16, 0);
    LCD_ShowString(140, 90, "m/s^2", RED, WHITE, 16, 0);
    LCD_ShowString(140, 110, "m/s^2", RED, WHITE, 16, 0);
    if (acceleration_x & 0x8000)//用位与来检测最高位。真表示是一个负数
    {
        LCD_ShowChar(65, 70, '-', RED, WHITE, 16, 1);
        acceleration_x = ~acceleration_x + 1;//还原补码
        LCD_ShowFloatNum1(80, 70, (float) acceleration_x / 65535.0 * 4 * 9.8, 4, RED, WHITE, 16);
    } else {
        LCD_ShowChar(65, 70, '-', WHITE, WHITE, 16, 1);
        LCD_ShowFloatNum1(80, 70, (float) acceleration_x / 65535.0 * 4 * 9.8, 4, RED, WHITE, 16);
    }
    if (acceleration_y & 0x8000)//用位与来检测最高位。真表示是一个负数
    {
        LCD_ShowChar(65, 90, '-', RED, WHITE, 16, 1);
        acceleration_y = ~acceleration_y + 1;//还原补码
        LCD_ShowFloatNum1(80, 90, (float) acceleration_y / 65535.0 * 4 * 9.8, 4, RED, WHITE, 16);
    } else {
        LCD_ShowChar(65, 90, '-', WHITE, WHITE, 16, 1);
        LCD_ShowFloatNum1(80, 90, (float) acceleration_y / 65535.0 * 4 * 9.8, 4, RED, WHITE, 16);
    }
    if (acceleration_z & 0x8000)//用位与来检测最高位。真表示是一个负数
    {
        LCD_ShowChar(65, 110, '-', RED, WHITE, 16, 1);
        acceleration_z = ~acceleration_z + 1;//还原补码
        LCD_ShowFloatNum1(80, 110, (float) acceleration_z / 65535.0 * 4 * 9.8, 4, RED, WHITE, 16);
    } else {
        LCD_ShowChar(65, 70, '-', WHITE, WHITE, 16, 1);
        LCD_ShowFloatNum1(80, 110, (float) acceleration_z / 65535.0 * 4 * 9.8, 4, RED, WHITE, 16);
    }

}

void MPU6050_LCD_PrintGyro(int16_t gyro_x, int16_t gyro_y, int16_t gyro_z) {
    LCD_ShowString(50, 130, "x:", RED, WHITE, 16, 0);
    LCD_ShowString(50, 150, "y:", RED, WHITE, 16, 0);
    LCD_ShowString(50, 170, "z:", RED, WHITE, 16, 0);
    LCD_ShowString(140, 130, "°/s", RED, WHITE, 16, 0);
    LCD_ShowString(140, 150, "°/s", RED, WHITE, 16, 0);
    LCD_ShowString(140, 170, "°/s", RED, WHITE, 16, 0);
    if (gyro_x & 0x8000)//用位与来检测最高位。真表示是一个负数
    {
        LCD_ShowChar(65, 130, '-', RED, WHITE, 16, 1);
        gyro_x = ~gyro_x + 1;//还原补码
        LCD_ShowFloatNum1(80, 130, (float) gyro_x / 65535.0 * 250, 4, RED, WHITE, 16);
    } else {
        LCD_ShowChar(65, 130, '-', WHITE, WHITE, 16, 1);
        LCD_ShowFloatNum1(80, 130, (float) gyro_x / 65535.0 * 250, 4, RED, WHITE, 16);
    }
    if (gyro_y & 0x8000)//用位与来检测最高位。真表示是一个负数
    {
        LCD_ShowChar(65, 150, '-', RED, WHITE, 16, 1);
        gyro_y = ~gyro_y + 1;//还原补码
        LCD_ShowFloatNum1(80, 150, (float) gyro_y / 65535.0 * 250, 4, RED, WHITE, 16);
    } else {
        LCD_ShowChar(65, 150, '-', WHITE, WHITE, 16, 1);
        LCD_ShowFloatNum1(80, 150, (float) gyro_y / 65535.0 * 250, 4, RED, WHITE, 16);
    }
    if (gyro_z & 0x8000)//用位与来检测最高位。真表示是一个负数
    {
        LCD_ShowChar(65, 170, '-', RED, WHITE, 16, 1);
        gyro_z = ~gyro_z + 1;//还原补码
        LCD_ShowFloatNum1(80, 170, (float) gyro_z / 65535.0 * 250, 4, RED, WHITE, 16);
    } else {
        LCD_ShowChar(65, 170, '-', WHITE, WHITE, 16, 1);
        LCD_ShowFloatNum1(80, 170, (float) gyro_z / 65535.0 * 250, 4, RED, WHITE, 16);
    }
}

// Created by Chen on 2023/12/7.
//
