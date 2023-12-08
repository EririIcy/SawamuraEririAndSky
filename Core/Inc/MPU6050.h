//
// Created by Chen on 2023/12/7.
//

#ifndef SAWAMURAERIRIANDSKY_MPU6050_H
#define SAWAMURAERIRIANDSKY_MPU6050_H

void MPU6050_Init(I2C_HandleTypeDef *I2cHandle) ;
// 读取 MPU6050 数据
void MPU6050_ReadAccel(I2C_HandleTypeDef *I2cHandle, int16_t *Accel_X, int16_t *Accel_Y, int16_t *Accel_Z) ;
void MPU6050_ReadGyro(I2C_HandleTypeDef *I2cHandle, int16_t *Gyro_X, int16_t *Gyro_Y, int16_t *Gyro_Z);
void MPU6050_LCD_PrintAccel(int16_t acceleration_x, int16_t acceleration_y, int16_t acceleration_z);
void MPU6050_LCD_PrintGyro(int16_t gyro_x, int16_t gyro_y, int16_t gyro_z);
#endif //SAWAMURAERIRIANDSKY_MPU6050_H
