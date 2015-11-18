/**
******************************************************************************
* @file    Project/STM32F4xx_StdPeriph_Templates/main.c 
* @author  MCD Application Team
* @version V1.6.0
* @date    04-September-2015
* @brief   Main program body
******************************************************************************
* @attention
*
* <h2><center>&copy; COPYRIGHT 2015 STMicroelectronics</center></h2>
*
* Licensed under MCD-ST Liberty SW License Agreement V2, (the "License");
* You may not use this file except in compliance with the License.
* You may obtain a copy of the License at:
*
*        http://www.st.com/software_license_agreement_liberty_v2
*
* Unless required by applicable law or agreed to in writing, software 
* distributed under the License is distributed on an "AS IS" BASIS, 
* WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
* See the License for the specific language governing permissions and
* limitations under the License.
*
******************************************************************************
*/

/* Includes ------------------------------------------------------------------*/
#include "main.h"

#include "stm324xg_eval_lcd.h"
#include "cmsis_os.h"

#include "tulips.c"

/** @addtogroup Template_Project
* @{
*/ 

/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
__IO uint32_t uwTimingDelay;

osThreadId defaultTaskHandle;
RCC_ClocksTypeDef RCC_Clocks;


/* Private function prototypes -----------------------------------------------*/
static void Delay(__IO uint32_t nTime);

/* Private functions ---------------------------------------------------------*/


/* FreeRTOS tasks */
void StartDefaultTask(void const * argument);


void xTIM_Init()
  {
    TIM_TimeBaseInitTypeDef  TIM_TimeBaseStructure;
    TIM_OCInitTypeDef  TIM_OCInitStructure;    
        
    RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM4, ENABLE);
    
    /* Time base configuration */
    TIM_TimeBaseStructure.TIM_Period = 65535;
    TIM_TimeBaseStructure.TIM_Prescaler = (uint16_t) ((SystemCoreClock/2) / 21000000) - 1;
    TIM_TimeBaseStructure.TIM_ClockDivision = 0;
    TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;
    
    TIM_TimeBaseInit(TIM4, &TIM_TimeBaseStructure);
    
    /* PWM1 Mode configuration: Channel1 */
    TIM_OCInitStructure.TIM_OCMode = TIM_OCMode_PWM1;
    TIM_OCInitStructure.TIM_OutputState = TIM_OutputState_Enable;
    TIM_OCInitStructure.TIM_Pulse = 16383;
    TIM_OCInitStructure.TIM_OCPolarity = TIM_OCPolarity_High;    
    TIM_OC1Init(TIM4, &TIM_OCInitStructure);
    
    TIM_OC1PreloadConfig(TIM4, TIM_OCPreload_Disable);
    
    
    /* Output Compare Mode configuration: Channel2 */
    TIM_OCInitStructure.TIM_OCMode = TIM_OCMode_PWM1;
    TIM_OCInitStructure.TIM_OutputState = TIM_OutputState_Enable;
    TIM_OCInitStructure.TIM_Pulse = 40000;
    TIM_OC2Init(TIM4, &TIM_OCInitStructure);
    
    TIM_OC2PreloadConfig(TIM4, TIM_OCPreload_Disable);    
    
    //TIM_ARRPreloadConfig(TIM4, ENABLE);
    
    /* TIM4 enable counter */
    TIM_Cmd(TIM4, ENABLE);    
  }


void xGPIO_Init()
  {
    GPIO_InitTypeDef  GPIO_InitStructure;    
    
    /* GPIOD Peripheral clock enable */
    RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOB, ENABLE);
    RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOC, ENABLE);
    RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOD, ENABLE);
    
    /* LCD backlight pin */
    GPIO_InitStructure.GPIO_Pin = LCD_BL_PIN;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;
    GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;
    GPIO_Init(LCD_BL_PORT, &GPIO_InitStructure);
    
    /* Connect TIM4 pins to AF2 */ 
    GPIO_PinAFConfig(LCD_BL_PORT, GPIO_PinSource12, GPIO_AF_TIM4);
    
    /* Beeper pin */
    GPIO_InitStructure.GPIO_Pin = BEEPER_PIN;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;
    GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;
    GPIO_Init(BEEPER_PORT, &GPIO_InitStructure);
    
    /* Connect TIM4 pins to AF2 */ 
    GPIO_PinAFConfig(BEEPER_PORT, GPIO_PinSource7, GPIO_AF_TIM4); 
    
    /* LCD RST pin */
    GPIO_InitStructure.GPIO_Pin = LCD_RST_PIN;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;
    GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_2MHz;
    GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;
    GPIO_Init(LCD_RST_PORT, &GPIO_InitStructure);
  }




/**
* @brief  Main program
* @param  None
* @retval None
*/
int main(void)
  {
    SystemCoreClockUpdate();
    
    /* SysTick end of count event each 10ms */
    RCC_GetClocksFreq(&RCC_Clocks);
    SysTick_Config(RCC_Clocks.HCLK_Frequency / 1000);
    
    /* Add your application code here */
    /* Insert 50 ms delay */
    Delay(5);
    
    xTIM_Init();   
    xGPIO_Init();  
    
    STM_EVAL_LEDInit(LED3);
    STM_EVAL_PBInit(BUTTON_KEY, BUTTON_MODE_GPIO);
    
    LCD_Init();
    LCD_Clear(COLOR.BLUE);
    
    LCD_SetTextColor(COLOR.WHITE);
    
    LCD_DrawLine(10, 10, 200, LCD_DIR_HORIZONTAL);
    LCD_DrawLine(10, 10, 300, LCD_DIR_VERTICAL);
    
    LCD_SetTextColor(COLOR.RED);
    LCD_DrawRect(20, 20, 50, 50); 
    
    LCD_SetBackColor(COLOR.GREEN);
    LCD_DrawFullRect(75, 20, 50, 50);
    
    LCD_DrawCircle(50, 100, 20);
    LCD_DrawFullCircle(100, 100, 20);
    
    LCD_DrawUniLine(150, 150, 180, 200);
    
    Delay(1000);
    
    LCD_WriteBMP((uint32_t) image_data_Tulips);
    Delay(100);
    
    
    LCD_SetFont(&Font8x8);
    
    LCD_SetTextColor(COLOR.WHITE);
    LCD_SetBackColor(COLOR.BLUE);
   
    //LCD_DisplayChar(100, 100, 'Q');
    //LCD_DisplayStringLine(20, "QWERTY");
    
    XPT2046_Init();
    Read_XPT2046(); //dummy read    
    
    
    /* Create the thread(s) */
    /* definition and creation of defaultTask */
    osThreadDef(defaultTask, StartDefaultTask, osPriorityNormal, 0, 128);
    defaultTaskHandle = osThreadCreate(osThread(defaultTask), NULL);

    /* Start scheduler */
    osKernelStart();
    
   
    /* Infinite loop */
    while (1);
  }

/**
* @brief  Inserts a delay time.
* @param  nTime: specifies the delay time length, in milliseconds.
* @retval None
*/
static void Delay(__IO uint32_t nTime)
  { 
    uwTimingDelay = nTime;
    
    while(uwTimingDelay != 0);
  }




/* StartDefaultTask function */
void StartDefaultTask(void const * argument)
{
  /* Infinite loop */
  for(;;)
  {    
    Coordinate *coord;
    char str[10];

    while(!XPT2046_Press())
      {
        //TIM4->CCER |= TIM_CCER_CC2E;    // beeper enable   
        
        coord = Read_XPT2046();
        
        if(coord != 0x00)
          {
            uint16tostr(str, coord->x, 10);        
            LCD_DisplayStringLine(20, (uint8_t*)str);
            
            uint16tostr(str, coord->y, 10);        
            LCD_DisplayStringLine(30, (uint8_t*)str);
            
            LCD_DrawCircle(((coord->x)/14.4)-26, ((coord->y)/11)-30, 1);
      
          }        
        osDelay(1);
      }
    
    //TIM4->CCER &= ~(TIM_CCER_CC2E | TIM_CCER_CC2P);     //beeper disable, clear polarity
    
    STM_EVAL_LEDToggle(LED3);
    if(STM_EVAL_PBGetState(BUTTON_WAKEUP))
      {
      }
    else
      {
      }
    
    osDelay(20);
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
    
    /* Infinite loop */
    while (1)
      {
      }
  }
#endif

/**
* @}
*/


/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
