//
// Created by Chen on 2023/12/7.
//

#ifndef SAWAMURAERIRIANDSKY_MPU6050_H
#define SAWAMURAERIRIANDSKY_MPU6050_H

void MPU6050_Init(I2C_HandleTypeDef *I2cHandle) ;
// 读取 MPU6050 数据
void MPU6050_ReadAccel(I2C_HandleTypeDef *I2cHandle, int16_t *Accel_X, int16_t *Accel_Y, int16_t *Accel_Z) ;

#endif //SAWAMURAERIRIANDSKY_MPU6050_H
