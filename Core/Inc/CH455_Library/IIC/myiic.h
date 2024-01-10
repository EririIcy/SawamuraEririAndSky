#ifndef _MYIIC_H
#define _MYIIC_H
#include "stdint.h"
#include "stm32f4xx.h"
//////////////////////////////////////////////////////////////////////////////////	 
//������ֻ��ѧϰʹ�ã�δ��������ɣ��������������κ���;
//All rights reserved									  
////////////////////////////////////////////////////////////////////////////////// 	
//IO��������
#define SDA_IN()  {GPIOC->MODER&=~(uint32_t)(3U<<(15*2));GPIOC->MODER|=(uint32_t)(0U<<15*2);}	//PC15����ģʽ
#define SDA_OUT() {GPIOC->MODER&=~(uint32_t)(3U<<(15*2));GPIOC->MODER|=(uint32_t)(1U<<15*2);} //PC15���ģʽ


#define IIC_SCL_0  HAL_GPIO_WritePin(GPIOC,GPIO_PIN_14,GPIO_PIN_RESET) //SCL
#define IIC_SCL_1  HAL_GPIO_WritePin(GPIOC,GPIO_PIN_14,GPIO_PIN_SET) //SCL

#define IIC_SDA_0  HAL_GPIO_WritePin(GPIOC,GPIO_PIN_15,GPIO_PIN_RESET) //SDA
#define IIC_SDA_1  HAL_GPIO_WritePin(GPIOC,GPIO_PIN_15,GPIO_PIN_SET) //SDA

#define READ_SDA  HAL_GPIO_ReadPin(GPIOC,GPIO_PIN_15)  //����SDA
//IIC���в�������
void IIC_Init(void);                //��ʼ��IIC��IO��				 
void IIC_Start(void);				//����IIC��ʼ�ź�
void IIC_Stop(void);	  			//����IICֹͣ�ź�
void IIC_Send_Byte(uint8_t txd);			//IIC����һ���ֽ�
uint8_t IIC_Read_Byte(uint8_t ack);//IIC��ȡһ���ֽ�
uint8_t IIC_Wait_Ack(void); 				//IIC�ȴ�ACK�ź�
void IIC_Ack(void);					//IIC����ACK�ź�
void IIC_NAck(void);				//IIC������ACK�ź�

void IIC_Write_One_Byte(uint8_t daddr,uint8_t addr,uint8_t data);
uint8_t IIC_Read_One_Byte(uint8_t daddr,uint8_t addr);	 
#endif

