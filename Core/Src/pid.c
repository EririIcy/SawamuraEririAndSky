/*
 * pid.c
 *
 *  Created on: 2023��9��6��
 *      Author: 77cream
 */

// #include "pid.h"
//
//
//

// void PID_Init (PID *pid, float p, float i, float d, float maxI, float maxOut)
//{
//     pid->kp = p;
//     pid->ki = i;
//     pid->kd = d;
//     pid->maxIntegral = maxI;
//     pid->maxOutput = maxOut;
// }
//
////����һ��pid����
////����Ϊ(pid�ṹ��,Ŀ��ֵ,����ֵ)������������pid�ṹ���output��Ա��
// void PID_Calc (PID *pid,float set_value, float get_value)
//{
//     //��������
//     pid->lastError = pid->error; //����error������
//     pid->error = set_value - get_value; //������error
//
//     //����΢��
//     float dout = (pid->error - pid->lastError) * pid->kd;
//     //�������
//     float pout = pid->error * pid->kp;
//     //�������
//     pid->integral += pid->error * pid->ki;
//
//     //�����޷�
//     if (pid->integral > pid->maxIntegral)
//         pid->integral = pid->maxIntegral;
//     else if (pid->integral < -pid->maxIntegral)
//         pid->integral = -pid->maxIntegral;
//     //�������
//     pid->output = pout + dout + pid->integral;
//     //����޷�
//     if (pid->output > pid->maxOutput)
//         pid->output = pid->maxOutput;
//     else if (pid->output < -pid->maxOutput)
//         pid->output = -pid->maxOutput;
// }

#include "pid.h"

/**************************************************************************
�������ܣ�����ʽPID�����ĳ�ʼ��
��ڲ�����pidָ�롢��ʼ���趨��p��i��dֵ������޷�ֵ
����  ֵ����
**************************************************************************/
PID pid_fei;

void Incremental_PID_Init(PID *pid, float p, float i, float d, float maxOutput)
{
    pid->kp = p;
    pid->ki = i;
    pid->kd = d;
    pid->maxOutput = maxOutput;
}

/**************************************************************************
�������ܣ�����PI������
��ڲ�����pidָ�롢Ŀ��ֵ������ֵ
����  ֵ����
��������ʽ��ɢPID��ʽ
pwm+=Kp[e��k��-e(k-1)]+Ki*e(k)+Kd[e(k)-2e(k-1)+e(k-2)]
e(k)������ƫ��
e(k-1)������һ�ε�ƫ��  �Դ�����
pwm�����������
�����ǵ��ٶȿ��Ʊջ�ϵͳ���棬ֻʹ��PI����
pwm+=Kp[e��k��-e(k-1)]+Ki*e(k)
**************************************************************************/
void Incremental_PID_Calc(PID *pid, float set_value, float get_value)
{
    pid->error = set_value - get_value; // ����ƫ��
    pid->output += pid->kp * (pid->error - pid->lastError) + pid->ki * pid->error +
                   pid->kd * (pid->error - 2 * pid->lastError + pid->lastlastError); // ����ʽPI������
    pid->lastlastError = pid->lastError;                                             // �������ϴ����
    pid->lastError = pid->error;                                                     // ������һ��ƫ��

    if (pid->output > pid->maxOutput)
        pid->output = pid->maxOutput;
    else if (pid->output < -pid->maxOutput)
        pid->output = -pid->maxOutput; // ����޷�
}

/**************************************************************************
�������ܣ�λ��ʽPID�����ĳ�ʼ��
��ڲ�����pidָ�롢��ʼ���趨��p��i��dֵ�������޷�ֵ������޷�ֵ
����  ֵ����
**************************************************************************/
void Positional_PID_Init(PID *pid, float p, float i, float d, float maxI, float maxOutput)
{
    pid->kp = p;
    pid->ki = i;
    pid->kd = d;
    pid->maxIntegral = maxI;
    pid->maxOutput = maxOutput;
}

// ����һ��pid����
// ����Ϊ(pid�ṹ��,Ŀ��ֵ,����ֵ)������������pid�ṹ���output��Ա��
void Positional_PID_Calc(PID *pid, float set_value, float get_value)
{
    float dout, pout;
    // ��������
    pid->lastError = pid->error;        // ����error������
    pid->error = set_value - get_value; // ������error

    // ����΢��
    dout = (pid->error - pid->lastError) * pid->kd;
    // �������
    pout = pid->error * pid->kp;
    // �������
    pid->integral += pid->error * pid->ki;

    // �����޷�
    if (pid->integral > pid->maxIntegral)
        pid->integral = pid->maxIntegral;
    else if (pid->integral < -pid->maxIntegral)
        pid->integral = -pid->maxIntegral;
    // �������
    pid->output = pout + dout + pid->integral;
    // ����޷�
    if (pid->output > pid->maxOutput)
        pid->output = pid->maxOutput;
    else if (pid->output < -pid->maxOutput)
        pid->output = -pid->maxOutput;
}

void pid_init(void){

    Positional_PID_Init(&pid_fei, Pd_1, Id_1, Dd_1, 50, 10000);
}
