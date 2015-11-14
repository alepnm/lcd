/**
  ******************************************************************************
  * @file    Project/STM32F4xx_StdPeriph_Templates/main.c 
  * @author  MCD Application Team
  * @version V1.0.0
  * @date    30-September-2011
  * @brief   Main program body
  ******************************************************************************
  * @attention
  *
  * THE PRESENT FIRMWARE WHICH IS FOR GUIDANCE ONLY AIMS AT PROVIDING CUSTOMERS
  * WITH CODING INFORMATION REGARDING THEIR PRODUCTS IN ORDER FOR THEM TO SAVE
  * TIME. AS A RESULT, STMICROELECTRONICS SHALL NOT BE HELD LIABLE FOR ANY
  * DIRECT, INDIRECT OR CONSEQUENTIAL DAMAGES WITH RESPECT TO ANY CLAIMS ARISING
  * FROM THE CONTENT OF SUCH FIRMWARE AND/OR THE USE MADE BY CUSTOMERS OF THE
  * CODING INFORMATION CONTAINED HEREIN IN CONNECTION WITH THEIR PRODUCTS.
  *
  * <h2><center>&copy; COPYRIGHT 2011 STMicroelectronics</center></h2>
  ******************************************************************************
  */ 

/* Includes ------------------------------------------------------------------*/
#include "main.h"
#include "fonts.h"

/** @addtogroup Template_Project
  * @{
  */
/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/

/* Private function prototypes -----------------------------------------------*/
/* Private functions ---------------------------------------------------------*/

/**
  * @brief  Main program.
  * @param  None
  * @retval None
  */

void DevsInit(void)
{
	//LCD_Init();
    STM322xG_LCD_Init();
	LCD_Clear(LCD_COLOR_BLUE2);
	LCD_SetTextColor(LCD_COLOR_GREEN);
	LCD_SetBackColor(LCD_COLOR_BLUE2);
	LCD_DisplayStringLine(50,"    Hello World!!!     ");
	LCD_SetFont(&Font8x8);
	LCD_SetTextColor(LCD_COLOR_BLACK);
	LCD_DisplayStringLine(135,"               Soft: v1.0.0   ");
	LCD_SetFont(&Font8x12);
	LCD_SetTextColor(LCD_COLOR_BLACK);
	LCD_DisplayStringLine(110,"           Demo_Keil_LCD_SSD1289");
	LCD_SetFont(&Font12x12);
	LCD_SetTextColor(LCD_COLOR_GREY);
	LCD_DisplayStringLine(90,"           Linx_83        ");
	LCD_SetTextColor(LCD_COLOR_BLUE);
	LCD_DrawLine(230,60,50,LCD_DIR_HORIZONTAL);
	LCD_SetTextColor(LCD_COLOR_RED);
	LCD_DrawCircle(200,280,20);
	LCD_SetTextColor(LCD_COLOR_MAGENTA);
	LCD_DrawRect(160,80,50,50);
	LCD_SetTextColor(LCD_COLOR_BLUE);
	LCD_DrawFullRect(150,210,120,40);
	LCD_SetTextColor(LCD_COLOR_YELLOW);
	LCD_DrawFullCircle(150,250,20);
	LCD_DrawFullRect(190,210,120,40);
}

int main (void)
{
   DevsInit();
   while(1)
   {
   }
   return 0;
}


#ifdef  USE_FULL_ASSERT

/**
  * @brief  Reports the name of the source file and the source line number
  *   where the assert_param error has occurred.
  * @param  file: pointer to the source file name
  * @param  line: assert_param error line source number
  * @retval None
  */
void assert_failed(uint8_t* file, uint32_t line)
{ 
  /* User can add his own implementation to report the file name and line number,
     ex: printf("Wrong parameters value: file %s on line %d\r\n", file, line) */

  /* Infinite loop */
  while (1)
  {
  }
}
#endif

/**
  * @}
  */


/******************* (C) COPYRIGHT 2011 STMicroelectronics *****END OF FILE****/
