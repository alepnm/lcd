// FSMC_LowLevel.c - низкоуровневый драйвер FSMC для LCD
// STM32F103ZET6

#include <stdint.h>
#include <stdbool.h>

#include "stm32f4xx.h"
#include "stm32f4xx_gpio.h"
#include "stm32f4xx_fsmc.h"
#include "stm32f4xx_rcc.h"
#include "stm32f4xx_dma.h"
#include "misc.h"

#include "FSMC_LowLevel.h"

//#define DMA_CHANNEL              DMA2_Channel4
//#define DMA_STREAM_CLOCK         RCC_AHBPeriph_DMA2
//#define DMA_STREAM_STATUS_BIT	 DMA2_FLAG_TC4


// Для SSD1963, SSD1280, ILI9320
//  RS = 1 - данные
//  RS = 0 - команды
typedef struct
{
  volatile uint16_t LCD_REG;
  volatile uint16_t LCD_RAM;
} LCD_TypeDef;

// Note: LCD /CS is NE1 - Bank 1 of NOR/SRAM Bank 1~4
//#define LCD_BASE           ((uint32_t)(0x60000000 | 0x0001fffE))
#define LCD_BASE           ((uint32_t)(0x6001fffE))
#define LCD                ((LCD_TypeDef *) LCD_BASE)

// Конфигурирование ног FSMC
static void FSMC_LinesConfig ( void );
// Настройка FSMC
static void FSMC_Config ( void );

// флаг завершения DMA передачи
//static volatile bool isDmaTransferOk;

// Инициализация FSMC
void FSMC_Init ( void )
{
	// Конфигурирование ног FSMC
	FSMC_LinesConfig ( );

	// Настройка FSMC
	FSMC_Config ( );

//	isDmaTransferOk = true;
} // FSMC_Init

// Запись в регистр LCD
void FSMC_LcdWriteReg ( uint8_t LCD_Reg, uint16_t LCD_RegValue )
{
	// Write 16-bit Index, then Write Reg
	LCD->LCD_REG = LCD_Reg;
	// Write 16-bit Reg
	LCD->LCD_RAM = LCD_RegValue;
} // FSMC_LcdWriteReg

// Чтение регистра LCD
uint16_t FSMC_LcdReadReg ( uint8_t LCD_Reg )
{
	uint16_t val;

	// Write 16-bit Index (then Read Reg)
	LCD -> LCD_REG = LCD_Reg;
	// Read 16-bit Reg
	val = LCD -> LCD_RAM;

	return val;
} // FSMC_LcdReadReg

// Запись команды в LCD
void FSMC_LcdWriteCmd ( uint16_t val )
{
	// Write 16-bit Index (then Read Reg)
	LCD->LCD_REG = val;
} // FSMC_LcdWriteCmd

// Запись данных в LCD
void FSMC_LcdWriteData ( uint16_t val )
{
	// Write 16-bit Reg
	LCD -> LCD_RAM = val;
} // FSMC_LcdWriteData

// Чтение данных из LCD
uint16_t FSMC_LcdReadData ( void )
{
	return LCD -> LCD_RAM;
} // FSMC_LcdReadData

// Передача блока данных по DMA в LCD
// count - длина данных в байтах, д.б. кратна 4
void FSMC_TransferDataDMAToLCD ( void *buffer, uint16_t count, bool isRamAddressIncrement )
{
    DMA_InitTypeDef DMA_InitStructure;

    RCC_AHB1PeriphClockCmd ( RCC_AHB1Periph_DMA2, ENABLE );

    // DMA2_Channel4
    DMA_DeInit ( DMA2_Channel4 );
    DMA_StructInit (&DMA_InitStructure);

    DMA_InitStructure.DMA_BufferSize = count;
    DMA_InitStructure.DMA_DIR = DMA_DIR_MemoryToMemory;
//    DMA_InitStructure.DMA_M2M = DMA_M2M_Enable;

    DMA_InitStructure.DMA_Memory0BaseAddr = (uint32_t)(&(LCD->LCD_RAM));
    DMA_InitStructure.DMA_MemoryDataSize = DMA_MemoryDataSize_HalfWord;
   	DMA_InitStructure.DMA_MemoryInc = DMA_MemoryInc_Disable;

    DMA_InitStructure.DMA_Mode = DMA_Mode_Normal;

    DMA_InitStructure.DMA_PeripheralBaseAddr = (uint32_t) buffer;
    DMA_InitStructure.DMA_PeripheralDataSize = DMA_PeripheralDataSize_HalfWord;
//    DMA_InitStructure.DMA_PeripheralInc = DMA_PeripheralInc_Enable;
    if ( isRamAddressIncrement )
    	DMA_InitStructure.DMA_MemoryInc = DMA_PeripheralInc_Enable;
    else
    	DMA_InitStructure.DMA_MemoryInc = DMA_PeripheralInc_Disable;

    DMA_InitStructure.DMA_Priority = DMA_Priority_Low;

    // DMA2_Channel4
    DMA_Init ( DMA2_Channel4, &DMA_InitStructure );
    DMA_Cmd ( DMA2_Channel4, ENABLE );

    // Ждать конца передачи
    // DMA2_FLAG_TC4 - DMA2Channel4 Transfer complete
    while ( !DMA_GetFlagStatus ( DMA2_FLAG_TC4 ) );
    DMA_ClearFlag ( DMA2_FLAG_TC4 );
} // FSMC_TransferDataDMA


/////////////////////////////////////////////////////////////
// Внутренние функции

// Конфигурирование ног FSMC
static void FSMC_LinesConfig ( void )
{
	GPIO_InitTypeDef GPIO_InitStructure;

	// Enable GPIOD, GPIOE clocks
	RCC_APB2PeriphClockCmd ( RCC_APB2Periph_GPIOD | RCC_APB2Periph_GPIOE, ENABLE );

	// -- GPIO Configuration ------------------------------------------------------
	// LCD DB15-DB0       PD10..PD8, PE15..PE7, PD1, PD0, PD15, PD14
	// LCD nCS  (7)       NCE2 (PD7)
	// LCD nRD  (10)      NOE  (PD4)
	// LCD nWR  (9)       NWE  (PD5)
	// LCD RS   (8)       CLE  (PD11)

	// SRAM Data lines,  NOE and NWE configuration
	// PD0, PD1, PD4, PD5, PD7, PD8, PD9, PD10, PD11, PD14, PD15
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_0 | GPIO_Pin_1 | GPIO_Pin_4 | GPIO_Pin_5 | GPIO_Pin_7 |
			GPIO_Pin_8 | GPIO_Pin_9 | GPIO_Pin_10 |	GPIO_Pin_11 | GPIO_Pin_14 | GPIO_Pin_15;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
	GPIO_Init ( GPIOD, &GPIO_InitStructure );

	// PE7..PE15
	// PE7, PE8, PE9, PE10, PE11, PE12, PE13, PE14, PE15
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_7 | GPIO_Pin_8 | GPIO_Pin_9 | GPIO_Pin_10 |
			GPIO_Pin_11 | GPIO_Pin_12 | GPIO_Pin_13 | GPIO_Pin_14 | GPIO_Pin_15;
	GPIO_Init ( GPIOE, &GPIO_InitStructure );
} // FSMC_LinesConfig

// Настройка FSMC
static void FSMC_Config ( void )
{
	FSMC_NORSRAMInitTypeDef FSMC_NORSRAMInitStructure;
	FSMC_NORSRAMTimingInitTypeDef TimingWriteInitStructure;

	// Enable the FSMC Clock
	RCC_AHBPeriphClockCmd ( RCC_AHBPeriph_FSMC, ENABLE );

	//-- FSMC Configuration ---------------------------------------------------
	TimingWriteInitStructure.FSMC_AddressSetupTime = 2;
	TimingWriteInitStructure.FSMC_AddressHoldTime = 0;
	TimingWriteInitStructure.FSMC_DataSetupTime = 10;
	TimingWriteInitStructure.FSMC_BusTurnAroundDuration = 0;
	TimingWriteInitStructure.FSMC_CLKDivision = 0;
	TimingWriteInitStructure.FSMC_DataLatency = 0;
	TimingWriteInitStructure.FSMC_AccessMode = FSMC_AccessMode_A;

	FSMC_NORSRAMInitStructure.FSMC_Bank = FSMC_Bank1_NORSRAM1;
	FSMC_NORSRAMInitStructure.FSMC_DataAddressMux = FSMC_DataAddressMux_Disable;
	FSMC_NORSRAMInitStructure.FSMC_MemoryType = FSMC_MemoryType_SRAM;
	FSMC_NORSRAMInitStructure.FSMC_MemoryDataWidth = FSMC_MemoryDataWidth_16b;
	FSMC_NORSRAMInitStructure.FSMC_BurstAccessMode = FSMC_BurstAccessMode_Disable;
	FSMC_NORSRAMInitStructure.FSMC_AsynchronousWait = FSMC_AsynchronousWait_Disable;
	FSMC_NORSRAMInitStructure.FSMC_WaitSignalPolarity = FSMC_WaitSignalPolarity_Low;
	FSMC_NORSRAMInitStructure.FSMC_WrapMode = FSMC_WrapMode_Disable;
	FSMC_NORSRAMInitStructure.FSMC_WaitSignalActive = FSMC_WaitSignalActive_BeforeWaitState;
	FSMC_NORSRAMInitStructure.FSMC_WriteOperation = FSMC_WriteOperation_Enable;
	FSMC_NORSRAMInitStructure.FSMC_WaitSignal = FSMC_WaitSignal_Disable;
	FSMC_NORSRAMInitStructure.FSMC_ExtendedMode = FSMC_ExtendedMode_Disable;
	FSMC_NORSRAMInitStructure.FSMC_WriteBurst = FSMC_WriteBurst_Disable;
	FSMC_NORSRAMInitStructure.FSMC_ReadWriteTimingStruct = &TimingWriteInitStructure;
	FSMC_NORSRAMInitStructure.FSMC_WriteTimingStruct = &TimingWriteInitStructure;

	FSMC_NORSRAMInit ( &FSMC_NORSRAMInitStructure );
	FSMC_NORSRAMCmd ( FSMC_Bank1_NORSRAM1, ENABLE );
} // FSMC_Config
