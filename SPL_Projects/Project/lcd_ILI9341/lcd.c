/////////////////////////////////////////////////////////////////////////
////////     проект для STM32F4-Discovery (STM32F407VGT6)     ///////////
////   дисплей на контроллере ILI9341, взят с платы STMF429I-Disco  /////
////////////      используется  StdPeriph Library Driver     ////////////
/////////////////////// Zlodey январь 2014 //////////////////////////////
/////////////////////////////////////////////////////////////////////////

// для запуска дисплея в режиме 16 бит, парраллельная шина, необходимо на ножки дисплея подать IM3=0, IM2=0, IM1=0, IM0=1
// шина данных дисплея 16 бит
// подключение выводов: Микроконтроллер <-> Дисплей
// GPIO D0  <-> LCD D2
// GPIO D1  <-> LCD D3
// GPIO D4  <-> LCD RD
// GPIO D5  <-> LCD WR
// GPIO D7  <-> LCD CS
// GPIO D8  <-> LCD D13
// GPIO D9  <-> LCD D14
// GPIO D10 <-> LCD D15
// GPIO D11 <-> LCD RS
// GPIO D14 <-> LCD D0
// GPIO D15 <-> LCD D1
// GPIO E7  <-> LCD D4
// GPIO E8  <-> LCD D5
// GPIO E9  <-> LCD D6
// GPIO E10 <-> LCD D7
// GPIO E11 <-> LCD D8
// GPIO E12 <-> LCD D9
// GPIO E13 <-> LCD D10
// GPIO E14 <-> LCD D11
// GPIO E15 <-> LCD D12
// выводы дисплея: TE, VSYNC, HSYNC, ENABLE, DOTCLK, SDA не используются
#define LCD_PORT GPIOC         // порт, к которому подключены управляющие выводы дисплея
#define LCD_RST GPIO_Pin_13     // PC4, вывод RST (reset) дисплея (0 дисплей в состоянии сброса, 1 рабочий режим)

#define GPIO_HIGH(a, b) a -> BSRRL = b  // теперь при помощи конструкции GPIO_HIGH(a, b) можно выдать лог.1 на порт "a" пин "b"
#define GPIO_LOW(a, b) a -> BSRRH = b   // теперь при помощи конструкции GPIO_LOW(a, b) можно выдать лог.0 на порт "a" пин "b"

// набор стандартных цветов, чтобы не оперировать HEX-значениями
#define RED    0xF800   // красный
#define GREEN  0x07E0   // зелёный
#define BLUE   0x001F   // синий
#define BLACK  0x0000   // чёрный
#define WHITE  0xFFFF   // белый


// подпрограмма задержки
void _delay_ms(uint32_t ms)
  {
    volatile uint32_t nCount;
    RCC_ClocksTypeDef RCC_Clocks;
    RCC_GetClocksFreq (&RCC_Clocks);
    
    nCount=(RCC_Clocks.HCLK_Frequency/10000)*ms;
    for (; nCount!=0; nCount--);
  }


// отправляет команду на дисплей (16 бит)
void LCD_write_comand(uint16_t cmd) 
  {
    *(__IO uint16_t *)(0x60000000) = cmd;	// этот адрес взял где то в инете
  }


// отправляет данные на дисплей (16 бит)
void LCD_write_data(uint16_t data) 
  {
    *(__IO uint16_t *)(0x60020000) = data;	// этот адрес взял где то в инете
  }


// Инициализируем FSMC контроллер, к которому подключен дисплей по шине Intel-8080 (D0...D15, RS, WR, RD, CS)
// Шина данных дисплея 16 бит
// Подключение дисплея к контроллеру:
// FSMC D0  <-> LCD D0
// FSMC D1  <-> LCD D1
// FSMC D2  <-> LCD D2
//         ...
// FSMC D15 <-> LCD D15
// FSMC NWE <-> LCD WR
// FSMC NOE <-> LCD RD
// FSMC NE1 <-> LCD CS
// FSMC A16 <-> LCD RS
void Init_FSMC(void)
  {
    // Объявляем структуру для инициализации GPIO
    GPIO_InitTypeDef GPIO_InitStructure;
    
    // Включить тактирование портов GPIOD, GPIOE, и AFIO
    RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOD | RCC_AHB1Periph_GPIOE |
                           RCC_AHB1Periph_GPIOC, ENABLE);
    
    // SRAM Data lines,  NOE and NWE configuration
    // GPIOD выводы настроим как выходы
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_0 | GPIO_Pin_1 | GPIO_Pin_4 | GPIO_Pin_5 |
      GPIO_Pin_7 | GPIO_Pin_8 | GPIO_Pin_9 |
        GPIO_Pin_10 |GPIO_Pin_11 | GPIO_Pin_14 | GPIO_Pin_15;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;                 // режим альтернативной функции
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;            // частота 50 МГц
    GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;               // двухтактный выход
    GPIO_InitStructure.GPIO_PuPd  = GPIO_PuPd_NOPULL;            // без подтяжки
    GPIO_Init(GPIOD, &GPIO_InitStructure);                       // выполняем инициализацию
    // GPIOD используются FSMC контроллером
    GPIO_PinAFConfig(GPIOD, GPIO_PinSource0, GPIO_AF_FSMC);      // GPIO D0  <-> FSMC D2
    GPIO_PinAFConfig(GPIOD, GPIO_PinSource1, GPIO_AF_FSMC);      // GPIO D1  <-> FSMC D3
    GPIO_PinAFConfig(GPIOD, GPIO_PinSource4, GPIO_AF_FSMC);      // GPIO D4  <-> FSMC NOE
    GPIO_PinAFConfig(GPIOD, GPIO_PinSource5, GPIO_AF_FSMC);      // GPIO D5  <-> FSMC NWE
    GPIO_PinAFConfig(GPIOD, GPIO_PinSource7, GPIO_AF_FSMC);      // GPIO D7  <-> FSMC NE1
    GPIO_PinAFConfig(GPIOD, GPIO_PinSource8, GPIO_AF_FSMC);      // GPIO D8  <-> FSMC D13
    GPIO_PinAFConfig(GPIOD, GPIO_PinSource9, GPIO_AF_FSMC);      // GPIO D9  <-> FSMC D14
    GPIO_PinAFConfig(GPIOD, GPIO_PinSource10, GPIO_AF_FSMC);     // GPIO D10 <-> FSMC D15
    GPIO_PinAFConfig(GPIOD, GPIO_PinSource11, GPIO_AF_FSMC);     // GPIO D11 <-> FSMC A16
    GPIO_PinAFConfig(GPIOD, GPIO_PinSource14, GPIO_AF_FSMC);     // GPIO D14 <-> FSMC D0
    GPIO_PinAFConfig(GPIOD, GPIO_PinSource15, GPIO_AF_FSMC);     // GPIO D15 <-> FSMC D1
    
    // GPIOE выводы настроим как выходы
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_7 | GPIO_Pin_8 | GPIO_Pin_9 | GPIO_Pin_10 |
      GPIO_Pin_11 | GPIO_Pin_12 | GPIO_Pin_13 | GPIO_Pin_14 | 
        GPIO_Pin_15;
    GPIO_Init(GPIOE, &GPIO_InitStructure);
    // GPIOE используются FSMC контроллером
    GPIO_PinAFConfig(GPIOE, GPIO_PinSource7 , GPIO_AF_FSMC);     // GPIO E7  <-> FSMC D4
    GPIO_PinAFConfig(GPIOE, GPIO_PinSource8 , GPIO_AF_FSMC);     // GPIO E8  <-> FSMC D5
    GPIO_PinAFConfig(GPIOE, GPIO_PinSource9 , GPIO_AF_FSMC);     // GPIO E9  <-> FSMC D6
    GPIO_PinAFConfig(GPIOE, GPIO_PinSource10 , GPIO_AF_FSMC);    // GPIO E10 <-> FSMC D7
    GPIO_PinAFConfig(GPIOE, GPIO_PinSource11 , GPIO_AF_FSMC);    // GPIO E11 <-> FSMC D8
    GPIO_PinAFConfig(GPIOE, GPIO_PinSource12 , GPIO_AF_FSMC);    // GPIO E12 <-> FSMC D9
    GPIO_PinAFConfig(GPIOE, GPIO_PinSource13 , GPIO_AF_FSMC);    // GPIO E13 <-> FSMC D10
    GPIO_PinAFConfig(GPIOE, GPIO_PinSource14 , GPIO_AF_FSMC);    // GPIO E14 <-> FSMC D11
    GPIO_PinAFConfig(GPIOE, GPIO_PinSource15 , GPIO_AF_FSMC);    // GPIO E15 <-> FSMC D12
    
    // Настройка контроллера FSMC
    FSMC_NORSRAMInitTypeDef  FSMC_NORSRAMInitStructure;
    FSMC_NORSRAMTimingInitTypeDef  p;
    
    // Включить тактирование FSMC
    RCC_AHB3PeriphClockCmd(RCC_AHB3Periph_FSMC, ENABLE);
    
    // SRAM Bank 1
    // Конфигурация FSMC_Bank1_NORSRAM1
    p.FSMC_AddressSetupTime = 0x6;
    p.FSMC_AddressHoldTime = 0;
    p.FSMC_DataSetupTime = 0x6;
    p.FSMC_BusTurnAroundDuration = 0;
    p.FSMC_CLKDivision = 0;
    p.FSMC_DataLatency = 0;
    p.FSMC_AccessMode = FSMC_AccessMode_A;
    // Color LCD configuration
    //   LCD configured as follow:
    //      - Data/Address MUX = Disable
    //      - Memory Type = SRAM
    //      - Data Width = 16bit
    //      - Write Operation = Enable
    //      - Extended Mode = Enable
    //      - Asynchronous Wait = Disable */
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
    FSMC_NORSRAMInitStructure.FSMC_ReadWriteTimingStruct = &p;
    FSMC_NORSRAMInitStructure.FSMC_WriteTimingStruct = &p;
    
    FSMC_NORSRAMInit(&FSMC_NORSRAMInitStructure);   
    
    // Enable FSMC NOR/SRAM Bank1
    FSMC_NORSRAMCmd(FSMC_Bank1_NORSRAM1, ENABLE);
  }


// инициализируем дисплей
void Init_LCD(void)
  {
    GPIO_InitTypeDef GPIO_InitStructure;                 // структура инициализации

    RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOC, ENABLE);

    GPIO_InitStructure.GPIO_Pin = LCD_RST;            // настраиваем только некоторые выводы порта
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;    // частота работы порта
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;        // режим - выход
    GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;       // пуш-пулл
    GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;     // без подтягивающих резисторов
    GPIO_Init(GPIOC, &GPIO_InitStructure);               // запуск настройки
    
    // управляющие выводы дисплея
    GPIO_LOW(LCD_PORT, LCD_RST);     // лог.0 на вывод RESET
    _delay_ms(50);
    // начинаем инициализацию LCD
    GPIO_HIGH(LCD_PORT, LCD_RST);    // лог.1 на вывод RESET
    _delay_ms(50);
    LCD_write_comand (0x01); // software reset comand
    _delay_ms(5);
    LCD_write_comand (0x28); // display off
    //------------power control------------------------------
    LCD_write_comand (0xc0); // power control
    LCD_write_data   (0x26); // GVDD = 4.75v
    LCD_write_comand (0xc1); // power control
    LCD_write_data   (0x11); // AVDD=VCIx2, VGH=VCIx7, VGL=-VCIx3
    //--------------VCOM-------------------------------------
    LCD_write_comand (0xc5); // vcom control
    LCD_write_data   (0x35); // Set the VCOMH voltage (0x35 = 4.025v)
    LCD_write_data   (0x3e); // Set the VCOML voltage (0x3E = -0.950v)
    LCD_write_comand (0xc7); // vcom control
    LCD_write_data   (0xbe); // 0x94 (0xBE = nVM: 1, VCOMH: VMH–2, VCOML: VML–2)
    //------------memory access control------------------------
    LCD_write_comand (0x36); // memory access control
    LCD_write_data   (0x48); // 0048 my,mx,mv,ml,BGR,mh,0.0 (mirrors)
    LCD_write_comand (0x3a); // pixel format set
    LCD_write_data   (0x55); // 16bit /pixel
    //-------------ddram ----------------------------
    LCD_write_comand (0x2a); // column set
    LCD_write_data   (0x00); // x0_HIGH---0
    LCD_write_data   (0x00); // x0_LOW----0
    LCD_write_data   (0x00); // x1_HIGH---240
    LCD_write_data   (0xEF); // x1_LOW----240
    LCD_write_comand (0x2b); // page address set
    LCD_write_data   (0x00); // y0_HIGH---0
    LCD_write_data   (0x00); // y0_LOW----0
    LCD_write_data   (0x01); // y1_HIGH---320
    LCD_write_data   (0x3F); // y1_LOW----320
    LCD_write_comand (0x34); // tearing effect off
    //LCD_write_cmd(0x35); // tearing effect on
    //LCD_write_cmd(0xb4); // display inversion
    LCD_write_comand (0xb7); // entry mode set
    // Deep Standby Mode: OFF
    // Set the output level of gate driver G1~G320: Normal display
    // Low voltage detection: Disable
    LCD_write_data   (0x07); 
    //-----------------display------------------------
    LCD_write_comand (0xb6); // display function control
    //Set the scan mode in non-display area
    //Determine source/VCOM output in a non-display area in the partial display mode
    LCD_write_data   (0x0a);
    //Select whether the liquid crystal type is normally white type or normally black type
    //Sets the direction of scan by the gate driver in the range determined by SCN and NL
    //Select the shift direction of outputs from the source driver
    //Sets the gate driver pin arrangement in combination with the GS bit to select the optimal scan mode for the module
    //Specify the scan cycle interval of gate driver in non-display area when PTG to select interval scan
    LCD_write_data   (0x82);
    // Sets the number of lines to drive the LCD at an interval of 8 lines
    LCD_write_data   (0x27); 
    LCD_write_data   (0x00); // clock divisor
    LCD_write_comand (0x11); // sleep out
    _delay_ms(100);
    LCD_write_comand (0x29); // display on
    _delay_ms(100);
    LCD_write_comand (0x2c); // memory write
    _delay_ms(5);
  }


// Заливка дисплея чёрным
void LCD_Clear(uint16_t color)  
  {     uint32_t y;
  for (y = 0; y < 76800; y++)       // на дисплее с разрешениием 240*320 всего 76800 пикселей
    {
      LCD_write_data(color);       // RRRRRGGGGGGBBBBB
    }
  }