#ifndef __TOUCH_7846_H
#define __TOUCH_7846_H

#ifdef __cplusplus
 extern "C" {
#endif
 
#include "stm32f4xx_hal.h"
#include "cmsis_os.h"
   
   
typedef struct 
{
	uint16_t X0;
	uint16_t Y0;
	uint16_t X; 
	uint16_t Y;						   	    
	uint8_t  Key_Sta;
	uint8_t  Key_LSta;
        uint8_t  noise;
        uint16_t time;
	float xfac;
	float yfac;
	short xoff;
	short yoff;
}Pen_Holder;

extern Pen_Holder Pen_Point;
 
#define TOUCH_CS_PORT	 GPIOC	
#define TOUCH_CS_PIN	 GPIO_Pin_6

#define T_CS()    GPIO_ResetBits(TOUCH_CS_PORT, TOUCH_CS_PIN);
#define T_DCS()   GPIO_SetBits(TOUCH_CS_PORT, TOUCH_CS_PIN);

#define CMD_RDY 0X90  
#define CMD_RDX	0XD0     											 
 

#define PEN  GPIOB->IDR&(1<<12) //
#define NPEN !(0x0080&PEN)  //!PEN




#define sFLASH_SPI                           SPI2
#define sFLASH_SPI_CLK                       RCC_APB1Periph_SPI2
#define sFLASH_SPI_CLK_INIT                  RCC_APB1PeriphClockCmd

#define sFLASH_SPI_SCK_PIN                   GPIO_Pin_13
#define sFLASH_SPI_SCK_GPIO_PORT             GPIOB
#define sFLASH_SPI_SCK_GPIO_CLK              RCC_AHB1Periph_GPIOB
#define sFLASH_SPI_SCK_SOURCE                GPIO_PinSource13
#define sFLASH_SPI_SCK_AF                    GPIO_AF_SPI2

#define sFLASH_SPI_MISO_PIN                  GPIO_Pin_14
#define sFLASH_SPI_MISO_GPIO_PORT            GPIOB
#define sFLASH_SPI_MISO_GPIO_CLK             RCC_AHB1Periph_GPIOB
#define sFLASH_SPI_MISO_SOURCE               GPIO_PinSource14
#define sFLASH_SPI_MISO_AF                   GPIO_AF_SPI2

#define sFLASH_SPI_MOSI_PIN                  GPIO_Pin_15
#define sFLASH_SPI_MOSI_GPIO_PORT            GPIOB
#define sFLASH_SPI_MOSI_GPIO_CLK             RCC_AHB1Periph_GPIOB
#define sFLASH_SPI_MOSI_SOURCE               GPIO_PinSource15
#define sFLASH_SPI_MOSI_AF                   GPIO_AF_SPI2

#define sFLASH_CS_PIN                        GPIO_Pin_11
#define sFLASH_CS_GPIO_PORT                  GPIOB
#define sFLASH_CS_GPIO_CLK                   RCC_AHB1Periph_GPIOB

#define sFLASH_CS_LOW()       GPIO_ResetBits(sFLASH_CS_GPIO_PORT, sFLASH_CS_PIN)
#define sFLASH_CS_HIGH()      GPIO_SetBits(sFLASH_CS_GPIO_PORT, sFLASH_CS_PIN) 

unsigned char SPI_RW_Byte(uint8_t num);
void SPI_Delay(unsigned int DelayCnt);
uint16_t TPReadX(void);
uint16_t TPReadY(void);	   
uint16_t read_once(void);	
uint8_t Read_Ads7846(void); 

//void EXTI9_5_IRQHandler(void);
//void NVIC_TOUCHConfiguration(void);
void touch_init(void);				  
//void LCD_ShowNum(uint8_t x,uint16_t y,uint16_t data);
//void Drow_Touch_Point(uint16_t x,uint16_t y);
//void Draw_Big_Point(uint16_t x,uint16_t y);
void Convert_Pos(void);
//void Touch_Adjust(void);

#define Key_Down 0x01
#define Key_Up   0x00 

#ifdef __cplusplus
}
#endif

#endif 


















