#ifndef _AHT10_IIC_H
#define _AHT10_IIC_H
#include "stdint.h"
#include "stm32f4xx.h"
//////////////////////////////////////////////////////////////////////////////////	 
//本程序只供学习使用，未经作者许可，不得用于其它任何用途
//All rights reserved									  
////////////////////////////////////////////////////////////////////////////////// 	
//IO方向设置

#define AHT10_SDA_IN()  {GPIOF->MODER&=~(3<<(10*2));GPIOF->MODER|=0<<10*2;}	//PH5输入模式
#define AHT10_SDA_OUT() {GPIOF->MODER&=~(3<<(10*2));GPIOF->MODER|=1<<10*2;} //PH5输出模式


#define AHT10_IIC_SCL_0  HAL_GPIO_WritePin(GPIOH,GPIO_PIN_15,GPIO_PIN_RESET) //SCL
#define AHT10_IIC_SCL_1  HAL_GPIO_WritePin(GPIOH,GPIO_PIN_15,GPIO_PIN_SET) //SCL

#define AHT10_IIC_SDA_0  HAL_GPIO_WritePin(GPIOF,GPIO_PIN_10,GPIO_PIN_RESET) //SDA
#define AHT10_IIC_SDA_1  HAL_GPIO_WritePin(GPIOF,GPIO_PIN_10,GPIO_PIN_SET) //SDA

#define AHT10_READ_SDA  HAL_GPIO_ReadPin(GPIOF,GPIO_PIN_10)  //输入SDA
//AHT10_IIC所有操作函数
void AHT10_IIC_Init(void);                //初始化AHT10_IIC的IO口				 
void AHT10_IIC_Start(void);				//发送AHT10_IIC开始信号
void AHT10_IIC_Stop(void);	  			//发送AHT10_IIC停止信号
void AHT10_IIC_Send_Byte(uint8_t txd);			//AHT10_IIC发送一个字节
uint8_t AHT10_IIC_Read_Byte(uint8_t ack);//AHT10_IIC读取一个字节
uint8_t AHT10_IIC_Wait_Ack(void); 				//AHT10_IIC等待ACK信号
void AHT10_IIC_Ack(void);					//AHT10_IIC发送ACK信号
void AHT10_IIC_NAck(void);				//AHT10_IIC不发送ACK信号

void AHT10_IIC_Write_One_Byte(uint8_t daddr,uint8_t addr,uint8_t data);
uint8_t AHT10_IIC_Read_One_Byte(uint8_t daddr,uint8_t addr);	 
#endif

