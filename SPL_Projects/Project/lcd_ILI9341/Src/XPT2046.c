/* Includes ------------------------------------------------------------------*/
#include "XPT2046.h"

/* Private variables ---------------------------------------------------------*/
Matrix        matrix;
Coordinate    display;
Coordinate    ScreenSample[3];
Coordinate    DisplaySample[3] = { {30, 45}, {290, 45}, {160, 210} };

/* Private define ------------------------------------------------------------*/
#define THRESHOLD 10

/* Private function prototypes -----------------------------------------------*/
static void Delay(__IO uint32_t nTime);

__IO uint32_t uwTS_Delay;

void XPT2046_Init(void) 
  { 
    GPIO_InitTypeDef GPIO_InitStruct;
    SPI_InitTypeDef  SPI_InitStructure;    
    EXTI_InitTypeDef   EXTI_InitStruct;
    NVIC_InitTypeDef   NVIC_InitStruct;
    
    /* Configure GPIOs ---------------------------------------------------------*/
    RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOB, ENABLE);
    
    /* Configure SPI2 pins: SCK, MISO and MOSI ---------------------------------*/ 
    GPIO_InitStruct.GPIO_Pin = XPT2046_SCK_PIN | XPT2046_MISO_PIN | XPT2046_MOSI_PIN;
    GPIO_InitStruct.GPIO_Mode = GPIO_Mode_AF;
    GPIO_InitStruct.GPIO_OType = GPIO_OType_PP;
    GPIO_InitStruct.GPIO_Speed = GPIO_Speed_100MHz;
    GPIO_InitStruct.GPIO_PuPd = GPIO_PuPd_NOPULL;
	GPIO_Init(XPT2046_PORT, &GPIO_InitStruct);
    
    GPIO_PinAFConfig(XPT2046_PORT, GPIO_PinSource13, GPIO_AF_SPI2);
    GPIO_PinAFConfig(XPT2046_PORT, GPIO_PinSource14, GPIO_AF_SPI2);
    GPIO_PinAFConfig(XPT2046_PORT, GPIO_PinSource15, GPIO_AF_SPI2);
    
    /* TP_CS */
    GPIO_InitStruct.GPIO_Pin = XPT2046_CS_PIN;
	GPIO_InitStruct.GPIO_Mode = GPIO_Mode_OUT;
    GPIO_InitStruct.GPIO_OType = GPIO_OType_PP;
    GPIO_InitStruct.GPIO_Speed = GPIO_Speed_100MHz;
    GPIO_InitStruct.GPIO_PuPd = GPIO_PuPd_NOPULL;
	GPIO_Init(XPT2046_PORT, &GPIO_InitStruct);
    
    /* TP_IRQ */
    GPIO_InitStruct.GPIO_Pin =  XPT2046_IRQ_PIN;
    GPIO_InitStruct.GPIO_Mode = GPIO_Mode_IN;
    GPIO_InitStruct.GPIO_OType = GPIO_OType_PP;
    GPIO_InitStruct.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_InitStruct.GPIO_PuPd = GPIO_PuPd_NOPULL;
    GPIO_Init(XPT2046_PORT, &GPIO_InitStruct);

    SYSCFG_EXTILineConfig(EXTI_PortSourceGPIOB, EXTI_PinSource12);

    EXTI_InitStruct.EXTI_Line = EXTI_Line12;
    EXTI_InitStruct.EXTI_Mode = EXTI_Mode_Interrupt;
    EXTI_InitStruct.EXTI_Trigger = EXTI_Trigger_Falling;  
    EXTI_InitStruct.EXTI_LineCmd = ENABLE;
    EXTI_Init(&EXTI_InitStruct);

    NVIC_InitStruct.NVIC_IRQChannel = EXTI15_10_IRQn;
    NVIC_InitStruct.NVIC_IRQChannelPreemptionPriority = 0x01;
    NVIC_InitStruct.NVIC_IRQChannelSubPriority = 0x01;
    NVIC_InitStruct.NVIC_IRQChannelCmd = ENABLE;
    NVIC_Init(&NVIC_InitStruct);
    
    
    set_CS(); 
    
    /* Configure SPI -----------------------------------------------------------*/
    RCC_APB1PeriphClockCmd(RCC_APB1Periph_SPI2, ENABLE);
    /* DISABLE SPI2 */ 
    SPI_Cmd(SPI2, DISABLE); 
    /* SPI2 Config -------------------------------------------------------------*/ 
    SPI_InitStructure.SPI_Direction = SPI_Direction_2Lines_FullDuplex; 
    SPI_InitStructure.SPI_Mode = SPI_Mode_Master; 
    SPI_InitStructure.SPI_DataSize = SPI_DataSize_8b; 
    SPI_InitStructure.SPI_CPOL = SPI_CPOL_High; 
    SPI_InitStructure.SPI_CPHA = SPI_CPHA_2Edge; 
    SPI_InitStructure.SPI_NSS = SPI_NSS_Soft; 
    SPI_InitStructure.SPI_BaudRatePrescaler = SPI_BaudRatePrescaler_128; 
    SPI_InitStructure.SPI_FirstBit = SPI_FirstBit_MSB; 
    SPI_InitStructure.SPI_CRCPolynomial = 7; 
    SPI_Init(SPI2, &SPI_InitStructure); 
    /* Enable SPI2 */ 
    SPI_Cmd(SPI2, ENABLE); 
  } 

uint16_t Read_X(void)  
  {  
    uint16_t curr_X;
    
    reset_CS(); 
    Delay(1); 
    WR_CMD(CHX); 
    Delay(1); 
    curr_X=RD_AD(); 
    set_CS(); 
    return curr_X;    
  } 

uint16_t Read_Y(void)  
  {  
    uint16_t curr_Y; 
    
    reset_CS(); 
    Delay(1); 
    WR_CMD(CHY); 
    Delay(1); 
    curr_Y=RD_AD(); 
    set_CS(); 
    return curr_Y;     
  } 

void XPT2046_GetAdXY(int *x,int *y)  
  { 
    uint16_t adx,ady; 
    adx=Read_X(); 
    Delay(1); 
    ady=Read_Y(); 
    *x=adx; 
    *y=ady; 
  } 

Coordinate *Read_XPT2046(void)
  {
    static Coordinate  screen;
    int m0,m1,m2,TP_X[1],TP_Y[1],temp[3];
    uint8_t count=0;
    int buffer[2][9]={{0},{0}};
    
    do
      {		   
        XPT2046_GetAdXY(TP_X,TP_Y);  
        buffer[0][count]=TP_X[0];  
        buffer[1][count]=TP_Y[0];
        count++;  
      }
    while(!read_IRQ()&& count<9);  /* TP_INT_IN  */
    if(count==9)   /* Average X Y  */ 
      {
        /* Average X  */
        temp[0]=(buffer[0][0]+buffer[0][1]+buffer[0][2])/3;
        temp[1]=(buffer[0][3]+buffer[0][4]+buffer[0][5])/3;
        temp[2]=(buffer[0][6]+buffer[0][7]+buffer[0][8])/3;
        
        m0=temp[0]-temp[1];
        m1=temp[1]-temp[2];
        m2=temp[2]-temp[0];
        
        m0=m0>0?m0:(-m0);
        m1=m1>0?m1:(-m1);
        m2=m2>0?m2:(-m2);
        
        if( m0>THRESHOLD  &&  m1>THRESHOLD  &&  m2>THRESHOLD ) return 0;
        
        if(m0<m1)
          {
            if(m2<m0) 
              screen.x=(temp[0]+temp[2])/2;
            else 
              screen.x=(temp[0]+temp[1])/2;	
          }
        else if(m2<m1) 
          screen.x=(temp[0]+temp[2])/2;
        else 
          screen.x=(temp[1]+temp[2])/2;
        
        /* Average Y  */
        temp[0]=(buffer[1][0]+buffer[1][1]+buffer[1][2])/3;
        temp[1]=(buffer[1][3]+buffer[1][4]+buffer[1][5])/3;
        temp[2]=(buffer[1][6]+buffer[1][7]+buffer[1][8])/3;
        
        m0=temp[0]-temp[1];
        m1=temp[1]-temp[2];
        m2=temp[2]-temp[0];
        
        m0=m0>0?m0:(-m0);
        m1=m1>0?m1:(-m1);
        m2=m2>0?m2:(-m2);
        
        if(m0>THRESHOLD && m1>THRESHOLD && m2>THRESHOLD) return 0;
        
        if(m0<m1)
          {
            if(m2<m0) 
              screen.y=(temp[0]+temp[2])/2;
            else 
              screen.y=(temp[0]+temp[1])/2;	
          }
        else if(m2<m1) 
          screen.y=(temp[0]+temp[2])/2;
        else
          screen.y=(temp[1]+temp[2])/2;
        
        return &screen;
      }  
    return 0; 
  }

FunctionalState setCalibrationMatrix( Coordinate * displayPtr,
                                     Coordinate * screenPtr,
                                     Matrix * matrixPtr)
  {
    
    FunctionalState retTHRESHOLD = ENABLE ;
    /* K��(X0��X2) (Y1��Y2)��(X1��X2) (Y0��Y2) */
    matrixPtr->Divider = ((screenPtr[0].x - screenPtr[2].x) * (screenPtr[1].y - screenPtr[2].y)) - 
      ((screenPtr[1].x - screenPtr[2].x) * (screenPtr[0].y - screenPtr[2].y)) ;
    if( matrixPtr->Divider == 0 )
      {
        retTHRESHOLD = DISABLE;
      }
    else
      {
        /* A��((XD0��XD2) (Y1��Y2)��(XD1��XD2) (Y0��Y2))��K	*/
        matrixPtr->An = ((displayPtr[0].x - displayPtr[2].x) * (screenPtr[1].y - screenPtr[2].y)) - 
          ((displayPtr[1].x - displayPtr[2].x) * (screenPtr[0].y - screenPtr[2].y)) ;
        /* B��((X0��X2) (XD1��XD2)��(XD0��XD2) (X1��X2))��K	*/
        matrixPtr->Bn = ((screenPtr[0].x - screenPtr[2].x) * (displayPtr[1].x - displayPtr[2].x)) - 
          ((displayPtr[0].x - displayPtr[2].x) * (screenPtr[1].x - screenPtr[2].x)) ;
        /* C��(Y0(X2XD1��X1XD2)+Y1(X0XD2��X2XD0)+Y2(X1XD0��X0XD1))��K */
        matrixPtr->Cn = (screenPtr[2].x * displayPtr[1].x - screenPtr[1].x * displayPtr[2].x) * screenPtr[0].y +
          (screenPtr[0].x * displayPtr[2].x - screenPtr[2].x * displayPtr[0].x) * screenPtr[1].y +
            (screenPtr[1].x * displayPtr[0].x - screenPtr[0].x * displayPtr[1].x) * screenPtr[2].y ;
        /* D��((YD0��YD2) (Y1��Y2)��(YD1��YD2) (Y0��Y2))��K	*/
        matrixPtr->Dn = ((displayPtr[0].y - displayPtr[2].y) * (screenPtr[1].y - screenPtr[2].y)) - 
          ((displayPtr[1].y - displayPtr[2].y) * (screenPtr[0].y - screenPtr[2].y)) ;
        /* E��((X0��X2) (YD1��YD2)��(YD0��YD2) (X1��X2))��K	*/
        matrixPtr->En = ((screenPtr[0].x - screenPtr[2].x) * (displayPtr[1].y - displayPtr[2].y)) - 
          ((displayPtr[0].y - displayPtr[2].y) * (screenPtr[1].x - screenPtr[2].x)) ;
        /* F��(Y0(X2YD1��X1YD2)+Y1(X0YD2��X2YD0)+Y2(X1YD0��X0YD1))��K */
        matrixPtr->Fn = (screenPtr[2].x * displayPtr[1].y - screenPtr[1].x * displayPtr[2].y) * screenPtr[0].y +
          (screenPtr[0].x * displayPtr[2].y - screenPtr[2].x * displayPtr[0].y) * screenPtr[1].y +
            (screenPtr[1].x * displayPtr[0].y - screenPtr[0].x * displayPtr[1].y) * screenPtr[2].y ;
      }
    return( retTHRESHOLD ) ;
  }

FunctionalState getDisplayPoint(Coordinate * displayPtr,
                                Coordinate * screenPtr,
                                Matrix * matrixPtr )
  {
    FunctionalState retTHRESHOLD =ENABLE ;
    
    if( matrixPtr->Divider != 0 )
      {
        /* XD = AX+BY+C */        
        displayPtr->x = ( (matrixPtr->An * screenPtr->x) + 
                         (matrixPtr->Bn * screenPtr->y) + 
                           matrixPtr->Cn 
                             ) / matrixPtr->Divider ;
        /* YD = DX+EY+F */        
        displayPtr->y = ( (matrixPtr->Dn * screenPtr->x) + 
                         (matrixPtr->En * screenPtr->y) + 
                           matrixPtr->Fn 
                             ) / matrixPtr->Divider ;
      }
    else
      {
        retTHRESHOLD = DISABLE;
      }
    return(retTHRESHOLD);
  } 

void TouchPanel_Calibrate(void)
  {
    //  unsigned char i;
    //  Coordinate * Ptr;
    
    //  for(i=0;i<3;i++)
    //  {
    //   SSD1289_Clear(Black);
    //   SSD1289_CleanText(44,10,"Touch crosshair to calibrate",Red);
    //   Delay(250);
    //   SSD1289_DrawCross(DisplaySample[i].x,DisplaySample[i].y, Red, RGB565CONVERT(184,158,131));
    //   do
    //   {
    //     Ptr = Read_XPT2046();
    //   }
    //   while( Ptr == (void*)0 );
    //   ScreenSample[i].x= Ptr->x; ScreenSample[i].y= Ptr->y;
    //  }
    //  setCalibrationMatrix( &DisplaySample[0],&ScreenSample[0],&matrix );
    //  SSD1289_Clear(Black);
  } 

static void set_CS(void)
  {
    GPIO_SetBits(XPT2046_PORT, XPT2046_CS_PIN);
  }

static void reset_CS(void)
  {
    GPIO_ResetBits(XPT2046_PORT, XPT2046_CS_PIN);
  }

static uint16_t read_IRQ(void)
  {
    return GPIO_ReadInputDataBit(XPT2046_PORT, XPT2046_IRQ_PIN);
  }

uint16_t XPT2046_Press(void)
  {
    return read_IRQ();
  }

static void WR_CMD (uint16_t cmd)  
  { 
    /* Wait for SPI2 Tx buffer empty */ 
    while (SPI_I2S_GetFlagStatus(SPI2, SPI_I2S_FLAG_TXE) == RESET); 
    /* Send SPI2 data */ 
    SPI_I2S_SendData(SPI2,cmd); 
    /* Wait for SPI2 data reception */ 
    while (SPI_I2S_GetFlagStatus(SPI2, SPI_I2S_FLAG_RXNE) == RESET); 
    /* Read SPI2 received data */ 
    SPI_I2S_ReceiveData(SPI2); 
  } 

static uint16_t RD_AD(void)  
  { 
    uint16_t buf, temp; 
    /* Wait for SPI2 Tx buffer empty */ 
    while (SPI_I2S_GetFlagStatus(SPI2, SPI_I2S_FLAG_TXE) == RESET); 
    /* Send SPI2 data */ 
    SPI_I2S_SendData(SPI2,0x0000); 
    /* Wait for SPI2 data reception */ 
    while (SPI_I2S_GetFlagStatus(SPI2, SPI_I2S_FLAG_RXNE) == RESET); 
    /* Read SPI2 received data */ 
    temp=SPI_I2S_ReceiveData(SPI2); 
    buf=temp<<8; 
    Delay(1); 
    while (SPI_I2S_GetFlagStatus(SPI2, SPI_I2S_FLAG_TXE) == RESET); 
    /* Send SPI2 data */ 
    SPI_I2S_SendData(SPI2,0x0000); 
    /* Wait for SPI3 data reception */ 
    while (SPI_I2S_GetFlagStatus(SPI2, SPI_I2S_FLAG_RXNE) == RESET); 
    /* Read SPI2 received data */ 
    temp=SPI_I2S_ReceiveData(SPI2); 
    buf |= temp; 
    buf >>= 3; 
    buf &= 0xfff; 
    return buf; 
  }


static void Delay(unsigned int DelayCnt)
{
	unsigned int i;
	for(i=0;i<DelayCnt;i++);
}
