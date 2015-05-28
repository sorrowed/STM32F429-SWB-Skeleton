/**
 ******************************************************************************
 * @file    stm32f429i_discovery_lcd.h
 * @author  MCD Application Team
 * @version V1.0.1
 * @date    28-October-2013
 * @brief   This file contains all the functions prototypes for the
 *          stm32f429i_discovery_lcd.c driver.
 ******************************************************************************
 * @attention
 *
 * <h2><center>&copy; COPYRIGHT 2013 STMicroelectronics</center></h2>
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

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef LCD_H_
#define LCD_H_

#include <Fonts.h>
#include "stm32f4xx.h"

#ifdef __cplusplus
extern "C"
{
#endif 

typedef struct
{
	int16_t X;
	int16_t Y;
} Point, *pPoint;

#define  LCD_PIXEL_WIDTH    ((uint16_t)240)
#define  LCD_PIXEL_HEIGHT   ((uint16_t)320)

#define LCD_FRAME_BUFFER       ((uint32_t)0xD0000000)
#define BUFFER_OFFSET          ((uint32_t)0x50000)

/** 
 * @brief  LCD Control pin
 */
#define LCD_NCS_PIN             GPIO_Pin_2                  
#define LCD_NCS_GPIO_PORT       GPIOC                      
#define LCD_NCS_GPIO_CLK        RCC_AHB1Periph_GPIOC 

/** 
 * @brief  LCD Command/data pin
 */
#define LCD_WRX_PIN             GPIO_Pin_13                  
#define LCD_WRX_GPIO_PORT       GPIOD
#define LCD_WRX_GPIO_CLK        RCC_AHB1Periph_GPIOD 

/** 
 * @brief  LCD SPI Interface pins
 */
#define LCD_SPI_SCK_PIN               GPIO_Pin_7                     /* PF.07 */
#define LCD_SPI_SCK_GPIO_PORT         GPIOF                          /* GPIOF */
#define LCD_SPI_SCK_GPIO_CLK          RCC_AHB1Periph_GPIOF  
#define LCD_SPI_SCK_SOURCE            GPIO_PinSource7
#define LCD_SPI_SCK_AF                GPIO_AF_SPI5
#define LCD_SPI_MISO_PIN              GPIO_Pin_8                     /* PF.08 */
#define LCD_SPI_MISO_GPIO_PORT        GPIOF                          /* GPIOF */
#define LCD_SPI_MISO_GPIO_CLK         RCC_AHB1Periph_GPIOF  
#define LCD_SPI_MISO_SOURCE           GPIO_PinSource8
#define LCD_SPI_MISO_AF               GPIO_AF_SPI5
#define LCD_SPI_MOSI_PIN              GPIO_Pin_9                     /* PF.09 */
#define LCD_SPI_MOSI_GPIO_PORT        GPIOF                          /* GPIOF */
#define LCD_SPI_MOSI_GPIO_CLK         RCC_AHB1Periph_GPIOF  
#define LCD_SPI_MOSI_SOURCE           GPIO_PinSource9
#define LCD_SPI_MOSI_AF               GPIO_AF_SPI5
#define LCD_SPI                       SPI5
#define LCD_SPI_CLK                   RCC_APB2Periph_SPI5 

/** 
 * @brief  LCD Registers
 */
#define LCD_SLEEP_OUT            0x11   /* Sleep out register */
#define LCD_GAMMA                0x26   /* Gamma register */
#define LCD_DISPLAY_OFF          0x28   /* Display off register */
#define LCD_DISPLAY_ON           0x29   /* Display on register */
#define LCD_COLUMN_ADDR          0x2A   /* Colomn address register */ 
#define LCD_PAGE_ADDR            0x2B   /* Page address register */ 
#define LCD_GRAM                 0x2C   /* GRAM register */   
#define LCD_MAC                  0x36   /* Memory Access Control register*/
#define LCD_PIXEL_FORMAT         0x3A   /* Pixel Format register */
#define LCD_WDB                  0x51   /* Write Brightness Display register */
#define LCD_WCD                  0x53   /* Write Control Display register*/
#define LCD_RGB_INTERFACE        0xB0   /* RGB Interface Signal Control */
#define LCD_FRC                  0xB1   /* Frame Rate Control register */
#define LCD_BPC                  0xB5   /* Blanking Porch Control register*/
#define LCD_DFC                  0xB6   /* Display Function Control register*/
#define LCD_POWER1               0xC0   /* Power Control 1 register */
#define LCD_POWER2               0xC1   /* Power Control 2 register */
#define LCD_VCOM1                0xC5   /* VCOM Control 1 register */
#define LCD_VCOM2                0xC7   /* VCOM Control 2 register */
#define LCD_POWERA               0xCB   /* Power control A register */
#define LCD_POWERB               0xCF   /* Power control B register */
#define LCD_PGAMMA               0xE0   /* Positive Gamma Correction register*/
#define LCD_NGAMMA               0xE1   /* Negative Gamma Correction register*/
#define LCD_DTCA                 0xE8   /* Driver timing control A */
#define LCD_DTCB                 0xEA   /* Driver timing control B */
#define LCD_POWER_SEQ            0xED   /* Power on sequence register */
#define LCD_3GAMMA_EN            0xF2   /* 3 Gamma enable register */
#define LCD_INTERFACE            0xF6   /* Interface control register */
#define LCD_PRC                  0xF7   /* Pump ratio control register */

/** 
 * @brief  LCD color
 */
#define LCD_COLOR_WHITE          0xFFFF
#define LCD_COLOR_BLACK          0x0000
#define LCD_COLOR_GREY           0xF7DE
#define LCD_COLOR_BLUE           0x001F
#define LCD_COLOR_BLUE2          0x051F
#define LCD_COLOR_RED            0xF800
#define LCD_COLOR_MAGENTA        0xF81F
#define LCD_COLOR_GREEN          0x07E0
#define LCD_COLOR_CYAN           0x7FFF
#define LCD_COLOR_YELLOW         0xFFE0

#define LCD_LINE(x) ((x) * (((sFONT *)LcdGetFont())->Height))

/** 
 * @brief LCD default font
 */
#define LCD_DEFAULT_FONT         Font16x24

/** 
 * @brief  LCD Direction
 */
#define LCD_DIR_HORIZONTAL       0x0000
#define LCD_DIR_VERTICAL         0x0001

/**
 * @}
 */

/** 
 * @brief  LCD Layer
 */
#define LCD_BACKGROUND_LAYER     0x0000
#define LCD_FOREGROUND_LAYER     0x0001

/**
 * @}
 */

/** @defgroup STM32F429I_DISCOVERY_LCD_Exported_Macros
 * @{
 */
#define ASSEMBLE_RGB(R, G, B)    ((((R)& 0xF8) << 8) | (((G) & 0xFC) << 3) | (((B) & 0xF8) >> 3))  

/**
 * @}
 */

/** @defgroup STM32F429I_DISCOVERY_LCD_Exported_Functions
 * @{
 */

void LcdInit( void );
void LcdSetLayer( uint32_t Layerx );
void LcdSetColors( uint16_t _TextColor, uint16_t _BackColor );
void LcdGetColors( uint16_t *_TextColor, uint16_t *_BackColor );
void LcdSetTextColor( uint16_t Color );
void LcdSetBackColor( uint16_t Color );
void LcdSetTransparency( uint8_t transparency );
void LcdClearLine( uint16_t Line );
void LcdClear( uint16_t Color );
uint32_t LcdSetCursor( uint16_t Xpos, uint16_t Ypos );
void LcdSetColorKeying( uint32_t RGBValue );
void LcdReSetColorKeying( void );

void LcdSetFont( sFONT *fonts );
sFONT * LcdGetFont( void );

void LcdDrawRawChar( uint16_t Xpos, uint16_t Ypos, const uint16_t *c );
void LcdDrawChar( uint16_t Line, uint16_t Column, uint8_t Ascii );
void LcdDrawString( uint16_t Line, char* ptr );

void LcdSetDisplayWindow( uint16_t Xpos, uint16_t Ypos, uint16_t Height, uint16_t Width );
void LcdWindowModeDisable( void );
void LcdDrawLine( uint16_t Xpos, uint16_t Ypos, uint16_t Length, uint8_t Direction );
void LcdDrawRect( uint16_t Xpos, uint16_t Ypos, uint16_t Width, uint16_t Height );
void LcdDrawCircle( uint16_t Xpos, uint16_t Ypos, uint16_t Radius );
void LcdDrawEllipse( int Xpos, int Ypos, int Radius, int Radius2 );
void LcdDrawFullEllipse( int Xpos, int Ypos, int Radius, int Radius2 );
void LcdDrawMonoPict( const uint32_t *Pict );
void LcdWriteBMP( uint32_t BmpAddress );
void LcdDrawUniLine( uint16_t x1, uint16_t y1, uint16_t x2, uint16_t y2 );
void LcdDrawFullRect( uint16_t Xpos, uint16_t Ypos, uint16_t Width, uint16_t Height );
void LcdDrawFullCircle( uint16_t Xpos, uint16_t Ypos, uint16_t Radius );
void LcdPolyLine( pPoint Points, uint16_t PointCount );
void LcdPolyLineRelative( pPoint Points, uint16_t PointCount );
void LcdClosedPolyLine( pPoint Points, uint16_t PointCount );
void LcdClosedPolyLineRelative( pPoint Points, uint16_t PointCount );
void LcdFillPolyLine( pPoint Points, uint16_t PointCount );
void LcdTriangle( pPoint Points, uint16_t PointCount );
void LcdFillTriangle( uint16_t x1, uint16_t x2, uint16_t x3, uint16_t y1, uint16_t y2, uint16_t y3 );
void LcdWriteCommand( uint8_t LCD_Reg );
void LcdWriteData( uint8_t value );
void LcdPowerOn( void );
void LcdDisplayOn( void );
void LcdDisplayOff( void );

#ifdef __cplusplus
}
#endif

#endif /* LCD_H_ */
