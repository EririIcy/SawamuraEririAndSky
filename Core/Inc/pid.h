/*
 * pid.h
 *
 *  Created on: 2023��9��6��
 *      Author: 77cream
 */

#ifndef USER_PID_H_
#define USER_PID_H_

//#include "zf_common_headfile.h"
#include "main.h"

//#include "image.h"
//#include "encoder.h"
//typedef struct
//{
//    float kp,ki,kd;//����ϵ��
//    float error,lastError;//���ϴ����
//    float integral,maxIntegral;//���֡������޷�
//    float output,maxOutput;//���������޷�
//}PID;



//void PID_Init(PID *pid,float p,float i,float d,float maxI,float maxOut);
//void PID_Calc(PID *pid,float set_value,float get_value);

//

typedef struct
{
    float kp,ki,kd;                         //����ϵ��
    float error,lastError,lastlastError;    //���ϴ������ϴ����
    float integral,maxIntegral;             //���֡������޷�
    float output,maxOutput;                 //���������޷�
} PID;

extern PID pid_fei;

void Incremental_PID_Init(PID *pid, float p, float i, float d, float maxOutput);
void Incremental_PID_Calc(PID *pid, float set_value, float get_value);

void Positional_PID_Init(PID *pid, float p, float i, float d, float maxI, float maxOutput);
void Positional_PID_Calc(PID *pid,float set_value, float get_value);
void pid_init(void);




int Velocity_Control(int MeasureVelocity,int TargetVelocity);
#endif /* USER_PID_H_ */
