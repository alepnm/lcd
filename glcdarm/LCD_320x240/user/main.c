/**
 *
 */
/* Includes *******************************************************************/
#include "main.h"
#include "string.h"

TIM_TimeBaseInitTypeDef  TIM_TimeBaseStructure;
TIM_OCInitTypeDef  TIM_OCInitStructure;
uint16_t CCR1_Val = 600;

int main (void)
{
	char str[10];
	
	
  /* Initialization */

  Init_SysTick();
  Init_GPIO();
  Init_FSMC();
  //Init_LCD();	
	LCD_Init();	
	
  pwm_init();
  touch_init();

  /*!< At this stage the microcontroller clock setting is already configured,
         this is done through SystemInit() function which is called from startup
         file (startup_stm32f4xx.s) before to branch to application main.
         To reconfigure the default setting of SystemInit() function, refer to
         system_stm32f4xx.c file
       */

  /* Demo */
  TIM4->CCR1 = 80;

  Clear_Screen(LCD_BLACK);
	
	//Delay_ms(100);	
	//Demo_MMIA();
	
	Set_Font(&Font16x24);
  Display_String(14, 295, "Digital Devices", LCD_WHITE);
	Delay_ms(500);
	
	//Draw_Image(0, 0, 240, 320, image_data_Tulips);
	//Delay_ms(500);
	
  while(1)
  {	
		static char bakx[10], baky[10];
	
		Convert_Pos();	
	
			
		Display_String(100, 100, bakx, LCD_BLACK);
		Display_String(130, 100, baky, LCD_BLACK);
		
		uint16tostr(str, Pen_Point.X, 10);
		Display_String(100, 100, str, LCD_BLUE);
		strcpy(bakx, str);
		
		uint16tostr(str, Pen_Point.Y, 10);
//		uint16tostr(str, TIM4->CCR1, 10);
		Display_String(130, 100, str, LCD_BLUE);
		strcpy(baky, str);
		
		Draw_Full_Circle(Pen_Point.X0, Pen_Point.Y0, 2, LCD_YELLOW);
		
		Delay_ms(20);
	}
}

/*
 * Demonstration project designed for MMIA.
 */

void Demo_MMIA(void)
{
  uint16_t Number=0;
  //int CharCount;
  char StrNumber[10];

  Clear_Screen(0x0000);

  Set_Font(&Font16x24);
  Display_String(14, 295, "Digital Devices", LCD_WHITE);
  uint16tostr(StrNumber, Number, 10);
  Display_String(43, 295, StrNumber, LCD_WHITE);

  //CharCount = sprintf(StrNumber,"%d", Number);
  //Display_String(43, 295, StrNumber, LCD_WHITE);

  Display_String(72, 287, "(c)2013", LCD_WHITE);
  Set_Font(&Font12x12);
  Display_String(97, 285, "STM32F4-Discovery", LCD_WHITE);

  //Draw_Image(120, 195, 70, 70, img00);

  Set_Font(&Font8x8);
  Display_String(220, 259, "Compiled by jopl", LCD_WHITE);
  Display_String(230, 259, "SSD1289 + XPT2046", LCD_WHITE);

  Number = 100;
  Set_Font(&Font16x24);
  while (Number != 0)
  {
	  //Draw_Full_Rect(43, 295 ,61 ,25 , LCD_BLACK);
		
	  uint16tostr(StrNumber, Number, 10);
		
	  Display_String(43, 295, StrNumber, LCD_WHITE);
	  //CharCount = sprintf(StrNumber,"%d", Number);
	  //Display_String(43, 295, StrNumber, LCD_WHITE);
	    //TIM_OCInitStructure.TIM_Pulse = Number;
	    //TIM_OC2Init(TIM3, &TIM_OCInitStructure);
	    //TIM_OC2PreloadConfig(TIM3, TIM_OCPreload_Enable);
	  Delay_ms(100);
	  Number--;
  }

  GPIO_ToggleBits(GPIOD, GPIO_Pin_13);
	
	//Clear_Screen(0x0000);
	//Draw_Image(0, 319, 240, 320, img02);
	//Delay_ms(3000);

  Clear_Screen(0x0000);
  Set_Font(&Font16x24);
  Display_String(107, 255, "Random Lines", LCD_WHITE);

  Delay_ms(2000);
  Clear_Screen(0x0000);
  Random_Lines();
  Delay_ms(500);

  Clear_Screen(0x0000);
  Set_Font(&Font16x24);
  Display_String(107, 295, "Random Rectangles", LCD_WHITE);

  Delay_ms(2000);
  Clear_Screen(0x0000);
  Random_Rect();
  Delay_ms(500);

  Clear_Screen(0x0000);
  Set_Font(&Font16x24);
  Display_String(107, 271, "Random Circles", LCD_WHITE);

  Delay_ms(2000);
  Clear_Screen(0x0000);
  Random_Circle();
  Delay_ms(500);

  Clear_Screen(0x0000);
  Set_Font(&Font16x24);
  Display_String(107, 199, "Images", LCD_WHITE);

  //Delay_ms(2000);
  //Draw_Image(0, 319, 240, 320, img02);

}

/*
 * Draw random lines.
 */

void Random_Lines(void)
{
  uint16_t x1,y1,x2,y2;
  uint32_t i;
  uint16_t cr;

  for(i=0;i<500;i++)
  {
    x1=rand() % 240;  /*TODO: in Eclipse yields rand() error (no reference to _sbrk) */
    y1=rand() % 320;
    x2=rand() % 240;
    y2=rand() % 320;

    cr=rand();

    Draw_Line(x1, y1 ,x2 ,y2 , cr << 3);
    Delay_ms(10);
  }
}

/*
 * Draw random rectangles.
 */

void Random_Rect(void)
{
  uint16_t x1,y1,x2,y2,z;
  uint32_t i;
  uint16_t cr;

  for(i=0;i<500;i++)
  {
    x1=rand() % 240;  /*TODO: in Eclipse yields rand() error (no reference to _sbrk) */
    y1=rand() % 320;
    x2=rand() % 240;
    y2=rand() % 320;

    cr=rand();

    z=rand() % 10;

    if (z >= 5) Draw_Rect(x1, y1 ,x2 ,y2 , cr << 3);
    else Draw_Full_Rect(x1, y1 ,x2 ,y2 , cr << 3);
    Delay_ms(10);
  }
}

/*
 * Draw random circles.
 */

void Random_Circle(void)
{
  uint16_t x, y, r, z;
  uint32_t i;
  uint16_t cr;

  for(i=0;i<500;i++)
  {
    x=rand() % 140;  /*TODO: in Eclipse yields rand() error (no reference to _sbrk) */
    y=rand() % 220;
    r=(rand() % 50) + 1;

    cr=rand() << 3;

    z=rand() % 10;

    if (z >= 5) Draw_Circle(x+50, y+50, r, cr);
    else Draw_Full_Circle(x+50, y+50, r, cr);
    Delay_ms(10);
  }
}

#ifdef  USE_FULL_ASSERT

/**
  * @brief  Reports the name of the source file and the source line number
  *         where the assert_param error has occurred.
  * @param  file: pointer to the source file name
  * @param  line: assert_param error line source number
  * @retval None
  */
void assert_failed(uint8_t* file, uint32_t line)
{
  /* User can add his own implementation to report the file name and line number,
     ex: printf("Wrong parameters value: file %s on line %d\r\n", file, line) */

  while (1)
  {}
}
#endif

/**
  * @}
  */

/**
  * @}
  */

/******************* (C) COPYRIGHT 2011 STMicroelectronics *****END OF FILE****/
