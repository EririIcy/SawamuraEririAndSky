#include "i2c.h"
#include "lcd.h"
#include "MPU6050.h"
#include "delay.h"
#include <math.h>

#define MPU6050_ADDR 0xD0 // 0x68 << 1, MPU6050的地址是0x68，但是用HAL_I2C进行访问时，地址要左移一位。

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


/* 读取 MPU6050 加速度数据*/
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
    //传入原始数据即可。函数包含了将补码转化成原码的部分，显示负数没有问题。
    //it's ok to input raw date since this function can convert the complement to the source code
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
/*
 *     将角度信息打印在LCD屏幕上。
 *     传入原始数据即可。函数包含了将补码转化成原码的部分，显示负数没有问题。
 *     it's ok to input raw date since this function can convert the complement to the source code
 * */
void MPU6050_LCD_PrintAngle(float pitch, float roll, float yaw) {

    uint16_t x_start = 110;//输出的x起始位置
    uint16_t y_start = 32;
    uint16_t y_step = 32;
    uint32_t font_color = RED;
    uint32_t background_color = WHITE;
    uint32_t pitch_minus_color = background_color;
    uint32_t roll_minus_color = background_color;
    uint32_t yaw_minus_color = background_color;
    //the pitch/roll/yaw should be a positive number, otherwise it cannot be shown on the screen successfully
    if (pitch < 0) {
        pitch = fabs(pitch);
        pitch_minus_color = font_color;
    }
    if (roll < 0) {
        roll = fabs(roll);
        roll_minus_color = font_color;
    }
    if (yaw < 0) {
        yaw = fabs(yaw);
        yaw_minus_color = font_color;
    }
    LCD_ShowFloatNum1(x_start + 32, y_start + 1 * y_step, pitch, 4, font_color, background_color, 32);
    LCD_ShowFloatNum1(x_start + 32, y_start + 2 * y_step, roll, 4, font_color, background_color, 32);
    LCD_ShowFloatNum1(x_start + 32, y_start + 3 * y_step, yaw, 4, font_color, background_color, 32);
    LCD_ShowChar(x_start, y_start + 1 * y_step, '-', pitch_minus_color, background_color, 32, 1);
    LCD_ShowChar(x_start, y_start + 2 * y_step, '-', roll_minus_color, background_color, 32, 1);
    LCD_ShowChar(x_start, y_start + 3 * y_step, '-', yaw_minus_color, background_color, 32, 1);
}
//定义各变量

#define RtA         57.295779f    //弧度->角度
#define AtR            0.0174533f    //角度->弧度
#define Acc_G         0.0011963f
#define Gyro_G         0.0610351f
#define Gyro_Gr        0.0010653f

#define Kp 18.0f
#define Ki 0.008f
#define halfT 0.008f
float q0 = 1, q1 = 0, q2 = 0, q3 = 0;
float exInt = 0, eyInt = 0, ezInt = 0;

/*
 *  通过六轴传感器的数据计算得到角度
 *     */
void IMU_Update(short gyrox, short gyroy, short gyroz, short accx, short accy, short accz, float *Angle) {

    float ax = accx, ay = accy, az = accz;
    float gx = gyrox, gy = gyroy, gz = gyroz;
    float norm;
    float vx, vy, vz;
    float ex, ey, ez;
    float q0q0 = q0 * q0;
    float q0q1 = q0 * q1;
    float q0q2 = q0 * q2;
    float q1q1 = q1 * q1;
    float q1q3 = q1 * q3;
    float q2q2 = q2 * q2;
    float q2q3 = q2 * q3;
    float q3q3 = q3 * q3;
    if (ax * ay * az == 0)//此时任意方向角加速度为0
        return;
    gx *= Gyro_Gr;
    gy *= Gyro_Gr;
    gz *= Gyro_Gr;

    //对速度值归一化

    norm = sqrt(ax * ax + ay * ay + az * az);
    ax = ax / norm;
    ay = ay / norm;
    az = az / norm;

    // 提取姿态矩阵的重力分量
    vx = 2 * (q1q3 - q0q2);
    vy = 2 * (q0q1 + q2q3);
    vz = q0q0 - q1q1 - q2q2 + q3q3;

    // 得到误差向量，并进行积分而消除
    ex = (ay * vz - az * vy);
    ey = (az * vx - ax * vz);
    ez = (ax * vy - ay * vx);

    exInt = exInt + ex * Ki;
    eyInt = eyInt + ey * Ki;
    ezInt = ezInt + ez * Ki;

    // 互补滤波，将误差输入Pid控制器与本次姿态更新中陀螺仪测得的角速度相加，得到一个修正的角速度值，获得的修正的角速度值去更新四元素，从而获得准确的姿态角信息。
    gx = gx + Kp * ex + exInt;
    gy = gy + Kp * ey + eyInt;
    gz = gz + Kp * ez + ezInt;

    // 解四元数
    q0 = q0 + (-q1 * gx - q2 * gy - q3 * gz) * halfT;
    q1 = q1 + (q0 * gx + q2 * gz - q3 * gy) * halfT;
    q2 = q2 + (q0 * gy - q1 * gz + q3 * gx) * halfT;
    q3 = q3 + (q0 * gz + q1 * gy - q2 * gx) * halfT;

    // 四元数归一化
    norm = sqrt(q0 * q0 + q1 * q1 + q2 * q2 + q3 * q3);

    q0 = q0 / norm;
    q1 = q1 / norm;
    q2 = q2 / norm;
    q3 = q3 / norm;

    //求解姿态角（欧拉角）
    Angle[0] = asin(-2 * q1 * q3 + 2 * q0 * q2) * RtA; //pitch
    Angle[1] = atan2(2 * q2 * q3 + 2 * q0 * q1, -2 * q1 * q1 - 2 * q2 * q2 + 1) * RtA;      //roll
    Angle[2] = atan2(2 * q1 * q2 + 2 * q0 * q3, -2 * q2 * q2 - 2 * q3 * q3 + 1) * RtA;             // yaw
}
/*——————————————————————————————————————————————————————————————————————————————————————————
 * 版权声明：本文为CSDN博主「致力于成为光头」的原创文章，遵循CC 4.0 BY-SA版权协议，转载请附上原文出处链接及本声明。
原文链接：https://blog.csdn.net/m0_57763261/article/details/123085723*/