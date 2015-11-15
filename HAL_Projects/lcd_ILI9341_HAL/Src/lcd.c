
#include "lcd.h"

static sFONT *Current_Font;

uint32_t qqq;


/*
 * Store data to LCD RAM.
 */
inline static void LCD_Set_Register(uint16_t reg, uint16_t data)
{
  FSMC_LCD->REG = reg;
  FSMC_LCD->RAM = data;
}


inline static uint16_t LCD_Get_Ram(uint16_t reg)
{
  FSMC_LCD->REG = reg;
  return FSMC_LCD->RAM;
}

/*
 * Set Register
 */
inline static void LCD_WriteComand(uint16_t cmd) 
  {
    FSMC_LCD->REG = cmd;
  }

/*
 * Set Data
 */
inline static void LCD_WriteData(uint16_t data) 
  {
    FSMC_LCD->RAM = data;
  }

/*
 * Prepares writing to GDDRAM.
 * Next coming data are directly displayed.
 */
inline static void Write_GDDRAM_Prepare(void)
{
  FSMC_LCD->REG = LCD_GDDRAM_WRITE;
}


/*
 * Prepares reading from GDDRAM.
 * Next coming data are directly displayed.
 */
inline static void Read_GDDRAM_Prepare(void)
{
  FSMC_LCD->REG = LCD_GDDRAM_READ;
}


void LCD_Init(void)
  {
    ClrBit(LCD_RST_GPIO_Port, LCD_RST_Pin);
    HAL_Delay(50);
    SetBit(LCD_RST_GPIO_Port, LCD_RST_Pin);
    HAL_Delay(50);
    
    LCD_WriteComand (0x01); // software reset comand
    //LCD_WriteComand (0x04);   //read Display ID
    HAL_Delay(5);
    LCD_WriteComand (0x28); // display off
    //------------power control------------------------------
    LCD_WriteComand (0xC0); // power control
    LCD_WriteData   (0x26); // GVDD = 4.75v
    LCD_WriteComand (0xC1); // power control
    LCD_WriteData   (0x11); // AVDD=VCIx2, VGH=VCIx7, VGL=-VCIx3
    //--------------VCOM-------------------------------------
    LCD_WriteComand (0xC5); // vcom control
    LCD_WriteData   (0x35); // Set the VCOMH voltage (0x35 = 4.025v)
    LCD_WriteData   (0x3E); // Set the VCOML voltage (0x3E = -0.950v)
    LCD_WriteComand (0xC7); // vcom control
    LCD_WriteData   (0xBE); // 0x94 (0xBE = nVM: 1, VCOMH: VMH–2, VCOML: VML–2)
    //------------memory access control------------------------
    LCD_WriteComand (0x36); // memory access control
    LCD_WriteData   (0x48); // 0048 my,mx,mv,ml,BGR,mh,0.0 (mirrors)
    LCD_WriteComand (0x3A); // pixel format set
    LCD_WriteData   (0x55); // 16bit /pixel
    //-------------ddram ----------------------------
    LCD_WriteComand (0x2A); // column set
    LCD_WriteData   (0x00); // x0_HIGH---0
    LCD_WriteData   (0x00); // x0_LOW----0
    LCD_WriteData   (0x00); // x1_HIGH---240
    LCD_WriteData   (0xEF); // x1_LOW----240
    LCD_WriteComand (0x2B); // page address set
    LCD_WriteData   (0x00); // y0_HIGH---0
    LCD_WriteData   (0x00); // y0_LOW----0
    LCD_WriteData   (0x01); // y1_HIGH---320
    LCD_WriteData   (0x3F); // y1_LOW----320
    //LCD_WriteComand (0x2E);   // memory read
    LCD_WriteComand (0x34); // tearing effect off
    //LCD_write_cmd(0x35); // tearing effect on
    //LCD_write_cmd(0xb4); // display inversion
    LCD_WriteComand (0xB7); // entry mode set
    // Deep Standby Mode: OFF
    // Set the output level of gate driver G1~G320: Normal display
    // Low voltage detection: Disable
    LCD_WriteData   (0x07); 
    //-----------------display------------------------
    LCD_WriteComand (0xB6); // display function control
    //Set the scan mode in non-display area
    //Determine source/VCOM output in a non-display area in the partial display mode
    LCD_WriteData   (0x0A);
    //Select whether the liquid crystal type is normally white type or normally black type
    //Sets the direction of scan by the gate driver in the range determined by SCN and NL
    //Select the shift direction of outputs from the source driver
    //Sets the gate driver pin arrangement in combination with the GS bit to select the optimal scan mode for the module
    //Specify the scan cycle interval of gate driver in non-display area when PTG to select interval scan
    LCD_WriteData   (0x82);
    // Sets the number of lines to drive the LCD at an interval of 8 lines
    LCD_WriteData   (0x27); 
    LCD_WriteData   (0x00); // clock divisor
    LCD_WriteComand (0x11); // sleep out
    HAL_Delay(100);
    LCD_WriteComand (0x29); // display on
    HAL_Delay(100);
    Write_GDDRAM_Prepare();
    HAL_Delay(5);
    
    LCD_Clear();
    HAL_Delay(5);
  }


void LCD_Off(void)
  {
    LCD_WriteComand (0x28);
  }

void LCD_On(void)
  {
    LCD_WriteComand (0x29);
  }

void LCD_Clear()  
  {   
    LCD_Fill(COLOR.BLACK);
  }

void LCD_Fill(uint16_t color)  
  {     
    uint32_t i;
   
    Set_Cursor(0, 0);
    
    Write_GDDRAM_Prepare();
    
    for (i = 0; i < 320*240; i++)
      {
        LCD_WriteData(color);
      }
  }

/*
 * Draw a Single Pixel on Position [x, y].
 */
void Draw_Pixel(uint16_t x, uint16_t y, uint16_t color)
{
    Set_Cursor(x, y);
    Write_GDDRAM_Prepare();
    LCD_WriteData(color);
}


/* neveikia */
void Read_Pixel(uint16_t x, uint16_t y)
  {    
    Set_Cursor(x, y);
    Read_GDDRAM_Prepare();
    LCD_WriteComand(0x00);
    LCD_WriteComand(0x00);
    qqq = FSMC_LCD->REG;
    qqq = LCD_Get_Ram(0x00);
    qqq = LCD_Get_Ram(0x00);
    qqq = LCD_Get_Ram(0x00);
    qqq = LCD_Get_Ram(0x00);
    qqq = LCD_Get_Ram(0x00);
  }


/*
 * Set Cursor to Position [x, y].
 */
void Set_Cursor(uint16_t xpos, uint16_t ypos)
{	  	    
	LCD_WriteComand(0x2A);
	LCD_WriteData(xpos>>8);
	LCD_WriteData(0x00FF&xpos);
	
	LCD_WriteComand(0x2B);
	LCD_WriteData(ypos>>8);
	LCD_WriteData(0x00FF&ypos);		
}

/*
 * Draw a line in the requested color.
 */
void Draw_Line(uint16_t x1, uint16_t y1, uint16_t x2, uint16_t y2, uint16_t color)
{
  uint8_t yLonger = 0;
  int incrementVal, endVal;
  int shortLen = y2-y1;
  int longLen = x2-x1;
	int decInc;
	int j = 0, i = 0;

  if(abs(shortLen) > abs(longLen)) {
    int swap = shortLen;
    shortLen = longLen;
    longLen = swap;
    yLonger = 1;
  }

  endVal = longLen;

  if(longLen < 0) {
    incrementVal =- 1;
    longLen =- longLen;
    endVal--;
  } else {
    incrementVal = 1;
    endVal++;
  }

  if(longLen == 0)
    decInc = 0;
  else
    decInc = (shortLen << 16) / longLen;

  if(yLonger) {
    for(i = 0;i != endVal;i += incrementVal) {
      Draw_Pixel(x1 + (j >> 16),y1 + i,color);
      j += decInc;
    }
  } else {
    for(i = 0;i != endVal;i += incrementVal) {
      Draw_Pixel(x1 + i,y1 + (j >> 16),color);
      j += decInc;
    }
  }
}


/*
 * Draw a rectangle in the requested color.
 * x1, y1 - the position of one corner
 * x2, y2 - the position of the other corner
 * color - color of the rectangle.
 */
void Draw_Rect(uint16_t x1, uint16_t y1, uint16_t x2, uint16_t y2, uint16_t color)
{
  Draw_Line(x1, y1, x2, y1, color);
  Draw_Line(x2, y1, x2, y2, color);
  Draw_Line(x2, y2, x1, y2, color);
  Draw_Line(x1, y2, x1, y1, color);
}


/*
 * Draw a full rectangle.
 * x1, y1 - the position of one corner
 * x2, y2 - the position of the other corner
 * color - color of the rectangle.
 */
void Draw_Full_Rect(uint16_t x1, uint16_t y1, uint16_t x2, uint16_t y2, uint16_t color)
{
  while(x1 < x2)
  {
    Draw_Line(x1, y1, x1, y2, color);
    x1++;
  }
}


/*
 * Draw a circle.
 * x, y - center of circle.
 * r - radius.
 * color - color of the circle.
 */
void Draw_Circle(uint16_t x, uint16_t y, uint16_t r, uint16_t color)
{
  int32_t  D;       /* Decision Variable */
  int32_t  CurX;   /* Current X Value */
  int32_t  CurY;   /* Current Y Value */

  D = 3 - (r << 1);
  CurX = 0;
  CurY = r;

  while (CurX <= CurY)
  {
    if (((x+CurX) < 240) && ((y+CurY) < 320))
      Draw_Pixel(x+CurX, y+CurY, color);
    if (((x+CurX) < 240) && ((y-CurY) >= 0))
      Draw_Pixel(x+CurX, y-CurY, color);
    if (((x-CurX) >= 0) && ((y+CurY) < 320))
      Draw_Pixel(x-CurX, y+CurY, color);
    if (((x-CurX) >= 0) && ((y-CurY) >= 0))
      Draw_Pixel(x-CurX, y-CurY, color);
    if (((x+CurY) < 240) && ((y+CurX) < 320))
      Draw_Pixel(x+CurY, y+CurX, color);
    if (((x+CurY) < 240) && ((y-CurX) >= 0))
      Draw_Pixel(x+CurY, y-CurX, color);
    if (((x-CurY) >= 0) && ((y+CurX) < 320))
      Draw_Pixel(x-CurY, y+CurX, color);
    if (((x-CurY) >= 0) && ((y-CurX) >= 0))
      Draw_Pixel(x-CurY, y-CurX, color);

    if (D < 0)
    {
      D += (CurX << 2) + 6;
    }
    else
    {
      D += ((CurX - CurY) << 2) + 10;
      CurY--;
    }
    CurX++;
  }
}


/*
 * Draw a full circle.
 * x, y - center of circle.
 * r - radius.
 * color - color of the circle.
 */
void Draw_Full_Circle(uint16_t x, uint16_t y, uint16_t r, uint16_t color)
{
  int32_t  D;       /* Decision Variable */
  uint32_t  CurX;   /* Current X Value */
  uint32_t  CurY;   /* Current Y Value */

  D = 3 - (r << 1);
  CurX = 0;
  CurY = r;

  while (CurX <= CurY)
  {
    if(CurY > 0)
    {
      Draw_Line(x-CurX, y+CurY, x-CurX, y-CurY, color);
      Draw_Line(x+CurX, y+CurY, x+CurX, y-CurY, color);
    }

    if(CurX > 0)
    {
      Draw_Line(x-CurY, y+CurX, x-CurY, y-CurX, color);
      Draw_Line(x+CurY, y+CurX, x+CurY, y-CurX, color);
    }

    if (D < 0)
    {
      D += (CurX << 2) + 6;
    }
    else
    {
      D += ((CurX - CurY) << 2) + 10;
      CurY--;
    }
    CurX++;
  }
}


/*
 * Draw a character.
 * x, y - position
 * color - character's color
 * *c - pointer to character data
 */
void Draw_Char(uint16_t x, uint16_t y, const uint16_t *c, uint16_t color)
{
  uint32_t index = 0, i = 0;

  for(index = 0; index < Current_Font->Height; index++)
  {
    for(i = 0; i < Current_Font->Width; i++)
    {
      if( ((((c[index] & ((0x80 << ((Current_Font->Width / 12 ) * 8 ) ) >> i)) == 0x00) && (Current_Font->Width <= 12)) ||
          (((c[index] & (0x1 << i)) == 0x00)&&(Current_Font->Width > 12 )))  == 0x00)
      {
        Draw_Pixel(x, y-1-i, color);
      }
    }
    x++;
  }
}


/*
 * Display a character.
 * x, y - position
 * c - character in ASCII
 * color - character's color
 */
void Display_Char(uint16_t x, uint16_t y, uint8_t c, uint16_t color)
{
  c -= 32;
  Draw_Char(x, y, &Current_Font->table[c * Current_Font->Height], color);
}


/*
 * Display a string.
 * x - line. Next line have to respect the high of used font.
 * y - vertical position
 * *ptr - pointer to string.
 */
void Display_String(uint16_t x, uint16_t y, char *ptr,uint16_t color)
{
  uint16_t refcolumn = y;

  /* Send the string character by character on LCD */
  while ((*ptr != 0) & (((refcolumn + 1) & 0xFFFF) >= Current_Font->Width))
  {
    /* Display one character on LCD */
    Display_Char(x, refcolumn, *ptr, color);
    /* Decrement the column position by 16 */
    refcolumn -= Current_Font->Width;
    /* Point on the next character */
    ptr++;
  }
}


/*
 * Sets the text font.
 */
void Set_Font(sFONT *fonts)
{
  Current_Font = fonts;
}

/*
 * Draw an image in format GRB565.
 * x, y - position, where to start displaying.
 * x_res, y_res - resolution in pixels.
 * *ptr_image - pointer to image array.
 */
void Draw_Image(uint16_t x, uint16_t y, uint16_t x_res, uint16_t y_res, const uint16_t *ptr_image)
{
  uint16_t i = 0, j = 0;

  for(i = 0; i < y_res; i++)
  {
    Set_Cursor(x, y+i);
		
    Write_GDDRAM_Prepare();
		
    for(j = 0; j < x_res; j++)
    {
      LCD_WriteData(*(ptr_image++));      
    }
  }
}


void Draw_Image_8bit_Block(uint16_t x, uint16_t y, uint16_t x_res, uint16_t y_res, const uint8_t *ptr_image)
{
  uint16_t color;
  uint16_t i = 0, j = 0;

  for(i = 0; i < y_res; i++)
  {
    Set_Cursor(x, y+i);
		
    Write_GDDRAM_Prepare();
		
		for(j = 0; j < x_res; j++)
    {
			color = (*(ptr_image+1) << 8) | (*(ptr_image));
			LCD_WriteData(color);
			ptr_image += 2;
		}
  }
}

/* neveikia */
void LCD_Setup (uint8_t contrast, uint8_t bright, uint8_t color)
{
    LCD_WriteComand ( 0xBC );
    LCD_WriteData(contrast);
    LCD_WriteData(bright);
    LCD_WriteData(color);
    LCD_WriteData(0x01);    //postprocessor: 1 - params on, 0 - params off
}

uint16_t LCD_BGR2RGB(uint16_t c)
{
	uint16_t  r, g, b, rgb;
    
	b=(c>>0)&0x1f;
	g=(c>>5)&0x3f;
	r=(c>>11)&0x1f;	 
	rgb=(b<<11)+(g<<5)+(r<<0);		 
	return(rgb);
}