#include "CH455_Library/IIC/myiic.h"
#include "delay.h"
//////////////////////////////////////////////////////////////////////////////////	 
//������ֻ��ѧϰʹ�ã�δ��������ɣ��������������κ���;
// STM32F429������
//IIC��������	   
//All rights reserved									  
////////////////////////////////////////////////////////////////////////////////// 	

//IIC��ʼ��
void IIC_Init(void)
{
    GPIO_InitTypeDef GPIO_Initure;
    
    __HAL_RCC_GPIOC_CLK_ENABLE();   //ʹ��GPIOCʱ��
    
    //PC14,15��ʼ������
    GPIO_Initure.Pin=GPIO_PIN_14|GPIO_PIN_15;
    GPIO_Initure.Mode=GPIO_MODE_OUTPUT_PP;  //�������
    GPIO_Initure.Pull=GPIO_PULLUP;          //����
    GPIO_Initure.Speed=GPIO_SPEED_FAST;     //����
    HAL_GPIO_Init(GPIOC,&GPIO_Initure);
    
    IIC_SDA_1;
    IIC_SCL_1;  
}

////����IIC��ʼ�ź�
//void IIC_Start(void)
//{
//	SDA_OUT();     //sda�����
//	IIC_SDA_1;	  	  
//	IIC_SCL_1;
//	delay_us(1);
// 	IIC_SDA_0;//START:when CLK is high,DATA change form high to low 
//	delay_us(1);
//	IIC_SCL_0;//ǯסI2C���ߣ�׼�����ͻ�������� 
//}	  
////����IICֹͣ�ź�
//void IIC_Stop(void)
//{
//	SDA_OUT();//sda�����
//	IIC_SCL_0;
//	IIC_SDA_0;//STOP:when CLK is high DATA change form low to high
// 	delay_us(1);
//	IIC_SCL_1; 
//	delay_us(1);			
//	IIC_SDA_1;//����I2C���߽����ź�				   	
//}
////�ȴ�Ӧ���źŵ���
////����ֵ��1������Ӧ��ʧ��
////        0������Ӧ��ɹ�
//uint8_t IIC_Wait_Ack(void)
//{
//	uint8_t ucErrTime=0;
//	SDA_IN();      //SDA����Ϊ����  
//	IIC_SDA_1;delay_us(1);	   
//	IIC_SCL_1;delay_us(1);	 
//	while(READ_SDA)
//	{
//		ucErrTime++;
//		if(ucErrTime>250)
//		{
//			IIC_Stop();
//			return 1;
//		}
//	}
//	IIC_SCL_0;//ʱ�����0 	   
//	return 0;  
//} 
////����ACKӦ��
//void IIC_Ack(void)
//{
//	IIC_SCL_0;
//	SDA_OUT();
//	IIC_SDA_0;
//	delay_us(1);
//	IIC_SCL_1;
//	delay_us(1);
//	IIC_SCL_0;
//}
////������ACKӦ��		    
//void IIC_NAck(void)
//{
//	IIC_SCL_0;
//	SDA_OUT();
//	IIC_SDA_1;
//	delay_us(1);
//	IIC_SCL_1;
//	delay_us(1);
//	IIC_SCL_0;
//}					 				     
////IIC����һ���ֽ�
////���شӻ�����Ӧ��
////1����Ӧ��
////0����Ӧ��			  
//void IIC_Send_Byte(uint8_t txd)
//{                        
//    uint8_t t;   
//		SDA_OUT(); 	    
//    IIC_SCL_0;//����ʱ�ӿ�ʼ���ݴ���
//    for(t=0;t<8;t++)
//    {         
//				if((txd&0x80)>>7 == 1)
//				{
//					IIC_SDA_1;
//				}
//				else
//				{
//					IIC_SDA_0;
//				}
////        IIC_SDA=(txd&0x80)>>7;
//        txd<<=1; 	  
//				delay_us(1);   //��TEA5767��������ʱ���Ǳ����
//				IIC_SCL_1;
//				delay_us(1); 
//				IIC_SCL_0;	
//				delay_us(1);
//    }	 
//} 	    
////��1���ֽڣ�ack=1ʱ������ACK��ack=0������nACK   
//uint8_t IIC_Read_Byte(uint8_t ack)
//{
//	uint8_t i,receive=0;
//	SDA_IN();//SDA����Ϊ����
//  for(i=0;i<8;i++ )
//	{
//        IIC_SCL_0; 
//        delay_us(1);
//		IIC_SCL_1;
//        receive<<=1;
//        if(READ_SDA)receive++;   
//		delay_us(1); 
//    }					 
//    if (!ack)
//        IIC_NAck();//����nACK
//    else
//        IIC_Ack(); //����ACK   
//    return receive;
//}

//����IIC��ʼ�ź�
void IIC_Start(void)
{
	SDA_OUT();     //sda�����
	IIC_SDA_1;	  	  
	IIC_SCL_1;
	delay_us(1);
 	IIC_SDA_0;//START:when CLK is high,DATA change form high to low 
	delay_us(1);
	IIC_SCL_0;//ǯסI2C���ߣ�׼�����ͻ�������� 
}	  
//����IICֹͣ�ź�
void IIC_Stop(void)
{
	SDA_OUT();//sda�����
	IIC_SCL_0;
	IIC_SDA_0;//STOP:when CLK is high DATA change form low to high
 	delay_us(1);
	IIC_SCL_1; 
	delay_us(1);			
	IIC_SDA_1;//����I2C���߽����ź�				   	
}
//�ȴ�Ӧ���źŵ���
//����ֵ��1������Ӧ��ʧ��
//        0������Ӧ��ɹ�
uint8_t IIC_Wait_Ack(void)
{
	uint8_t ucErrTime=0;
	SDA_IN();      //SDA����Ϊ����  
	IIC_SDA_1;delay_us(1);	   
	IIC_SCL_1;delay_us(1);	 
	while(READ_SDA)
	{
		ucErrTime++;
		if(ucErrTime>250)
		{
			IIC_Stop();
			return 1;
		}
	}
	IIC_SCL_0;//ʱ�����0 	   
	return 0;  
} 
//����ACKӦ��
void IIC_Ack(void)
{
	IIC_SCL_0;
	SDA_OUT();
	IIC_SDA_0;
	delay_us(1);
	IIC_SCL_1;
	delay_us(1);
	IIC_SCL_0;
}
//������ACKӦ��		    
void IIC_NAck(void)
{
	IIC_SCL_0;
	SDA_OUT();
	IIC_SDA_1;
	delay_us(1);
	IIC_SCL_1;
	delay_us(1);
	IIC_SCL_0;
}					 				     
//IIC����һ���ֽ�
//���شӻ�����Ӧ��
//1����Ӧ��
//0����Ӧ��			  
void IIC_Send_Byte(uint8_t txd)
{                        
    uint8_t t;   
		SDA_OUT(); 	    
    IIC_SCL_0;//����ʱ�ӿ�ʼ���ݴ���
    for(t=0;t<8;t++)
    {         
				if((txd&0x80)>>7 == 1)
				{
					IIC_SDA_1;
				}
				else
				{
					IIC_SDA_0;
				}
//        IIC_SDA=(txd&0x80)>>7;
        txd<<=1; 	  
//				delay_us(1);   //��TEA5767��������ʱ���Ǳ����
				IIC_SCL_1;
				delay_us(1); 
				IIC_SCL_0;	
				delay_us(1);
    }	 
} 	    
//��1���ֽڣ�ack=1ʱ������ACK��ack=0������nACK   
uint8_t IIC_Read_Byte(uint8_t ack)
{
	uint8_t i,receive=0;
	SDA_IN();//SDA����Ϊ����
  for(i=0;i<8;i++ )
	{
        IIC_SCL_0; 
        delay_us(1);
		IIC_SCL_1;
        receive<<=1;
        if(READ_SDA)receive++;   
		delay_us(1); 
    }					 
    if (!ack)
        IIC_NAck();//����nACK
    else
        IIC_Ack(); //����ACK   
    return receive;
}



