#include "i2c.h"
#include "lcd.h"
#include "MPU6050.h"
#include "delay.h"
#include <math.h>

#define MPU6050_ADDR 0xD0 // 0x68 << 1, MPU6050�ĵ�ַ��0x68��������HAL_I2C���з���ʱ����ַҪ����һλ��

/* ��ʼ�� I2C �� MPU6050*/
void MPU6050_Init(I2C_HandleTypeDef *I2cHandle) {
    uint8_t data;
    // ���� MPU6050
    data = 0;
    HAL_I2C_Mem_Write(I2cHandle, MPU6050_ADDR, 0x6B, 1, &data, 1, 1000);

    // ���ò�����
    data = 0x07;
    HAL_I2C_Mem_Write(I2cHandle, MPU6050_ADDR, 0x19, 1, &data, 1, 1000);

    // ���ü��ٶȴ���������
    data = 0x00; // +/- 2g
    HAL_I2C_Mem_Write(I2cHandle, MPU6050_ADDR, 0x1C, 1, &data, 1, 1000);

    // ��������������
    data = 0x00; // +/- 250��/��
    HAL_I2C_Mem_Write(I2cHandle, MPU6050_ADDR, 0x1B, 1, &data, 1, 1000);
}


/* ��ȡ MPU6050 ���ٶ�����*/
void MPU6050_ReadAccel(I2C_HandleTypeDef *I2cHandle, int16_t *Accel_X, int16_t *Accel_Y, int16_t *Accel_Z) {
    uint8_t Rec_Data[6];

    // ��ȡ���ٶȼƵ� X, Y, Z ���ֵ
    HAL_I2C_Mem_Read(I2cHandle, MPU6050_ADDR, 0x3B, 1, Rec_Data, 6, 1000);

    *Accel_X = (int16_t) (Rec_Data[0] << 8 | Rec_Data[1]);  //���͵�������8λ�����ݣ���λ����8λ����λ������Ȼ������������Ӽ��ɵõ�������16λ����
    *Accel_Y = (int16_t) (Rec_Data[2] << 8 | Rec_Data[3]);
    *Accel_Z = (int16_t) (Rec_Data[4] << 8 | Rec_Data[5]);
}


void MPU6050_ReadGyro(I2C_HandleTypeDef *I2cHandle, int16_t *Gyro_X, int16_t *Gyro_Y, int16_t *Gyro_Z) {
    uint8_t Rec_Data[6];

    // ��ȡ�����ǵ� X, Y, Z ���ֵ
    HAL_I2C_Mem_Read(I2cHandle, MPU6050_ADDR, 0x43, 1, Rec_Data, 6, 1000);

    // �����յ���������ϳ�ʵ�ʵ������Ƕ���
    *Gyro_X = (int16_t) (Rec_Data[0] << 8 | Rec_Data[1]);
    *Gyro_Y = (int16_t) (Rec_Data[2] << 8 | Rec_Data[3]);
    *Gyro_Z = (int16_t) (Rec_Data[4] << 8 | Rec_Data[5]);
}

/*��ӡ����*/
void MPU6050_LCD_PrintAccel(int16_t acceleration_x, int16_t acceleration_y, int16_t acceleration_z) {
    LCD_ShowString(50, 70, "x:", RED, WHITE, 16, 0);
    LCD_ShowString(50, 90, "y:", RED, WHITE, 16, 0);
    LCD_ShowString(50, 110, "z:", RED, WHITE, 16, 0);
    LCD_ShowString(140, 70, "m/s^2", RED, WHITE, 16, 0);
    LCD_ShowString(140, 90, "m/s^2", RED, WHITE, 16, 0);
    LCD_ShowString(140, 110, "m/s^2", RED, WHITE, 16, 0);
    if (acceleration_x & 0x8000)//��λ����������λ�����ʾ��һ������
    {
        LCD_ShowChar(65, 70, '-', RED, WHITE, 16, 1);
        acceleration_x = ~acceleration_x + 1;//��ԭ����
        LCD_ShowFloatNum1(80, 70, (float) acceleration_x / 65535.0 * 4 * 9.8, 4, RED, WHITE, 16);
    } else {
        LCD_ShowChar(65, 70, '-', WHITE, WHITE, 16, 1);
        LCD_ShowFloatNum1(80, 70, (float) acceleration_x / 65535.0 * 4 * 9.8, 4, RED, WHITE, 16);
    }
    if (acceleration_y & 0x8000)//��λ����������λ�����ʾ��һ������
    {
        LCD_ShowChar(65, 90, '-', RED, WHITE, 16, 1);
        acceleration_y = ~acceleration_y + 1;//��ԭ����
        LCD_ShowFloatNum1(80, 90, (float) acceleration_y / 65535.0 * 4 * 9.8, 4, RED, WHITE, 16);
    } else {
        LCD_ShowChar(65, 90, '-', WHITE, WHITE, 16, 1);
        LCD_ShowFloatNum1(80, 90, (float) acceleration_y / 65535.0 * 4 * 9.8, 4, RED, WHITE, 16);
    }
    if (acceleration_z & 0x8000)//��λ����������λ�����ʾ��һ������
    {
        LCD_ShowChar(65, 110, '-', RED, WHITE, 16, 1);
        acceleration_z = ~acceleration_z + 1;//��ԭ����
        LCD_ShowFloatNum1(80, 110, (float) acceleration_z / 65535.0 * 4 * 9.8, 4, RED, WHITE, 16);
    } else {
        LCD_ShowChar(65, 70, '-', WHITE, WHITE, 16, 1);
        LCD_ShowFloatNum1(80, 110, (float) acceleration_z / 65535.0 * 4 * 9.8, 4, RED, WHITE, 16);
    }

}

void MPU6050_LCD_PrintGyro(int16_t gyro_x, int16_t gyro_y, int16_t gyro_z) {
    //����ԭʼ���ݼ��ɡ����������˽�����ת����ԭ��Ĳ��֣���ʾ����û�����⡣
    //it's ok to input raw date since this function can convert the complement to the source code
    LCD_ShowString(50, 130, "x:", RED, WHITE, 16, 0);
    LCD_ShowString(50, 150, "y:", RED, WHITE, 16, 0);
    LCD_ShowString(50, 170, "z:", RED, WHITE, 16, 0);
    LCD_ShowString(140, 130, "��/s", RED, WHITE, 16, 0);
    LCD_ShowString(140, 150, "��/s", RED, WHITE, 16, 0);
    LCD_ShowString(140, 170, "��/s", RED, WHITE, 16, 0);
    if (gyro_x & 0x8000)//��λ����������λ�����ʾ��һ������
    {
        LCD_ShowChar(65, 130, '-', RED, WHITE, 16, 1);
        gyro_x = ~gyro_x + 1;//��ԭ����
        LCD_ShowFloatNum1(80, 130, (float) gyro_x / 65535.0 * 250, 4, RED, WHITE, 16);
    } else {
        LCD_ShowChar(65, 130, '-', WHITE, WHITE, 16, 1);
        LCD_ShowFloatNum1(80, 130, (float) gyro_x / 65535.0 * 250, 4, RED, WHITE, 16);
    }
    if (gyro_y & 0x8000)//��λ����������λ�����ʾ��һ������
    {
        LCD_ShowChar(65, 150, '-', RED, WHITE, 16, 1);
        gyro_y = ~gyro_y + 1;//��ԭ����
        LCD_ShowFloatNum1(80, 150, (float) gyro_y / 65535.0 * 250, 4, RED, WHITE, 16);
    } else {
        LCD_ShowChar(65, 150, '-', WHITE, WHITE, 16, 1);
        LCD_ShowFloatNum1(80, 150, (float) gyro_y / 65535.0 * 250, 4, RED, WHITE, 16);
    }
    if (gyro_z & 0x8000)//��λ����������λ�����ʾ��һ������
    {
        LCD_ShowChar(65, 170, '-', RED, WHITE, 16, 1);
        gyro_z = ~gyro_z + 1;//��ԭ����
        LCD_ShowFloatNum1(80, 170, (float) gyro_z / 65535.0 * 250, 4, RED, WHITE, 16);
    } else {
        LCD_ShowChar(65, 170, '-', WHITE, WHITE, 16, 1);
        LCD_ShowFloatNum1(80, 170, (float) gyro_z / 65535.0 * 250, 4, RED, WHITE, 16);
    }
}
/*
 *     ���Ƕ���Ϣ��ӡ��LCD��Ļ�ϡ�
 *     ����ԭʼ���ݼ��ɡ����������˽�����ת����ԭ��Ĳ��֣���ʾ����û�����⡣
 *     it's ok to input raw date since this function can convert the complement to the source code
 * */
void MPU6050_LCD_PrintAngle(float pitch, float roll, float yaw) {

    uint16_t x_start = 110;//�����x��ʼλ��
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
//���������

#define RtA         57.295779f    //����->�Ƕ�
#define AtR            0.0174533f    //�Ƕ�->����
#define Acc_G         0.0011963f
#define Gyro_G         0.0610351f
#define Gyro_Gr        0.0010653f

#define Kp 18.0f
#define Ki 0.008f
#define halfT 0.008f
float q0 = 1, q1 = 0, q2 = 0, q3 = 0;
float exInt = 0, eyInt = 0, ezInt = 0;

/*
 *  ͨ�����ᴫ���������ݼ���õ��Ƕ�
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
    if (ax * ay * az == 0)//��ʱ���ⷽ��Ǽ��ٶ�Ϊ0
        return;
    gx *= Gyro_Gr;
    gy *= Gyro_Gr;
    gz *= Gyro_Gr;

    //���ٶ�ֵ��һ��

    norm = sqrt(ax * ax + ay * ay + az * az);
    ax = ax / norm;
    ay = ay / norm;
    az = az / norm;

    // ��ȡ��̬�������������
    vx = 2 * (q1q3 - q0q2);
    vy = 2 * (q0q1 + q2q3);
    vz = q0q0 - q1q1 - q2q2 + q3q3;

    // �õ���������������л��ֶ�����
    ex = (ay * vz - az * vy);
    ey = (az * vx - ax * vz);
    ez = (ax * vy - ay * vx);

    exInt = exInt + ex * Ki;
    eyInt = eyInt + ey * Ki;
    ezInt = ezInt + ez * Ki;

    // �����˲������������Pid�������뱾����̬�����������ǲ�õĽ��ٶ���ӣ��õ�һ�������Ľ��ٶ�ֵ����õ������Ľ��ٶ�ֵȥ������Ԫ�أ��Ӷ����׼ȷ����̬����Ϣ��
    gx = gx + Kp * ex + exInt;
    gy = gy + Kp * ey + eyInt;
    gz = gz + Kp * ez + ezInt;

    // ����Ԫ��
    q0 = q0 + (-q1 * gx - q2 * gy - q3 * gz) * halfT;
    q1 = q1 + (q0 * gx + q2 * gz - q3 * gy) * halfT;
    q2 = q2 + (q0 * gy - q1 * gz + q3 * gx) * halfT;
    q3 = q3 + (q0 * gz + q1 * gy - q2 * gx) * halfT;

    // ��Ԫ����һ��
    norm = sqrt(q0 * q0 + q1 * q1 + q2 * q2 + q3 * q3);

    q0 = q0 / norm;
    q1 = q1 / norm;
    q2 = q2 / norm;
    q3 = q3 / norm;

    //�����̬�ǣ�ŷ���ǣ�
    Angle[0] = asin(-2 * q1 * q3 + 2 * q0 * q2) * RtA; //pitch
    Angle[1] = atan2(2 * q2 * q3 + 2 * q0 * q1, -2 * q1 * q1 - 2 * q2 * q2 + 1) * RtA;      //roll
    Angle[2] = atan2(2 * q1 * q2 + 2 * q0 * q3, -2 * q2 * q2 - 2 * q3 * q3 + 1) * RtA;             // yaw
}
/*������������������������������������������������������������������������������������������������������������������������������������������������������������������������������������
 * ��Ȩ����������ΪCSDN�����������ڳ�Ϊ��ͷ����ԭ�����£���ѭCC 4.0 BY-SA��ȨЭ�飬ת���븽��ԭ�ĳ������Ӽ���������
ԭ�����ӣ�https://blog.csdn.net/m0_57763261/article/details/123085723*/