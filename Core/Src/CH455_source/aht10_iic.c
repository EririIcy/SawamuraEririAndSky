#include "CH455_Library/IIC/aht10_iic.h"
#include "delay.h"
//////////////////////////////////////////////////////////////////////////////////	 
//本程序只供学习使用，未经作者许可，不得用于其它任何用途
// STM32F429开发板
//IIC驱动代码	   
//All rights reserved									  
////////////////////////////////////////////////////////////////////////////////// 	

//IIC初始化
void AHT10_IIC_Init(void)
{
    GPIO_InitTypeDef GPIO_Initure;
    
    __HAL_RCC_GPIOF_CLK_ENABLE();   //使能GPIOF时钟
		__HAL_RCC_GPIOH_CLK_ENABLE();   //使能GPIOH时钟
    
    //PH5,PF10初始化设置
    GPIO_Initure.Pin=GPIO_PIN_10;
    GPIO_Initure.Mode=GPIO_MODE_OUTPUT_PP;  //推挽输出
    GPIO_Initure.Pull=GPIO_PULLUP;          //上拉
    GPIO_Initure.Speed=GPIO_SPEED_FAST;     //快速
    HAL_GPIO_Init(GPIOF,&GPIO_Initure);
	
		GPIO_Initure.Pin=GPIO_PIN_15;
    GPIO_Initure.Mode=GPIO_MODE_OUTPUT_PP;  //推挽输出
    GPIO_Initure.Pull=GPIO_PULLUP;          //上拉
    GPIO_Initure.Speed=GPIO_SPEED_FAST;     //快速
    HAL_GPIO_Init(GPIOH,&GPIO_Initure);
    
    AHT10_IIC_SDA_1;
    AHT10_IIC_SCL_1;  
}

//产生AHT10_IIC起始信号
void AHT10_IIC_Start(void)
{
	AHT10_SDA_OUT();     //sda线输出
	AHT10_IIC_SDA_1;	  	  
	AHT10_IIC_SCL_1;
	delay_us(4);
 	AHT10_IIC_SDA_0;//START:when CLK is high,DATA change form high to low 
	delay_us(4);
	AHT10_IIC_SCL_0;//钳住I2C总线，准备发送或接收数据 
}	  
//产生AHT10_IIC停止信号
void AHT10_IIC_Stop(void)
{
	AHT10_SDA_OUT();//sda线输出
	AHT10_IIC_SCL_0;
	AHT10_IIC_SDA_0;//STOP:when CLK is high DATA change form low to high
 	delay_us(4);
	AHT10_IIC_SCL_1; 
	delay_us(4);			
	AHT10_IIC_SDA_1;//发送I2C总线结束信号				   	
}
//等待应答信号到来
//返回值：1，接收应答失败
//        0，接收应答成功
uint8_t AHT10_IIC_Wait_Ack(void)
{
	uint8_t ucErrTime=0;
	AHT10_SDA_IN();      //SDA设置为输入  
	AHT10_IIC_SDA_1;delay_us(1);	   
	AHT10_IIC_SCL_1;delay_us(1);	 
	while(AHT10_READ_SDA)
	{
		ucErrTime++;
		if(ucErrTime>250)
		{
			AHT10_IIC_Stop();
			return 1;
		}
	}
	AHT10_IIC_SCL_0;//时钟输出0 	   
	return 0;  
} 
//产生ACK应答
void AHT10_IIC_Ack(void)
{
	AHT10_IIC_SCL_0;
	AHT10_SDA_OUT();
	AHT10_IIC_SDA_0;
	delay_us(2);
	AHT10_IIC_SCL_1;
	delay_us(2);
	AHT10_IIC_SCL_0;
}
//不产生ACK应答		    
void AHT10_IIC_NAck(void)
{
	AHT10_IIC_SCL_0;
	AHT10_SDA_OUT();
	AHT10_IIC_SDA_1;
	delay_us(2);
	AHT10_IIC_SCL_1;
	delay_us(2);
	AHT10_IIC_SCL_0;
}					 				     
//AHT10_IIC发送一个字节
//返回从机有无应答
//1，有应答
//0，无应答			  
void AHT10_IIC_Send_Byte(uint8_t txd)
{                        
    uint8_t t;   
	AHT10_SDA_OUT(); 	    
    AHT10_IIC_SCL_0;//拉低时钟开始数据传输
    for(t=0;t<8;t++)
    {         
				if((txd&0x80)>>7 == 1)
				{
					AHT10_IIC_SDA_1;
				}
				else
				{
					AHT10_IIC_SDA_0;
				}
//        AHT10_IIC_SDA=(txd&0x80)>>7;
        txd<<=1; 	  
				delay_us(2);   //对TEA5767这三个延时都是必须的
				AHT10_IIC_SCL_1;
				delay_us(2); 
				AHT10_IIC_SCL_0;	
				delay_us(2);
    }	 
} 	    
//读1个字节，ack=1时，发送ACK，ack=0，发送nACK   
uint8_t AHT10_IIC_Read_Byte(uint8_t ack)
{
	uint8_t i,receive=0;
	AHT10_SDA_IN();//SDA设置为输入
    for(i=0;i<8;i++ )
	{
        AHT10_IIC_SCL_0; 
        delay_us(2);
		AHT10_IIC_SCL_1;
        receive<<=1;
        if(AHT10_READ_SDA)receive++;   
		delay_us(1); 
    }					 
    if (!ack)
        AHT10_IIC_NAck();//发送nACK
    else
        AHT10_IIC_Ack(); //发送ACK   
    return receive;
}


