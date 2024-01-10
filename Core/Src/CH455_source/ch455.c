#include "CH455_Library/CH455/ch455.h"
#include "CH455_Library/IIC/myiic.h"
#include "stm32f4xx_hal.h"
#include "delay.h"

//////////////////////////////////////////////////////////////////////////////////	 
//������ֻ��ѧϰʹ�ã�δ��������ɣ��������������κ���;
// STM32F429������
//All rights reserved									  
////////////////////////////////////////////////////////////////////////////////// 	

uint8_t CH450_buf[6];                 //6λ�����
const uint8_t BCD_decode_tab[0x10] = { 0X3F, 0X06, 0X5B, 0X4F, 0X66, 0X6D, 0X7D, 0X07, 0X7F, 0X6F, 0X77, 0X7C, 0X39, 0X5E, 0X79, 0X71 };
uint16_t CH455_KEY_RX_FLAG=0;       //���̽���״̬���	
uint8_t CH455_KEY_NUM=0;					//���¼��̵�ֵ

void CH450_Write(uint16_t cmd)	//д����
{
		IIC_Start();               /*��������*/
   	IIC_Send_Byte(((uint8_t)(cmd>>7)&CH450_I2C_MASK)|CH450_I2C_ADDR1);
		IIC_Wait_Ack(); 
   	IIC_Send_Byte((uint8_t)cmd);               /*��������*/
		IIC_Wait_Ack(); 
  	IIC_Stop();                 /*��������*/ 
}

void	CH450_buf_write( uint16_t cmd )  // ��CH450������ݻ��߲�������,�Զ���������ӳ��
{
	if ( cmd & 0x1000 ) 
		{   // �������ݵ�����,��Ҫ�������ݵ�ӳ�󻺳���
		CH450_buf[ (uint8_t)( ( cmd >> 8 ) - 2 ) & 0x07 ] = (uint8_t)( cmd & 0xFF );	// �������ݵ���Ӧ��ӳ��Ԫ
	}
	CH450_Write( cmd );	// ����
}


void	CH450_buf_index( uint8_t index, uint8_t dat )  // ��CH450ָ����������������,�Զ���������ӳ��
// index Ϊ��������,��ЧֵΪ0��5,�ֱ��ӦDIG2��DIG7
{
	uint16_t cmd;
	CH450_buf[ index ] = dat;	// �������ݵ���Ӧ��ӳ��Ԫ
	cmd = ( CH450_DIG2 + ( (uint16_t)index << 8 ) ) | dat ;	// ���ɲ�������
	CH450_Write( cmd );	// ����
}

void CH450_set_bit(uint8_t bit_addr)     //��λ����
{
	uint8_t byte_addr;
	byte_addr=(bit_addr&0x38)>>3;
	if ( byte_addr < 6 ) CH450_buf_index( byte_addr, CH450_buf[byte_addr] | (1<<(bit_addr&0x07)) );
}

void CH450_clr_bit(uint8_t bit_addr)     //��λϨ��
{
	uint8_t byte_addr;
	byte_addr=(bit_addr&0x38)>>3;
	if ( byte_addr < 6 ) CH450_buf_index( byte_addr, CH450_buf[byte_addr] & ~(1<<(bit_addr&0x07)) );
}
//CH455��ʼ��
void CH455_init(void)
{
	uint8_t i;
	for ( i = 0; i < 6; i ++ ) CH450_buf_index( i, 0 ); // ��ΪCH450��λʱ�������ʾ���ݣ����Ըտ���������Ϊ��գ��ٿ���ʾ
	CH450_buf_write(CH450_SYSON2);	// ������ʾ������
// �����Ҫ����ˢ����ʾ���ݣ���ôֻҪִ��7���������6�����ݼ�������Լ�1��������ʾ����
	CH455_Display(1,1);			// ��ʾBCD��1
	CH455_Display(2,2);
	CH455_Display(3,3);
	CH455_Display(4,4);
}

//�������ʾ
void CH455_Display(uint8_t digital,uint8_t num)		
{
	if(digital == 1){
		CH450_buf_write(CH450_DIG4 | BCD_decode_tab[num]);  //��1λ�������ʾ
	}
	else if(digital == 2){
		CH450_buf_write(CH450_DIG5 | BCD_decode_tab[num]);  //��2λ�������ʾ
	}
	else if(digital == 3){
		CH450_buf_write(CH450_DIG6 | BCD_decode_tab[num]);  //��3λ�������ʾ
	}
	else if(digital == 4){
		CH450_buf_write(CH450_DIG7 | BCD_decode_tab[num]);  //��4λ�������ʾ
	}
}


uint8_t CH455_Key_Read(void)		// ��CH450������������������
{
		uint8_t keycode;
		uint8_t ch455_key_num=0;
	
   	IIC_Start();               
   	IIC_Send_Byte(((uint8_t)(CH450_GET_KEY>>7)&CH450_I2C_MASK)|0x01|CH450_I2C_ADDR1);
		IIC_Wait_Ack();
   	keycode=IIC_Read_Byte(0);  
		IIC_Stop();        

		//����ֵת��Ϊkey1-key16
		switch(keycode)
		{
			case 0x44:
				ch455_key_num = 0;   //��Ӧ�����ı��key1
				break;			
			case 0x45:
				ch455_key_num = 1;
				break;
			case 0x46:
				ch455_key_num = 2;
				break;
			case 0x47:
				ch455_key_num = 3;
				break;
			case 0x4C:
				ch455_key_num = 4;
				break;
			case 0x4D:
				ch455_key_num = 5;
				break;
			case 0x4E:
				ch455_key_num = 6;
				break;
			case 0x4F:
				ch455_key_num = 7;
				break;
			case 0x54:
				ch455_key_num = 8;
				break;
			case 0x55:
				ch455_key_num = 9;
				break;
			case 0x56:
				ch455_key_num = 10;
				break;
			case 0x57:
				ch455_key_num = 11;
				break;
			case 0x5C:
				ch455_key_num = 12;
				break;
			case 0x5D:
				ch455_key_num = 13;
				break;
			case 0x5E:
				ch455_key_num = 14;
				break;
			case 0x5F:
				ch455_key_num = 15;
				break;
									
			default:
				ch455_key_num = 0;
				break;
		}
		
		return(ch455_key_num);
}


