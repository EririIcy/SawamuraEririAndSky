#ifndef _CH455_H
#define _CH455_H
#include "stdint.h"



//////////////////////////////////////////////////////////////////////////////////	 
//������ֻ��ѧϰʹ�ã�δ��������ɣ��������������κ���;
//All rights reserved									  
////////////////////////////////////////////////////////////////////////////////// 	
//IO��������

/* ********************************************************************************************* */
/* �������� */

/* CH450�ĳ��������룬���������I2C���ݣ���ô��8λӦ������1λ */
#define CH450_SYSOFF    0x0400					// �ر���ʾ
#define CH450_SYSON1    0x0401					// ������ʾ
#define CH450_SYSON2    0x0403					// ������ʾ,����
#define CH450_GET_KEY	0x0700					// ��ȡ��������ֵ
#define CH450_DIG2      0x1200		            // �����λ2��ʾ,�����8λ����
#define CH450_DIG3      0x1300		            // �����λ3��ʾ,�����8λ����
#define CH450_DIG4      0x1400		            // �����λ4��ʾ,�����8λ����
#define CH450_DIG5      0x1500					// �����λ5��ʾ,�����8λ����
#define CH450_DIG6      0x1600					// �����λ6��ʾ,�����8λ����
#define CH450_DIG7      0x1700		            // �����λ7��ʾ,�����8λ����

// CH450�ӿڶ���
#define		CH450_I2C_ADDR1		0x40			// CH450�ĵ�ַ
#define		CH450_I2C_MASK		0x3E			// CH450�ĸ��ֽ���������

//��Ч��������
#define		CH450_KEY_MIN		0x42
#define		CH450_KEY_MAX		0x7F





void CH450_Write(uint16_t cmd);	// ��CH450������������
void	CH450_buf_write( uint16_t cmd );
void	CH450_buf_index( uint8_t index, uint8_t dat );
void CH450_set_bit(uint8_t bit_addr);
void CH450_clr_bit(uint8_t bit_addr);
// �����ӳ���
extern void CH455_init(void);
extern uint8_t CH455_Key_Read(void);				// ��CH450������������������
extern void CH455_Display(uint8_t digital,uint8_t num);
#endif



