#ifndef __LCD_H_
#define __LCD_H_


/* Includes */
#include "fonts.h"


/* Public functions */
void LCD_Init();
void LCD_Clear(void);
void LCD_Fill(uint16_t color);
void LCD_On(void);
void LCD_Off(void);
void Set_Cursor(uint16_t xpos, uint16_t ypos);
void Set_Font(sFONT *fonts);
void Draw_Pixel(uint16_t x, uint16_t y, uint16_t color);
void Read_Pixel(uint16_t x, uint16_t y);
void Draw_Line(uint16_t x1, uint16_t y1, uint16_t x2, uint16_t y2, uint16_t color);
void Draw_Rect(uint16_t x1, uint16_t y1, uint16_t x2, uint16_t y2, uint16_t color);
void Draw_Full_Rect(uint16_t x1, uint16_t y1, uint16_t x2, uint16_t y2, uint16_t color);
void Draw_Circle(uint16_t x, uint16_t y, uint16_t r, uint16_t color);
void Draw_Full_Circle(uint16_t x, uint16_t y, uint16_t r, uint16_t color);
void Draw_Char(uint16_t x, uint16_t y, const uint16_t *c, uint16_t color);
void Display_Char(uint16_t x, uint16_t y, uint8_t c, uint16_t color);
void Display_String(uint16_t x, uint16_t y, char *ptr,uint16_t color);

void Draw_Image(uint16_t x, uint16_t y, uint16_t x_res, uint16_t y_res, const uint16_t *ptr_image);
void Draw_Image_8bit_Block(uint16_t x, uint16_t y, uint16_t x_res, uint16_t y_res, const uint8_t *ptr_image);
uint16_t LCD_BGR2RGB(uint16_t c);
void LCD_Setup (uint8_t contrast, uint8_t bright, uint8_t color);


typedef struct
{
    const unsigned short *img;
    uint16_t width;
    uint16_t height;
    long  dataSize;
} tImage;


//#include "../img/squirrel.c"
#include "../img/tulips.c"
//#include "../img/paris.c"
#include "../img/onoff.c"



typedef struct
{
  __IO uint16_t REG;  /* LCD Index Write            Address offset 0x00 */
  __IO uint16_t RAM;      /* LCD Data Write             Address offset 0x02 */
}LCD_TypeDef;

#define FSMC_LCD_BASE    ((uint32_t) (0x60000000 | 0x0001FFFE))
#define FSMC_LCD         ((LCD_TypeDef *) FSMC_LCD_BASE)

#define LCD_GDDRAM_WRITE    0x2C; /* Graphic Display Data RAM Register. */
#define LCD_GDDRAM_READ     0x2E;

struct
  {
    uint16_t    WHITE;
    uint16_t    BLACK;
    uint16_t    GREEN;
    uint16_t    RED;
    uint16_t    BLUE;
    uint16_t    GREY;
    uint16_t    ORANGE;
    uint16_t    YELLOW;    
  }COLOR={0xFFFF, 0x0000, 0x07E0, 0xF800, 0x001F, 0xF7DE, 0xFA20, 0xFFE0};

#endif /* __LCD_H_ */