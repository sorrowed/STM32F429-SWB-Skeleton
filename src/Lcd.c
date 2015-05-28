#include "Lcd.h"
#include "SdRam.h"

#define POLY_Y(Z)          ((int32_t)((Points + Z)->X))
#define POLY_X(Z)          ((int32_t)((Points + Z)->Y))   

#define ABS(X)  ((X) > 0 ? (X) : -(X))    

static sFONT *LCD_Currentfonts;
static uint16_t CurrentTextColor = 0x0000;
static uint16_t CurrentBackColor = 0xFFFF;
static uint32_t CurrentFrameBuffer = LCD_FRAME_BUFFER;
static uint32_t CurrentLayer = LCD_BACKGROUND_LAYER;
static void delay( __IO uint32_t nCount );

static void LcdPutPixel( int16_t x, int16_t y );
static void LcdPolyLineRelativeClosed( pPoint Points, uint16_t PointCount, uint16_t Closed );
static void LcdGpioAfConfig( void );

static void LcdDeInit( void );
static void LcdLayerInit( void );
static void LcdChipSelect( FunctionalState NewState );
static void LcdCtrlLinesConfig( void );
static void LcdCtrlLinesWrite( GPIO_TypeDef* GPIOx, uint16_t CtrlPins, BitAction BitVal );
static void LcdSpiConfig( void );

/**
 * @}
 */

/** @defgroup STM32F429I_DISCOVERY_LCD_Private_Functions
 * @{
 */

/**
 * @brief  DeInitializes the LCD.
 * @param  None
 * @retval None
 */
void LcdDeInit( void )
{
	GPIO_InitTypeDef GpioCfg;

	LcdDisplayOff();
	SPI_Cmd( LCD_SPI, DISABLE );
	SPI_I2S_DeInit( LCD_SPI );

	RCC_APB2PeriphClockCmd( LCD_SPI_CLK, DISABLE );

	/* Configure NCS in Output Push-Pull mode */
	GpioCfg.GPIO_Pin = LCD_NCS_PIN;
	GpioCfg.GPIO_Mode = GPIO_Mode_IN;
	GpioCfg.GPIO_PuPd = GPIO_PuPd_NOPULL;
	GPIO_Init( LCD_NCS_GPIO_PORT, &GpioCfg );

	/* Configure SPI pins: SCK, MISO and MOSI */
	GpioCfg.GPIO_Pin = LCD_SPI_SCK_PIN;
	GPIO_Init( LCD_SPI_SCK_GPIO_PORT, &GpioCfg );

	GpioCfg.GPIO_Pin = LCD_SPI_MISO_PIN;
	GPIO_Init( LCD_SPI_MISO_GPIO_PORT, &GpioCfg );

	GpioCfg.GPIO_Pin = LCD_SPI_MOSI_PIN;
	GPIO_Init( LCD_SPI_MOSI_GPIO_PORT, &GpioCfg );

	/* GPIOA configuration */
	GPIO_PinAFConfig( GPIOA, GPIO_PinSource3, GPIO_AF_MCO );
	GPIO_PinAFConfig( GPIOA, GPIO_PinSource4, GPIO_AF_MCO );
	GPIO_PinAFConfig( GPIOA, GPIO_PinSource6, GPIO_AF_MCO );
	GPIO_PinAFConfig( GPIOA, GPIO_PinSource11, GPIO_AF_MCO );
	GPIO_PinAFConfig( GPIOA, GPIO_PinSource12, GPIO_AF_MCO );

	GpioCfg.GPIO_Pin = GPIO_Pin_3 | GPIO_Pin_4 | GPIO_Pin_6 | GPIO_Pin_11 | GPIO_Pin_12;
	GpioCfg.GPIO_Mode = GPIO_Mode_AF;
	GpioCfg.GPIO_Speed = GPIO_Speed_50MHz;
	GpioCfg.GPIO_OType = GPIO_OType_PP;
	GpioCfg.GPIO_PuPd = GPIO_PuPd_NOPULL;
	GPIO_Init( GPIOA, &GpioCfg );

	/* GPIOB configuration */
	GPIO_PinAFConfig( GPIOB, GPIO_PinSource0, GPIO_AF_MCO );
	GPIO_PinAFConfig( GPIOB, GPIO_PinSource1, GPIO_AF_MCO );
	GPIO_PinAFConfig( GPIOB, GPIO_PinSource8, GPIO_AF_MCO );
	GPIO_PinAFConfig( GPIOB, GPIO_PinSource9, GPIO_AF_MCO );
	GPIO_PinAFConfig( GPIOB, GPIO_PinSource10, GPIO_AF_MCO );
	GPIO_PinAFConfig( GPIOB, GPIO_PinSource11, GPIO_AF_MCO );

	GpioCfg.GPIO_Pin = GPIO_Pin_0 | GPIO_Pin_1 | GPIO_Pin_8 | GPIO_Pin_9 | GPIO_Pin_10 | GPIO_Pin_11;
	GpioCfg.GPIO_Mode = GPIO_Mode_AF;
	GpioCfg.GPIO_Speed = GPIO_Speed_50MHz;
	GpioCfg.GPIO_OType = GPIO_OType_PP;
	GpioCfg.GPIO_PuPd = GPIO_PuPd_NOPULL;
	GPIO_Init( GPIOB, &GpioCfg );

	/* GPIOC configuration */
	GPIO_PinAFConfig( GPIOC, GPIO_PinSource6, GPIO_AF_MCO );
	GPIO_PinAFConfig( GPIOC, GPIO_PinSource7, GPIO_AF_MCO );
	GPIO_PinAFConfig( GPIOC, GPIO_PinSource10, GPIO_AF_MCO );

	GpioCfg.GPIO_Pin = GPIO_Pin_6 | GPIO_Pin_7 | GPIO_Pin_10;
	GpioCfg.GPIO_Mode = GPIO_Mode_AF;
	GpioCfg.GPIO_Speed = GPIO_Speed_50MHz;
	GpioCfg.GPIO_OType = GPIO_OType_PP;
	GpioCfg.GPIO_PuPd = GPIO_PuPd_NOPULL;
	GPIO_Init( GPIOC, &GpioCfg );

	/* GPIOD configuration */
	GPIO_PinAFConfig( GPIOD, GPIO_PinSource3, GPIO_AF_MCO );
	GPIO_PinAFConfig( GPIOD, GPIO_PinSource6, GPIO_AF_MCO );

	GpioCfg.GPIO_Pin = GPIO_Pin_3 | GPIO_Pin_6;
	GpioCfg.GPIO_Mode = GPIO_Mode_AF;
	GpioCfg.GPIO_Speed = GPIO_Speed_50MHz;
	GpioCfg.GPIO_OType = GPIO_OType_PP;
	GpioCfg.GPIO_PuPd = GPIO_PuPd_NOPULL;
	GPIO_Init( GPIOD, &GpioCfg );

	/* GPIOF configuration */
	GPIO_PinAFConfig( GPIOF, GPIO_PinSource10, GPIO_AF_MCO );

	GpioCfg.GPIO_Pin = GPIO_Pin_10;
	GpioCfg.GPIO_Mode = GPIO_Mode_AF;
	GpioCfg.GPIO_Speed = GPIO_Speed_50MHz;
	GpioCfg.GPIO_OType = GPIO_OType_PP;
	GpioCfg.GPIO_PuPd = GPIO_PuPd_NOPULL;
	GPIO_Init( GPIOF, &GpioCfg );

	/* GPIOG configuration */
	GPIO_PinAFConfig( GPIOG, GPIO_PinSource6, GPIO_AF_MCO );
	GPIO_PinAFConfig( GPIOG, GPIO_PinSource7, GPIO_AF_MCO );
	GPIO_PinAFConfig( GPIOG, GPIO_PinSource10, GPIO_AF_MCO );
	GPIO_PinAFConfig( GPIOG, GPIO_PinSource11, GPIO_AF_MCO );
	GPIO_PinAFConfig( GPIOG, GPIO_PinSource12, GPIO_AF_MCO );

	GpioCfg.GPIO_Pin = GPIO_Pin_6 | GPIO_Pin_7 | GPIO_Pin_10 | GPIO_Pin_11 | GPIO_Pin_12;
	GpioCfg.GPIO_Mode = GPIO_Mode_AF;
	GpioCfg.GPIO_Speed = GPIO_Speed_50MHz;
	GpioCfg.GPIO_OType = GPIO_OType_PP;
	GpioCfg.GPIO_PuPd = GPIO_PuPd_NOPULL;
	GPIO_Init( GPIOG, &GpioCfg );
}

/**
 * @brief  Initializes the LCD.
 * @param  None
 * @retval None
 */
void LcdInit( void )
{
	LTDC_InitTypeDef LtdcCfg;

	/* Configure the LCD Control pins ------------------------------------------*/
	LcdCtrlLinesConfig();
	LcdChipSelect( DISABLE );
	LcdChipSelect( ENABLE );

	/* Configure the LCD_SPI interface -----------------------------------------*/
	LcdSpiConfig();

	/* Power on the LCD --------------------------------------------------------*/
	LcdPowerOn();

	/* Enable the LTDC Clock */
	RCC_APB2PeriphClockCmd( RCC_APB2Periph_LTDC, ENABLE );

	/* Enable the DMA2D Clock */
	RCC_AHB1PeriphClockCmd( RCC_AHB1Periph_DMA2D, ENABLE );

	/* Configure the LCD Control pins */
	LcdGpioAfConfig();

	/* LTDC Configuration *********************************************************/
	/* Polarity configuration */
	/* Initialize the horizontal synchronization polarity as active low */
	LtdcCfg.LTDC_HSPolarity = LTDC_HSPolarity_AL;
	/* Initialize the vertical synchronization polarity as active low */
	LtdcCfg.LTDC_VSPolarity = LTDC_VSPolarity_AL;
	/* Initialize the data enable polarity as active low */
	LtdcCfg.LTDC_DEPolarity = LTDC_DEPolarity_AL;
	/* Initialize the pixel clock polarity as input pixel clock */
	LtdcCfg.LTDC_PCPolarity = LTDC_PCPolarity_IPC;

	/* Configure R,G,B component values for LCD background color */
	LtdcCfg.LTDC_BackgroundRedValue = 0;
	LtdcCfg.LTDC_BackgroundGreenValue = 0;
	LtdcCfg.LTDC_BackgroundBlueValue = 0;

	/* Configure PLLSAI prescalers for LCD */
	/* Enable Pixel Clock */
	/* PLLSAI_VCO Input = HSE_VALUE/PLL_M = 1 Mhz */
	/* PLLSAI_VCO Output = PLLSAI_VCO Input * PLLSAI_N = 192 Mhz */
	/* PLLLCDCLK = PLLSAI_VCO Output/PLLSAI_R = 192/4 = 48 Mhz */
	/* LTDC clock frequency = PLLLCDCLK / RCC_PLLSAIDivR = 48/8 = 6 Mhz */
	RCC_PLLSAIConfig( 192, 7, 4 );
	RCC_LTDCCLKDivConfig( RCC_PLLSAIDivR_Div8 );

	/* Enable PLLSAI Clock */
	RCC_PLLSAICmd( ENABLE );
	/* Wait for PLLSAI activation */
	while( RCC_GetFlagStatus( RCC_FLAG_PLLSAIRDY ) == RESET )
	{
	}

	/* Timing configuration */
	/* Configure horizontal synchronization width */
	LtdcCfg.LTDC_HorizontalSync = 9;
	/* Configure vertical synchronization height */
	LtdcCfg.LTDC_VerticalSync = 1;
	/* Configure accumulated horizontal back porch */
	LtdcCfg.LTDC_AccumulatedHBP = 29;
	/* Configure accumulated vertical back porch */
	LtdcCfg.LTDC_AccumulatedVBP = 3;
	/* Configure accumulated active width */
	LtdcCfg.LTDC_AccumulatedActiveW = 269;
	/* Configure accumulated active height */
	LtdcCfg.LTDC_AccumulatedActiveH = 323;
	/* Configure total width */
	LtdcCfg.LTDC_TotalWidth = 279;
	/* Configure total height */
	LtdcCfg.LTDC_TotalHeigh = 327;

	LTDC_Init( &LtdcCfg );

	LcdLayerInit();

	/* LTDC reload configuration */
	LTDC_ReloadConfig( LTDC_IMReload );

	/* Enable the LTDC */
	LTDC_Cmd( ENABLE );

}

/**
 * @brief  Initializes the LCD Layers.
 * @param  None
 * @retval None
 */
void LcdLayerInit( void )
{
	LTDC_Layer_InitTypeDef LtdcLayerCfg;

	/* Windowing configuration */
	/* In this case all the active display area is used to display a picture then :
	 Horizontal start = horizontal synchronization + Horizontal back porch = 30
	 Horizontal stop = Horizontal start + window width -1 = 30 + 240 -1
	 Vertical start   = vertical synchronization + vertical back porch     = 4
	 Vertical stop   = Vertical start + window height -1  = 4 + 320 -1      */
	LtdcLayerCfg.LTDC_HorizontalStart = 30;
	LtdcLayerCfg.LTDC_HorizontalStop = ( LCD_PIXEL_WIDTH + 30 - 1 );
	LtdcLayerCfg.LTDC_VerticalStart = 4;
	LtdcLayerCfg.LTDC_VerticalStop = ( LCD_PIXEL_HEIGHT + 4 - 1 );

	/* Pixel Format configuration*/
	LtdcLayerCfg.LTDC_PixelFormat = LTDC_Pixelformat_RGB565;
	/* Alpha constant (255 totally opaque) */
	LtdcLayerCfg.LTDC_ConstantAlpha = 255;
	/* Default Color configuration (configure A,R,G,B component values) */
	LtdcLayerCfg.LTDC_DefaultColorBlue = 0;
	LtdcLayerCfg.LTDC_DefaultColorGreen = 0;
	LtdcLayerCfg.LTDC_DefaultColorRed = 0;
	LtdcLayerCfg.LTDC_DefaultColorAlpha = 0;
	/* Configure blending factors */
	LtdcLayerCfg.LTDC_BlendingFactor_1 = LTDC_BlendingFactor1_CA;
	LtdcLayerCfg.LTDC_BlendingFactor_2 = LTDC_BlendingFactor2_CA;

	/* the length of one line of pixels in bytes + 3 then :
	 Line Lenth = Active high width x number of bytes per pixel + 3
	 Active high width         = LCD_PIXEL_WIDTH
	 number of bytes per pixel = 2    (pixel_format : RGB565)
	 */
	LtdcLayerCfg.LTDC_CFBLineLength = ( ( LCD_PIXEL_WIDTH * 2 ) + 3 );
	/* the pitch is the increment from the start of one line of pixels to the
	 start of the next line in bytes, then :
	 Pitch = Active high width x number of bytes per pixel */
	LtdcLayerCfg.LTDC_CFBPitch = ( LCD_PIXEL_WIDTH * 2 );

	/* Configure the number of lines */
	LtdcLayerCfg.LTDC_CFBLineNumber = LCD_PIXEL_HEIGHT;

	/* Start Address configuration : the LCD Frame buffer is defined on SDRAM */
	LtdcLayerCfg.LTDC_CFBStartAdress = LCD_FRAME_BUFFER;

	/* Initialize LTDC layer 1 */
	LTDC_LayerInit( LTDC_Layer1, &LtdcLayerCfg );

	/* Configure Layer2 */
	/* Start Address configuration : the LCD Frame buffer is defined on SDRAM w/ Offset */
	LtdcLayerCfg.LTDC_CFBStartAdress = LCD_FRAME_BUFFER + BUFFER_OFFSET;

	/* Configure blending factors */
	LtdcLayerCfg.LTDC_BlendingFactor_1 = LTDC_BlendingFactor1_PAxCA;
	LtdcLayerCfg.LTDC_BlendingFactor_2 = LTDC_BlendingFactor2_PAxCA;

	/* Initialize LTDC layer 2 */
	LTDC_LayerInit( LTDC_Layer2, &LtdcLayerCfg );

	/* LTDC configuration reload */
	LTDC_ReloadConfig( LTDC_IMReload );

	/* Enable foreground & background Layers */
	LTDC_LayerCmd( LTDC_Layer1, ENABLE );
	LTDC_LayerCmd( LTDC_Layer2, ENABLE );

	/* LTDC configuration reload */
	LTDC_ReloadConfig( LTDC_IMReload );

	/* Set default font */
	LcdSetFont( &LCD_DEFAULT_FONT );

	/* dithering activation */
	LTDC_DitherCmd( ENABLE );
}

/**
 * @brief  Controls LCD Chip Select (CS) pin
 * @param  NewState CS pin state
 * @retval None
 */
void LcdChipSelect( FunctionalState NewState )
{
	if( NewState == DISABLE )
	{
		GPIO_ResetBits( LCD_NCS_GPIO_PORT, LCD_NCS_PIN ); /* CS pin low: LCD disabled */
	}
	else
	{
		GPIO_SetBits( LCD_NCS_GPIO_PORT, LCD_NCS_PIN ); /* CS pin high: LCD enabled */
	}
}

/**
 * @brief  Sets the LCD Layer.
 * @param  Layerx: specifies the Layer foreground or background.
 * @retval None
 */
void LcdSetLayer( uint32_t Layerx )
{
	if( Layerx == LCD_BACKGROUND_LAYER )
	{
		CurrentFrameBuffer = LCD_FRAME_BUFFER;
		CurrentLayer = LCD_BACKGROUND_LAYER;
	}
	else
	{
		CurrentFrameBuffer = LCD_FRAME_BUFFER + BUFFER_OFFSET;
		CurrentLayer = LCD_FOREGROUND_LAYER;
	}
}

/**
 * @brief  Sets the LCD Text and Background colors.
 * @param  TextColor: specifies the Text Color.
 * @param  BackColor: specifies the Background Color.
 * @retval None
 */
void LcdSetColors( uint16_t TextColor, uint16_t BackColor )
{
	CurrentTextColor = TextColor;
	CurrentBackColor = BackColor;
}

/**
 * @brief  Gets the LCD Text and Background colors.
 * @param  TextColor: pointer to the variable that will contain the Text
 Color.
 * @param  BackColor: pointer to the variable that will contain the Background
 Color.
 * @retval None
 */
void LcdGetColors( uint16_t *TextColor, uint16_t *BackColor )
{
	*TextColor = CurrentTextColor;
	*BackColor = CurrentBackColor;
}

/**
 * @brief  Sets the Text color.
 * @param  Color: specifies the Text color code RGB(5-6-5).
 * @retval None
 */
void LcdSetTextColor( uint16_t Color )
{
	CurrentTextColor = Color;
}

/**
 * @brief  Sets the Background color.
 * @param  Color: specifies the Background color code RGB(5-6-5).
 * @retval None
 */
void LcdSetBackColor( uint16_t Color )
{
	CurrentBackColor = Color;
}

/**
 * @brief  Sets the Text Font.
 * @param  fonts: specifies the font to be used.
 * @retval None
 */
void LcdSetFont( sFONT *fonts )
{
	LCD_Currentfonts = fonts;
}

/**
 * @brief  Configure the transparency.
 * @param  transparency: specifies the transparency,
 *         This parameter must range from 0x00 to 0xFF.
 * @retval None
 */
void LcdSetTransparency( uint8_t transparency )
{
	if( CurrentLayer == LCD_BACKGROUND_LAYER )
	{
		LTDC_LayerAlpha( LTDC_Layer1, transparency );
	}
	else
	{
		LTDC_LayerAlpha( LTDC_Layer2, transparency );
	}
	LTDC_ReloadConfig( LTDC_IMReload );
}

/**
 * @brief  Gets the Text Font.
 * @param  None.
 * @retval the used font.
 */
sFONT *LcdGetFont( void )
{
	return LCD_Currentfonts;
}

/**
 * @brief  Clears the selected line.
 * @param  Line: the Line to be cleared.
 *   This parameter can be one of the following values:
 *     @arg LCD_LINE_x: where x can be: 0..13 if LCD_Currentfonts is Font16x24
 *                                      0..26 if LCD_Currentfonts is Font12x12 or Font8x12
 *                                      0..39 if LCD_Currentfonts is Font8x8
 * @retval None
 */
void LcdClearLine( uint16_t Line )
{
	uint16_t refcolumn = 0;
	/* Send the string character by character on lCD */
	while( ( refcolumn < LCD_PIXEL_WIDTH )
			&& ( ( ( refcolumn + LCD_Currentfonts->Width ) & 0xFFFF ) >= LCD_Currentfonts->Width ) )
	{
		/* Display one character on LCD */
		LcdDrawChar( Line, refcolumn, ' ' );
		/* Decrement the column position by 16 */
		refcolumn += LCD_Currentfonts->Width;
	}
}

/**
 * @brief  Clears the hole LCD.
 * @param  Color: the color of the background.
 * @retval None
 */
void LcdClear( uint16_t Color )
{
	uint32_t index = 0;

	/* erase memory */
	for( index = 0x00; index < BUFFER_OFFSET; index++ )
	{
		*(__IO uint16_t*) ( CurrentFrameBuffer + ( 2 * index ) ) = Color;
	}
}

/**
 * @brief  Sets the cursor position.
 * @param  Xpos: specifies the X position.
 * @param  Ypos: specifies the Y position.
 * @retval Display Address
 */
uint32_t LcdSetCursor( uint16_t Xpos, uint16_t Ypos )
{
	return CurrentFrameBuffer + 2 * ( Xpos + ( LCD_PIXEL_WIDTH * Ypos ) );
}

/**
 * @brief  Config and Sets the color Keying.
 * @param  RGBValue: Specifies the Color reference.
 * @retval None
 */
void LcdSetColorKeying( uint32_t RGBValue )
{
	LTDC_ColorKeying_InitTypeDef LTDC_colorkeying_InitStruct;

	/* configure the color Keying */
	LTDC_colorkeying_InitStruct.LTDC_ColorKeyBlue = 0x0000FF & RGBValue;
	LTDC_colorkeying_InitStruct.LTDC_ColorKeyGreen = ( 0x00FF00 & RGBValue ) >> 8;
	LTDC_colorkeying_InitStruct.LTDC_ColorKeyRed = ( 0xFF0000 & RGBValue ) >> 16;

	if( CurrentLayer == LCD_BACKGROUND_LAYER )
	{
		/* Enable the color Keying for Layer1 */
		LTDC_ColorKeyingConfig( LTDC_Layer1, &LTDC_colorkeying_InitStruct, ENABLE );
		LTDC_ReloadConfig( LTDC_IMReload );
	}
	else
	{
		/* Enable the color Keying for Layer2 */
		LTDC_ColorKeyingConfig( LTDC_Layer2, &LTDC_colorkeying_InitStruct, ENABLE );
		LTDC_ReloadConfig( LTDC_IMReload );
	}
}

/**
 * @brief  Disable the color Keying.
 * @param  RGBValue: Specifies the Color reference.
 * @retval None
 */
void LcdReSetColorKeying( void )
{
	LTDC_ColorKeying_InitTypeDef LTDC_colorkeying_InitStruct;

	if( CurrentLayer == LCD_BACKGROUND_LAYER )
	{
		/* Disable the color Keying for Layer1 */
		LTDC_ColorKeyingConfig( LTDC_Layer1, &LTDC_colorkeying_InitStruct, DISABLE );
		LTDC_ReloadConfig( LTDC_IMReload );
	}
	else
	{
		/* Disable the color Keying for Layer2 */
		LTDC_ColorKeyingConfig( LTDC_Layer2, &LTDC_colorkeying_InitStruct, DISABLE );
		LTDC_ReloadConfig( LTDC_IMReload );
	}
}

/**
 * @brief  Draws a character on LCD.
 * @param  Xpos: the Line where to display the character shape.
 * @param  Ypos: start column address.
 * @param  c: pointer to the character data.
 * @retval None
 */
void LcdDrawRawChar( uint16_t Xpos, uint16_t Ypos, const uint16_t *c )
{
	uint32_t index = 0, counter = 0, xpos = 0;
	uint32_t Xaddress = 0;

	xpos = Xpos * LCD_PIXEL_WIDTH * 2;
	Xaddress += Ypos;

	for( index = 0; index < LCD_Currentfonts->Height; index++ )
	{

		for( counter = 0; counter < LCD_Currentfonts->Width; counter++ )
		{

			if( ( ( ( c[index] & ( ( 0x80 << ( ( LCD_Currentfonts->Width / 12 ) * 8 ) ) >> counter ) ) == 0x00 )
					&& ( LCD_Currentfonts->Width <= 12 ) )
					|| ( ( ( c[index] & ( 0x1 << counter ) ) == 0x00 ) && ( LCD_Currentfonts->Width > 12 ) ) )
			{
				/* Write data value to all SDRAM memory */
				*(__IO uint16_t*) ( CurrentFrameBuffer + ( 2 * Xaddress ) + xpos ) = CurrentBackColor;
			}
			else
			{
				/* Write data value to all SDRAM memory */
				*(__IO uint16_t*) ( CurrentFrameBuffer + ( 2 * Xaddress ) + xpos ) = CurrentTextColor;
			}
			Xaddress++;
		}
		Xaddress += ( LCD_PIXEL_WIDTH - LCD_Currentfonts->Width );
	}
}

/**
 * @brief  Displays one character (16dots width, 24dots height).
 * @param  Line: the Line where to display the character shape .
 *   This parameter can be one of the following values:
 *     @arg Linex: where x can be 0..29
 * @param  Column: start column address.
 * @param  Ascii: character ascii code, must be between 0x20 and 0x7E.
 * @retval None
 */
void LcdDrawChar( uint16_t Line, uint16_t Column, uint8_t Ascii )
{
	Ascii -= 32;

	LcdDrawRawChar( Line, Column, &LCD_Currentfonts->table[Ascii * LCD_Currentfonts->Height] );
}

/**
 * @brief  Displays a maximum of 20 char on the LCD.
 * @param  Line: the Line where to display the character shape .
 *   This parameter can be one of the following values:
 *     @arg Linex: where x can be 0..9
 * @param  *ptr: pointer to string to display on LCD.
 * @retval None
 */
void LcdDrawString( uint16_t Line, char *ptr )
{
	uint16_t refcolumn = 0;
	/* Send the string character by character on lCD */
	while( ( refcolumn < LCD_PIXEL_WIDTH )
			&& ( ( *ptr != 0 ) & ( ( ( refcolumn + LCD_Currentfonts->Width ) & 0xFFFF ) >= LCD_Currentfonts->Width ) ) )
	{
		/* Display one character on LCD */
		LcdDrawChar( Line, refcolumn, *ptr );
		/* Decrement the column position by width */
		refcolumn += LCD_Currentfonts->Width;
		/* Point on the next character */
		ptr++;
	}
}

/**
 * @brief  Sets a display window
 * @param  Xpos: specifies the X bottom left position from 0 to 240.
 * @param  Ypos: specifies the Y bottom left position from 0 to 320.
 * @param  Height: display window height, can be a value from 0 to 320.
 * @param  Width: display window width, can be a value from 0 to 240.
 * @retval None
 */
void LcdSetDisplayWindow( uint16_t Xpos, uint16_t Ypos, uint16_t Height, uint16_t Width )
{

	if( CurrentLayer == LCD_BACKGROUND_LAYER )
	{
		/* reconfigure the layer1 position */
		LTDC_LayerPosition( LTDC_Layer1, Xpos, Ypos );
		LTDC_ReloadConfig( LTDC_IMReload );

		/* reconfigure the layer1 size */
		LTDC_LayerSize( LTDC_Layer1, Width, Height );
		LTDC_ReloadConfig( LTDC_IMReload );
	}
	else
	{
		/* reconfigure the layer2 position */
		LTDC_LayerPosition( LTDC_Layer2, Xpos, Ypos );
		LTDC_ReloadConfig( LTDC_IMReload );

		/* reconfigure the layer2 size */
		LTDC_LayerSize( LTDC_Layer2, Width, Height );
		LTDC_ReloadConfig( LTDC_IMReload );
	}
}

/**
 * @brief  Disables LCD Window mode.
 * @param  None
 * @retval None
 */
void LcdWindowModeDisable( void )
{
	LcdSetDisplayWindow( 0, 0, LCD_PIXEL_HEIGHT, LCD_PIXEL_WIDTH );
}

/**
 * @brief  Displays a line.
 * @param Xpos: specifies the X position, can be a value from 0 to 240.
 * @param Ypos: specifies the Y position, can be a value from 0 to 320.
 * @param Length: line length.
 * @param Direction: line direction.
 *   This parameter can be one of the following values: LCD_DIR_HORIZONTAL or LCD_DIR_VERTICAL.
 * @retval None
 */
void LcdDrawLine( uint16_t Xpos, uint16_t Ypos, uint16_t Length, uint8_t Direction )
{
	DMA2D_InitTypeDef DMA2D_InitStruct;

	uint32_t Xaddress = 0;
	uint16_t Red_Value = 0, Green_Value = 0, Blue_Value = 0;

	Xaddress = CurrentFrameBuffer + 2 * ( LCD_PIXEL_WIDTH * Ypos + Xpos );

	Red_Value = ( 0xF800 & CurrentTextColor ) >> 11;
	Blue_Value = 0x001F & CurrentTextColor;
	Green_Value = ( 0x07E0 & CurrentTextColor ) >> 5;

	/* Configure DMA2D */
	DMA2D_DeInit();
	DMA2D_InitStruct.DMA2D_Mode = DMA2D_R2M;
	DMA2D_InitStruct.DMA2D_CMode = DMA2D_RGB565;
	DMA2D_InitStruct.DMA2D_OutputGreen = Green_Value;
	DMA2D_InitStruct.DMA2D_OutputBlue = Blue_Value;
	DMA2D_InitStruct.DMA2D_OutputRed = Red_Value;
	DMA2D_InitStruct.DMA2D_OutputAlpha = 0x0F;
	DMA2D_InitStruct.DMA2D_OutputMemoryAdd = Xaddress;

	if( Direction == LCD_DIR_HORIZONTAL )
	{
		DMA2D_InitStruct.DMA2D_OutputOffset = 0;
		DMA2D_InitStruct.DMA2D_NumberOfLine = 1;
		DMA2D_InitStruct.DMA2D_PixelPerLine = Length;
	}
	else
	{
		DMA2D_InitStruct.DMA2D_OutputOffset = LCD_PIXEL_WIDTH - 1;
		DMA2D_InitStruct.DMA2D_NumberOfLine = Length;
		DMA2D_InitStruct.DMA2D_PixelPerLine = 1;
	}

	DMA2D_Init( &DMA2D_InitStruct );
	/* Start Transfer */
	DMA2D_StartTransfer();
	/* Wait for CTC Flag activation */
	while( DMA2D_GetFlagStatus( DMA2D_FLAG_TC ) == RESET )
	{
	}

}

/**
 * @brief  Displays a rectangle.
 * @param  Xpos: specifies the X position, can be a value from 0 to 240.
 * @param  Ypos: specifies the Y position, can be a value from 0 to 320.
 * @param  Width: display rectangle width, can be a value from 0 to 240.
 * @param  Height: display rectangle height, can be a value from 0 to 320.
 * @retval None
 */
void LcdDrawRect( uint16_t Xpos, uint16_t Ypos, uint16_t Width, uint16_t Height )
{
	/* draw horizontal lines */
	LcdDrawLine( Xpos, Ypos, Width, LCD_DIR_HORIZONTAL );
	LcdDrawLine( Xpos, ( Ypos + Height ), Width, LCD_DIR_HORIZONTAL );

	/* draw vertical lines */
	LcdDrawLine( Xpos, Ypos, Height, LCD_DIR_VERTICAL );
	LcdDrawLine( ( Xpos + Width ), Ypos, Height, LCD_DIR_VERTICAL );
}

/**
 * @brief  Draw a circle.
 * @param  Xpos: specifies the X position, can be a value from 0 to 240.
 * @param  Ypos: specifies the Y position, can be a value from 0 to 320.
 * @param  Radius: radius of the circle.
 * @retval None
 */
void LcdDrawCircle( uint16_t Xpos, uint16_t Ypos, uint16_t Radius )
{
	int x = -Radius, y = 0, err = 2 - 2 * Radius, e2;
	do
	{
		*(__IO uint16_t*) ( CurrentFrameBuffer + ( 2 * ( ( Xpos - x ) + LCD_PIXEL_WIDTH * ( Ypos + y ) ) ) ) =
				CurrentTextColor;
		*(__IO uint16_t*) ( CurrentFrameBuffer + ( 2 * ( ( Xpos + x ) + LCD_PIXEL_WIDTH * ( Ypos + y ) ) ) ) =
				CurrentTextColor;
		*(__IO uint16_t*) ( CurrentFrameBuffer + ( 2 * ( ( Xpos + x ) + LCD_PIXEL_WIDTH * ( Ypos - y ) ) ) ) =
				CurrentTextColor;
		*(__IO uint16_t*) ( CurrentFrameBuffer + ( 2 * ( ( Xpos - x ) + LCD_PIXEL_WIDTH * ( Ypos - y ) ) ) ) =
				CurrentTextColor;

		e2 = err;
		if( e2 <= y )
		{
			err += ++y * 2 + 1;
			if( -x == y && e2 <= x )
				e2 = 0;
		}
		if( e2 > x )
			err += ++x * 2 + 1;
	} while( x <= 0 );
}

/**
 * @brief  Draw a full ellipse.
 * @param  Xpos: specifies the X position, can be a value from 0 to 240.
 * @param  Ypos: specifies the Y position, can be a value from 0 to 320.
 * @param  Radius: minor radius of ellipse.
 * @param  Radius2: major radius of ellipse.
 * @retval None
 */
void LcdDrawFullEllipse( int Xpos, int Ypos, int Radius, int Radius2 )
{
	int x = -Radius, y = 0, err = 2 - 2 * Radius, e2;
	float K = 0, rad1 = 0, rad2 = 0;

	rad1 = Radius;
	rad2 = Radius2;

	if( Radius > Radius2 )
	{
		do
		{
			K = (float) ( rad1 / rad2 );
			LcdDrawLine( ( Xpos + x ), ( Ypos - (uint16_t) ( y / K ) ), ( 2 * (uint16_t) ( y / K ) + 1 ),
			LCD_DIR_VERTICAL );
			LcdDrawLine( ( Xpos - x ), ( Ypos - (uint16_t) ( y / K ) ), ( 2 * (uint16_t) ( y / K ) + 1 ),
			LCD_DIR_VERTICAL );

			e2 = err;
			if( e2 <= y )
			{
				err += ++y * 2 + 1;
				if( -x == y && e2 <= x )
					e2 = 0;
			}
			if( e2 > x )
				err += ++x * 2 + 1;

		} while( x <= 0 );
	}
	else
	{
		y = -Radius2;
		x = 0;
		do
		{
			K = (float) ( rad2 / rad1 );
			LcdDrawLine( ( Xpos - (uint16_t) ( x / K ) ), ( Ypos + y ), ( 2 * (uint16_t) ( x / K ) + 1 ),
			LCD_DIR_HORIZONTAL );
			LcdDrawLine( ( Xpos - (uint16_t) ( x / K ) ), ( Ypos - y ), ( 2 * (uint16_t) ( x / K ) + 1 ),
			LCD_DIR_HORIZONTAL );

			e2 = err;
			if( e2 <= x )
			{
				err += ++x * 2 + 1;
				if( -y == x && e2 <= y )
					e2 = 0;
			}
			if( e2 > y )
				err += ++y * 2 + 1;
		} while( y <= 0 );
	}
}

/**
 * @brief  Displays an Ellipse.
 * @param  Xpos: specifies the X position, can be a value from 0 to 240.
 * @param  Ypos: specifies the Y position, can be a value from 0 to 320.
 * @param  Radius: specifies Radius.
 * @param  Radius2: specifies Radius2.
 * @retval None
 */
void LcdDrawEllipse( int Xpos, int Ypos, int Radius, int Radius2 )
{
	int x = -Radius, y = 0, err = 2 - 2 * Radius, e2;
	float K = 0, rad1 = 0, rad2 = 0;

	rad1 = Radius;
	rad2 = Radius2;

	if( Radius > Radius2 )
	{
		do
		{
			K = (float) ( rad1 / rad2 );
			*(__IO uint16_t*) ( CurrentFrameBuffer
					+ ( 2 * ( ( Xpos - x ) + LCD_PIXEL_WIDTH * ( Ypos + (uint16_t) ( y / K ) ) ) ) ) = CurrentTextColor;
			*(__IO uint16_t*) ( CurrentFrameBuffer
					+ ( 2 * ( ( Xpos + x ) + LCD_PIXEL_WIDTH * ( Ypos + (uint16_t) ( y / K ) ) ) ) ) = CurrentTextColor;
			*(__IO uint16_t*) ( CurrentFrameBuffer
					+ ( 2 * ( ( Xpos + x ) + LCD_PIXEL_WIDTH * ( Ypos - (uint16_t) ( y / K ) ) ) ) ) = CurrentTextColor;
			*(__IO uint16_t*) ( CurrentFrameBuffer
					+ ( 2 * ( ( Xpos - x ) + LCD_PIXEL_WIDTH * ( Ypos - (uint16_t) ( y / K ) ) ) ) ) = CurrentTextColor;

			e2 = err;
			if( e2 <= y )
			{
				err += ++y * 2 + 1;
				if( -x == y && e2 <= x )
					e2 = 0;
			}
			if( e2 > x )
				err += ++x * 2 + 1;
		} while( x <= 0 );
	}
	else
	{
		y = -Radius2;
		x = 0;
		do
		{
			K = (float) ( rad2 / rad1 );
			*(__IO uint16_t*) ( CurrentFrameBuffer
					+ ( 2 * ( ( Xpos - (uint16_t) ( x / K ) ) + LCD_PIXEL_WIDTH * ( Ypos + y ) ) ) ) = CurrentTextColor;
			*(__IO uint16_t*) ( CurrentFrameBuffer
					+ ( 2 * ( ( Xpos + (uint16_t) ( x / K ) ) + LCD_PIXEL_WIDTH * ( Ypos + y ) ) ) ) = CurrentTextColor;
			*(__IO uint16_t*) ( CurrentFrameBuffer
					+ ( 2 * ( ( Xpos + (uint16_t) ( x / K ) ) + LCD_PIXEL_WIDTH * ( Ypos - y ) ) ) ) = CurrentTextColor;
			*(__IO uint16_t*) ( CurrentFrameBuffer
					+ ( 2 * ( ( Xpos - (uint16_t) ( x / K ) ) + LCD_PIXEL_WIDTH * ( Ypos - y ) ) ) ) = CurrentTextColor;

			e2 = err;
			if( e2 <= x )
			{
				err += ++x * 2 + 1;
				if( -y == x && e2 <= y )
					e2 = 0;
			}
			if( e2 > y )
				err += ++y * 2 + 1;
		} while( y <= 0 );
	}
}

/**
 * @brief  Displays a mono-color picture.
 * @param  Pict: pointer to the picture array.
 * @retval None
 */
void LcdDrawMonoPict( const uint32_t *Pict )
{
	uint32_t index = 0, counter = 0;

	for( index = 0; index < 2400; index++ )
	{
		for( counter = 0; counter < 32; counter++ )
		{
			if( ( Pict[index] & ( 1 << counter ) ) == 0x00 )
			{
				*(__IO uint16_t*) ( CurrentFrameBuffer ) = CurrentBackColor;
			}
			else
			{
				*(__IO uint16_t*) ( CurrentFrameBuffer ) = CurrentTextColor;
			}
		}
	}
}

/**
 * @brief  Displays a bitmap picture loaded in the internal Flash.
 * @param  BmpAddress: Bmp picture address in the internal Flash.
 * @retval None
 */
void LcdWriteBMP( uint32_t BmpAddress )
{
	uint32_t index = 0, size = 0, width = 0, height = 0, bit_pixel = 0;
	uint32_t Address;
	uint32_t currentline = 0, linenumber = 0;

	Address = CurrentFrameBuffer;

	/* Read bitmap size */
	size = *(__IO uint16_t *) ( BmpAddress + 2 );
	size |= ( *(__IO uint16_t *) ( BmpAddress + 4 ) ) << 16;

	/* Get bitmap data address offset */
	index = *(__IO uint16_t *) ( BmpAddress + 10 );
	index |= ( *(__IO uint16_t *) ( BmpAddress + 12 ) ) << 16;

	/* Read bitmap width */
	width = *(uint16_t *) ( BmpAddress + 18 );
	width |= ( *(uint16_t *) ( BmpAddress + 20 ) ) << 16;

	/* Read bitmap height */
	height = *(uint16_t *) ( BmpAddress + 22 );
	height |= ( *(uint16_t *) ( BmpAddress + 24 ) ) << 16;

	/* Read bit/pixel */
	bit_pixel = *(uint16_t *) ( BmpAddress + 28 );

	if( CurrentLayer == LCD_BACKGROUND_LAYER )
	{
		/* reconfigure layer size in accordance with the picture */
		LTDC_LayerSize( LTDC_Layer1, width, height );
		LTDC_ReloadConfig( LTDC_VBReload );

		/* Reconfigure the Layer pixel format in accordance with the picture */
		if( ( bit_pixel / 8 ) == 4 )
		{
			LTDC_LayerPixelFormat( LTDC_Layer1, LTDC_Pixelformat_ARGB8888 );
			LTDC_ReloadConfig( LTDC_VBReload );
		}
		else if( ( bit_pixel / 8 ) == 2 )
		{
			LTDC_LayerPixelFormat( LTDC_Layer1, LTDC_Pixelformat_RGB565 );
			LTDC_ReloadConfig( LTDC_VBReload );
		}
		else
		{
			LTDC_LayerPixelFormat( LTDC_Layer1, LTDC_Pixelformat_RGB888 );
			LTDC_ReloadConfig( LTDC_VBReload );
		}
	}
	else
	{
		/* reconfigure layer size in accordance with the picture */
		LTDC_LayerSize( LTDC_Layer2, width, height );
		LTDC_ReloadConfig( LTDC_VBReload );

		/* Reconfigure the Layer pixel format in accordance with the picture */
		if( ( bit_pixel / 8 ) == 4 )
		{
			LTDC_LayerPixelFormat( LTDC_Layer2, LTDC_Pixelformat_ARGB8888 );
			LTDC_ReloadConfig( LTDC_VBReload );
		}
		else if( ( bit_pixel / 8 ) == 2 )
		{
			LTDC_LayerPixelFormat( LTDC_Layer2, LTDC_Pixelformat_RGB565 );
			LTDC_ReloadConfig( LTDC_VBReload );
		}
		else
		{
			LTDC_LayerPixelFormat( LTDC_Layer2, LTDC_Pixelformat_RGB888 );
			LTDC_ReloadConfig( LTDC_VBReload );
		}
	}

	/* compute the real size of the picture (without the header)) */
	size = ( size - index );

	/* bypass the bitmap header */
	BmpAddress += index;

	/* start copie image from the bottom */
	Address += width * ( height - 1 ) * ( bit_pixel / 8 );

	for( index = 0; index < size; index++ )
	{
		*(__IO uint8_t*) ( Address ) = *(__IO uint8_t *) BmpAddress;

		/*jump on next byte */
		BmpAddress++;
		Address++;
		currentline++;

		if( ( currentline / ( bit_pixel / 8 ) ) == width )
		{
			if( linenumber < height )
			{
				linenumber++;
				Address -= ( 2 * width * ( bit_pixel / 8 ) );
				currentline = 0;
			}
		}
	}
}

/**
 * @brief  Displays a full rectangle.
 * @param  Xpos: specifies the X position, can be a value from 0 to 240.
 * @param  Ypos: specifies the Y position, can be a value from 0 to 320.
 * @param  Height: rectangle height.
 * @param  Width: rectangle width.
 * @retval None
 */
void LcdDrawFullRect( uint16_t Xpos, uint16_t Ypos, uint16_t Width, uint16_t Height )
{
	DMA2D_InitTypeDef DMA2D_InitStruct;

	uint32_t Xaddress = 0;
	uint16_t Red_Value = 0, Green_Value = 0, Blue_Value = 0;

	Red_Value = ( 0xF800 & CurrentTextColor ) >> 11;
	Blue_Value = 0x001F & CurrentTextColor;
	Green_Value = ( 0x07E0 & CurrentTextColor ) >> 5;

	Xaddress = CurrentFrameBuffer + 2 * ( LCD_PIXEL_WIDTH * Ypos + Xpos );

	/* configure DMA2D */
	DMA2D_DeInit();
	DMA2D_InitStruct.DMA2D_Mode = DMA2D_R2M;
	DMA2D_InitStruct.DMA2D_CMode = DMA2D_RGB565;
	DMA2D_InitStruct.DMA2D_OutputGreen = Green_Value;
	DMA2D_InitStruct.DMA2D_OutputBlue = Blue_Value;
	DMA2D_InitStruct.DMA2D_OutputRed = Red_Value;
	DMA2D_InitStruct.DMA2D_OutputAlpha = 0x0F;
	DMA2D_InitStruct.DMA2D_OutputMemoryAdd = Xaddress;
	DMA2D_InitStruct.DMA2D_OutputOffset = ( LCD_PIXEL_WIDTH - Width );
	DMA2D_InitStruct.DMA2D_NumberOfLine = Height;
	DMA2D_InitStruct.DMA2D_PixelPerLine = Width;
	DMA2D_Init( &DMA2D_InitStruct );

	/* Start Transfer */
	DMA2D_StartTransfer();

	/* Wait for CTC Flag activation */
	while( DMA2D_GetFlagStatus( DMA2D_FLAG_TC ) == RESET )
	{
	}

	LcdSetTextColor( CurrentTextColor );
}

/**
 * @brief  Displays a full circle.
 * @param  Xpos: specifies the X position, can be a value from 0 to 240.
 * @param  Ypos: specifies the Y position, can be a value from 0 to 320.
 * @param  Radius
 * @retval None
 */
void LcdDrawFullCircle( uint16_t Xpos, uint16_t Ypos, uint16_t Radius )
{
	int32_t D; /* Decision Variable */
	uint32_t CurX;/* Current X Value */
	uint32_t CurY;/* Current Y Value */

	D = 3 - ( Radius << 1 );

	CurX = 0;
	CurY = Radius;

	while( CurX <= CurY )
	{
		if( CurY > 0 )
		{
			LcdDrawLine( Xpos - CurX, Ypos - CurY, 2 * CurY,
			LCD_DIR_VERTICAL );
			LcdDrawLine( Xpos + CurX, Ypos - CurY, 2 * CurY,
			LCD_DIR_VERTICAL );
		}

		if( CurX > 0 )
		{
			LcdDrawLine( Xpos - CurY, Ypos - CurX, 2 * CurX,
			LCD_DIR_VERTICAL );
			LcdDrawLine( Xpos + CurY, Ypos - CurX, 2 * CurX,
			LCD_DIR_VERTICAL );
		}
		if( D < 0 )
		{
			D += ( CurX << 2 ) + 6;
		}
		else
		{
			D += ( ( CurX - CurY ) << 2 ) + 10;
			CurY--;
		}
		CurX++;
	}

	LcdDrawCircle( Xpos, Ypos, Radius );
}

/**
 * @brief  Displays an uni-line (between two points).
 * @param  x1: specifies the point 1 x position.
 * @param  y1: specifies the point 1 y position.
 * @param  x2: specifies the point 2 x position.
 * @param  y2: specifies the point 2 y position.
 * @retval None
 */
void LcdDrawUniLine( uint16_t x1, uint16_t y1, uint16_t x2, uint16_t y2 )
{
	int16_t deltax = 0, deltay = 0, x = 0, y = 0, xinc1 = 0, xinc2 = 0, yinc1 = 0, yinc2 = 0, den = 0, num = 0, numadd =
			0, numpixels = 0, curpixel = 0;

	deltax = ABS( x2 - x1 ); /* The difference between the x's */
	deltay = ABS( y2 - y1 ); /* The difference between the y's */
	x = x1; /* Start x off at the first pixel */
	y = y1; /* Start y off at the first pixel */

	if( x2 >= x1 ) /* The x-values are increasing */
	{
		xinc1 = 1;
		xinc2 = 1;
	}
	else /* The x-values are decreasing */
	{
		xinc1 = -1;
		xinc2 = -1;
	}

	if( y2 >= y1 ) /* The y-values are increasing */
	{
		yinc1 = 1;
		yinc2 = 1;
	}
	else /* The y-values are decreasing */
	{
		yinc1 = -1;
		yinc2 = -1;
	}

	if( deltax >= deltay ) /* There is at least one x-value for every y-value */
	{
		xinc1 = 0; /* Don't change the x when numerator >= denominator */
		yinc2 = 0; /* Don't change the y for every iteration */
		den = deltax;
		num = deltax / 2;
		numadd = deltay;
		numpixels = deltax; /* There are more x-values than y-values */
	}
	else /* There is at least one y-value for every x-value */
	{
		xinc2 = 0; /* Don't change the x for every iteration */
		yinc1 = 0; /* Don't change the y when numerator >= denominator */
		den = deltay;
		num = deltay / 2;
		numadd = deltax;
		numpixels = deltay; /* There are more y-values than x-values */
	}

	for( curpixel = 0; curpixel <= numpixels; curpixel++ )
	{
		LcdPutPixel( x, y ); /* Draw the current pixel */
		num += numadd; /* Increase the numerator by the top of the fraction */
		if( num >= den ) /* Check if numerator >= denominator */
		{
			num -= den; /* Calculate the new numerator value */
			x += xinc1; /* Change the x as appropriate */
			y += yinc1; /* Change the y as appropriate */
		}
		x += xinc2; /* Change the x as appropriate */
		y += yinc2; /* Change the y as appropriate */
	}
}

/**
 * @brief  Displays an triangle.
 * @param  Points: pointer to the points array.
 * @retval None
 */
void LcdTriangle( pPoint Points, uint16_t PointCount )
{
	int16_t X = 0, Y = 0;
	pPoint First = Points;

	if( PointCount != 3 )
	{
		return;
	}

	while( --PointCount )
	{
		X = Points->X;
		Y = Points->Y;
		Points++;
		LcdDrawUniLine( X, Y, Points->X, Points->Y );
	}
	LcdDrawUniLine( First->X, First->Y, Points->X, Points->Y );
}

/**
 * @brief  Fill an triangle (between 3 points).
 * @param  x1..3: x position of triangle point 1..3.
 * @param  y1..3: y position of triangle point 1..3.
 * @retval None
 */
void LcdFillTriangle( uint16_t x1, uint16_t x2, uint16_t x3, uint16_t y1, uint16_t y2, uint16_t y3 )
{

	int16_t deltax = 0, deltay = 0, x = 0, y = 0, xinc1 = 0, xinc2 = 0, yinc1 = 0, yinc2 = 0, den = 0, num = 0, numadd =
			0, numpixels = 0, curpixel = 0;

	deltax = ABS( x2 - x1 ); /* The difference between the x's */
	deltay = ABS( y2 - y1 ); /* The difference between the y's */
	x = x1; /* Start x off at the first pixel */
	y = y1; /* Start y off at the first pixel */

	if( x2 >= x1 ) /* The x-values are increasing */
	{
		xinc1 = 1;
		xinc2 = 1;
	}
	else /* The x-values are decreasing */
	{
		xinc1 = -1;
		xinc2 = -1;
	}

	if( y2 >= y1 ) /* The y-values are increasing */
	{
		yinc1 = 1;
		yinc2 = 1;
	}
	else /* The y-values are decreasing */
	{
		yinc1 = -1;
		yinc2 = -1;
	}

	if( deltax >= deltay ) /* There is at least one x-value for every y-value */
	{
		xinc1 = 0; /* Don't change the x when numerator >= denominator */
		yinc2 = 0; /* Don't change the y for every iteration */
		den = deltax;
		num = deltax / 2;
		numadd = deltay;
		numpixels = deltax; /* There are more x-values than y-values */
	}
	else /* There is at least one y-value for every x-value */
	{
		xinc2 = 0; /* Don't change the x for every iteration */
		yinc1 = 0; /* Don't change the y when numerator >= denominator */
		den = deltay;
		num = deltay / 2;
		numadd = deltax;
		numpixels = deltay; /* There are more y-values than x-values */
	}

	for( curpixel = 0; curpixel <= numpixels; curpixel++ )
	{
		LcdDrawUniLine( x, y, x3, y3 );

		num += numadd; /* Increase the numerator by the top of the fraction */
		if( num >= den ) /* Check if numerator >= denominator */
		{
			num -= den; /* Calculate the new numerator value */
			x += xinc1; /* Change the x as appropriate */
			y += yinc1; /* Change the y as appropriate */
		}
		x += xinc2; /* Change the x as appropriate */
		y += yinc2; /* Change the y as appropriate */
	}

}
/**
 * @brief  Displays an poly-line (between many points).
 * @param  Points: pointer to the points array.
 * @param  PointCount: Number of points.
 * @retval None
 */
void LcdPolyLine( pPoint Points, uint16_t PointCount )
{
	int16_t X = 0, Y = 0;

	if( PointCount < 2 )
	{
		return;
	}

	while( --PointCount )
	{
		X = Points->X;
		Y = Points->Y;
		Points++;
		LcdDrawUniLine( X, Y, Points->X, Points->Y );
	}
}

/**
 * @brief  Displays an relative poly-line (between many points).
 * @param  Points: pointer to the points array.
 * @param  PointCount: Number of points.
 * @param  Closed: specifies if the draw is closed or not.
 *           1: closed, 0 : not closed.
 * @retval None
 */
static void LcdPolyLineRelativeClosed( pPoint Points, uint16_t PointCount, uint16_t Closed )
{
	int16_t X = 0, Y = 0;
	pPoint First = Points;

	if( PointCount < 2 )
	{
		return;
	}
	X = Points->X;
	Y = Points->Y;
	while( --PointCount )
	{
		Points++;
		LcdDrawUniLine( X, Y, X + Points->X, Y + Points->Y );
		X = X + Points->X;
		Y = Y + Points->Y;
	}
	if( Closed )
	{
		LcdDrawUniLine( First->X, First->Y, X, Y );
	}
}

/**
 * @brief  Displays a closed poly-line (between many points).
 * @param  Points: pointer to the points array.
 * @param  PointCount: Number of points.
 * @retval None
 */
void LcdClosedPolyLine( pPoint Points, uint16_t PointCount )
{
	LcdPolyLine( Points, PointCount );
	LcdDrawUniLine( Points->X, Points->Y, ( Points + PointCount - 1 )->X, ( Points + PointCount - 1 )->Y );
}

/**
 * @brief  Displays a relative poly-line (between many points).
 * @param  Points: pointer to the points array.
 * @param  PointCount: Number of points.
 * @retval None
 */
void LcdPolyLineRelative( pPoint Points, uint16_t PointCount )
{
	LcdPolyLineRelativeClosed( Points, PointCount, 0 );
}

/**
 * @brief  Displays a closed relative poly-line (between many points).
 * @param  Points: pointer to the points array.
 * @param  PointCount: Number of points.
 * @retval None
 */
void LcdClosedPolyLineRelative( pPoint Points, uint16_t PointCount )
{
	LcdPolyLineRelativeClosed( Points, PointCount, 1 );
}

/**
 * @brief  Displays a  full poly-line (between many points).
 * @param  Points: pointer to the points array.
 * @param  PointCount: Number of points.
 * @retval None
 */
void LcdFillPolyLine( pPoint Points, uint16_t PointCount )
{

	int16_t X = 0, Y = 0, X2 = 0, Y2 = 0, X_center = 0, Y_center = 0, X_first = 0, Y_first = 0, pixelX = 0, pixelY = 0,
			counter = 0;
	uint16_t IMAGE_LEFT = 0, IMAGE_RIGHT = 0, IMAGE_TOP = 0, IMAGE_BOTTOM = 0;

	IMAGE_LEFT = IMAGE_RIGHT = Points->X;
	IMAGE_TOP = IMAGE_BOTTOM = Points->Y;

	for( counter = 1; counter < PointCount; counter++ )
	{
		pixelX = POLY_X( counter );
		if( pixelX < IMAGE_LEFT )
		{
			IMAGE_LEFT = pixelX;
		}
		if( pixelX > IMAGE_RIGHT )
		{
			IMAGE_RIGHT = pixelX;
		}

		pixelY = POLY_Y( counter );
		if( pixelY < IMAGE_TOP )
		{
			IMAGE_TOP = pixelY;
		}
		if( pixelY > IMAGE_BOTTOM )
		{
			IMAGE_BOTTOM = pixelY;
		}
	}

	if( PointCount < 2 )
	{
		return;
	}

	X_center = ( IMAGE_LEFT + IMAGE_RIGHT ) / 2;
	Y_center = ( IMAGE_BOTTOM + IMAGE_TOP ) / 2;

	X_first = Points->X;
	Y_first = Points->Y;

	while( --PointCount )
	{
		X = Points->X;
		Y = Points->Y;
		Points++;
		X2 = Points->X;
		Y2 = Points->Y;

		LcdFillTriangle( X, X2, X_center, Y, Y2, Y_center );
		LcdFillTriangle( X, X_center, X2, Y, Y_center, Y2 );
		LcdFillTriangle( X_center, X2, X, Y_center, Y2, Y );
	}

	LcdFillTriangle( X_first, X2, X_center, Y_first, Y2, Y_center );
	LcdFillTriangle( X_first, X_center, X2, Y_first, Y_center, Y2 );
	LcdFillTriangle( X_center, X2, X_first, Y_center, Y2, Y_first );
}

/**
 * @brief  Writes command to select the LCD register.
 * @param  LCD_Reg: address of the selected register.
 * @retval None
 */
void LcdWriteCommand( uint8_t LCD_Reg )
{
	/* Reset WRX to send command */
	LcdCtrlLinesWrite( LCD_WRX_GPIO_PORT, LCD_WRX_PIN, Bit_RESET );

	/* Reset LCD control line(/CS) and Send command */
	LcdChipSelect( DISABLE );
	SPI_I2S_SendData( LCD_SPI, LCD_Reg );

	/* Wait until a data is sent(not busy), before config /CS HIGH */

	while( SPI_I2S_GetFlagStatus( LCD_SPI, SPI_I2S_FLAG_TXE ) == RESET )
		;

	while( SPI_I2S_GetFlagStatus( LCD_SPI, SPI_I2S_FLAG_BSY ) != RESET )
		;

	LcdChipSelect( ENABLE );
}

/**
 * @brief  Writes data to select the LCD register.
 *         This function must be used after LCD_WriteCommand() function
 * @param  value: data to write to the selected register.
 * @retval None
 */
void LcdWriteData( uint8_t value )
{
	/* Set WRX to send data */
	LcdCtrlLinesWrite( LCD_WRX_GPIO_PORT, LCD_WRX_PIN, Bit_SET );

	/* Reset LCD control line(/CS) and Send data */
	LcdChipSelect( DISABLE );
	SPI_I2S_SendData( LCD_SPI, value );

	/* Wait until a data is sent(not busy), before config /CS HIGH */

	while( SPI_I2S_GetFlagStatus( LCD_SPI, SPI_I2S_FLAG_TXE ) == RESET )
		;

	while( SPI_I2S_GetFlagStatus( LCD_SPI, SPI_I2S_FLAG_BSY ) != RESET )
		;

	LcdChipSelect( ENABLE );
}

/**
 * @brief  Configure the LCD controller (Power On sequence as described in ILI9341 Datasheet)
 * @param  None
 * @retval None
 */
void LcdPowerOn( void )
{
	LcdWriteCommand( 0xCA );
	LcdWriteData( 0xC3 );
	LcdWriteData( 0x08 );
	LcdWriteData( 0x50 );
	LcdWriteCommand( LCD_POWERB );
	LcdWriteData( 0x00 );
	LcdWriteData( 0xC1 );
	LcdWriteData( 0x30 );
	LcdWriteCommand( LCD_POWER_SEQ );
	LcdWriteData( 0x64 );
	LcdWriteData( 0x03 );
	LcdWriteData( 0x12 );
	LcdWriteData( 0x81 );
	LcdWriteCommand( LCD_DTCA );
	LcdWriteData( 0x85 );
	LcdWriteData( 0x00 );
	LcdWriteData( 0x78 );
	LcdWriteCommand( LCD_POWERA );
	LcdWriteData( 0x39 );
	LcdWriteData( 0x2C );
	LcdWriteData( 0x00 );
	LcdWriteData( 0x34 );
	LcdWriteData( 0x02 );
	LcdWriteCommand( LCD_PRC );
	LcdWriteData( 0x20 );
	LcdWriteCommand( LCD_DTCB );
	LcdWriteData( 0x00 );
	LcdWriteData( 0x00 );
	LcdWriteCommand( LCD_FRC );
	LcdWriteData( 0x00 );
	LcdWriteData( 0x1B );
	LcdWriteCommand( LCD_DFC );
	LcdWriteData( 0x0A );
	LcdWriteData( 0xA2 );
	LcdWriteCommand( LCD_POWER1 );
	LcdWriteData( 0x10 );
	LcdWriteCommand( LCD_POWER2 );
	LcdWriteData( 0x10 );
	LcdWriteCommand( LCD_VCOM1 );
	LcdWriteData( 0x45 );
	LcdWriteData( 0x15 );
	LcdWriteCommand( LCD_VCOM2 );
	LcdWriteData( 0x90 );
	LcdWriteCommand( LCD_MAC );
	LcdWriteData( 0xC8 );
	LcdWriteCommand( LCD_3GAMMA_EN );
	LcdWriteData( 0x00 );
	LcdWriteCommand( LCD_RGB_INTERFACE );
	LcdWriteData( 0xC2 );
	LcdWriteCommand( LCD_DFC );
	LcdWriteData( 0x0A );
	LcdWriteData( 0xA7 );
	LcdWriteData( 0x27 );
	LcdWriteData( 0x04 );

	/* colomn address set */
	LcdWriteCommand( LCD_COLUMN_ADDR );
	LcdWriteData( 0x00 );
	LcdWriteData( 0x00 );
	LcdWriteData( 0x00 );
	LcdWriteData( 0xEF );
	/* Page Address Set */
	LcdWriteCommand( LCD_PAGE_ADDR );
	LcdWriteData( 0x00 );
	LcdWriteData( 0x00 );
	LcdWriteData( 0x01 );
	LcdWriteData( 0x3F );
	LcdWriteCommand( LCD_INTERFACE );
	LcdWriteData( 0x01 );
	LcdWriteData( 0x00 );
	LcdWriteData( 0x06 );

	LcdWriteCommand( LCD_GRAM );
	delay( 200 );

	LcdWriteCommand( LCD_GAMMA );
	LcdWriteData( 0x01 );

	LcdWriteCommand( LCD_PGAMMA );
	LcdWriteData( 0x0F );
	LcdWriteData( 0x29 );
	LcdWriteData( 0x24 );
	LcdWriteData( 0x0C );
	LcdWriteData( 0x0E );
	LcdWriteData( 0x09 );
	LcdWriteData( 0x4E );
	LcdWriteData( 0x78 );
	LcdWriteData( 0x3C );
	LcdWriteData( 0x09 );
	LcdWriteData( 0x13 );
	LcdWriteData( 0x05 );
	LcdWriteData( 0x17 );
	LcdWriteData( 0x11 );
	LcdWriteData( 0x00 );
	LcdWriteCommand( LCD_NGAMMA );
	LcdWriteData( 0x00 );
	LcdWriteData( 0x16 );
	LcdWriteData( 0x1B );
	LcdWriteData( 0x04 );
	LcdWriteData( 0x11 );
	LcdWriteData( 0x07 );
	LcdWriteData( 0x31 );
	LcdWriteData( 0x33 );
	LcdWriteData( 0x42 );
	LcdWriteData( 0x05 );
	LcdWriteData( 0x0C );
	LcdWriteData( 0x0A );
	LcdWriteData( 0x28 );
	LcdWriteData( 0x2F );
	LcdWriteData( 0x0F );

	LcdWriteCommand( LCD_SLEEP_OUT );
	delay( 200 );
	LcdWriteCommand( LCD_DISPLAY_ON );
	/* GRAM start writing */
	LcdWriteCommand( LCD_GRAM );
}

/**
 * @brief  Enables the Display.
 * @param  None
 * @retval None
 */
void LcdDisplayOn( void )
{
	LcdWriteCommand( LCD_DISPLAY_ON );
}

/**
 * @brief  Disables the Display.
 * @param  None
 * @retval None
 */
void LcdDisplayOff( void )
{
	/* Display Off */
	LcdWriteCommand( LCD_DISPLAY_OFF );
}

/**
 * @brief  Configures LCD control lines in Output Push-Pull mode.
 * @note   The LCD_NCS line can be configured in Open Drain mode
 *         when VDDIO is lower than required LCD supply.
 * @param  None
 * @retval None
 */
void LcdCtrlLinesConfig( void )
{
	GPIO_InitTypeDef GPIO_InitStructure;

	/* Enable GPIOs clock*/
	RCC_AHB1PeriphClockCmd( LCD_NCS_GPIO_CLK | LCD_WRX_GPIO_CLK, ENABLE );

	/* Configure NCS in Output Push-Pull mode */
	GPIO_InitStructure.GPIO_Pin = LCD_NCS_PIN;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;
	GPIO_Init( LCD_NCS_GPIO_PORT, &GPIO_InitStructure );

	/* Configure WRX in Output Push-Pull mode */
	GPIO_InitStructure.GPIO_Pin = LCD_WRX_PIN;
	GPIO_Init( LCD_WRX_GPIO_PORT, &GPIO_InitStructure );

	/* Set chip select pin high */
	LcdCtrlLinesWrite( LCD_NCS_GPIO_PORT, LCD_NCS_PIN, Bit_SET );
}

/**
 * @brief  Sets or reset LCD control lines.
 * @param  GPIOx: where x can be B or D to select the GPIO peripheral.
 * @param  CtrlPins: the Control line.
 *   This parameter can be:
 *     @arg LCD_NCS_PIN: Chip Select pin
 *     @arg LCD_NWR_PIN: Read/Write Selection pin
 *     @arg LCD_RS_PIN: Register/RAM Selection pin
 * @param  BitVal: specifies the value to be written to the selected bit.
 *   This parameter can be:
 *     @arg Bit_RESET: to clear the port pin
 *     @arg Bit_SET: to set the port pin
 * @retval None
 */
void LcdCtrlLinesWrite( GPIO_TypeDef* GPIOx, uint16_t CtrlPins, BitAction BitVal )
{
	/* Set or Reset the control line */
	GPIO_WriteBit( GPIOx, (uint16_t) CtrlPins, (BitAction) BitVal );
}

/**
 * @brief  Configures the LCD_SPI interface.
 * @param  None
 * @retval None
 */
void LcdSpiConfig( void )
{
	SPI_InitTypeDef SPI_InitStructure;
	GPIO_InitTypeDef GPIO_InitStructure;

	/* Enable LCD_SPI_SCK_GPIO_CLK, LCD_SPI_MISO_GPIO_CLK and LCD_SPI_MOSI_GPIO_CLK clock */
	RCC_AHB1PeriphClockCmd(
	LCD_SPI_SCK_GPIO_CLK | LCD_SPI_MISO_GPIO_CLK | LCD_SPI_MOSI_GPIO_CLK, ENABLE );

	/* Enable LCD_SPI and SYSCFG clock  */
	RCC_APB2PeriphClockCmd( LCD_SPI_CLK, ENABLE );

	/* Configure LCD_SPI SCK pin */
	GPIO_InitStructure.GPIO_Pin = LCD_SPI_SCK_PIN;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_25MHz;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_DOWN;
	GPIO_Init( LCD_SPI_SCK_GPIO_PORT, &GPIO_InitStructure );

	/* Configure LCD_SPI MISO pin */
	GPIO_InitStructure.GPIO_Pin = LCD_SPI_MISO_PIN;
	GPIO_Init( LCD_SPI_MISO_GPIO_PORT, &GPIO_InitStructure );

	/* Configure LCD_SPI MOSI pin */
	GPIO_InitStructure.GPIO_Pin = LCD_SPI_MOSI_PIN;
	GPIO_Init( LCD_SPI_MOSI_GPIO_PORT, &GPIO_InitStructure );

	/* Connect SPI SCK */
	GPIO_PinAFConfig( LCD_SPI_SCK_GPIO_PORT, LCD_SPI_SCK_SOURCE,
	LCD_SPI_SCK_AF );

	/* Connect SPI MISO */
	GPIO_PinAFConfig( LCD_SPI_MISO_GPIO_PORT, LCD_SPI_MISO_SOURCE,
	LCD_SPI_MISO_AF );

	/* Connect SPI MOSI */
	GPIO_PinAFConfig( LCD_SPI_MOSI_GPIO_PORT, LCD_SPI_MOSI_SOURCE,
	LCD_SPI_MOSI_AF );

	SPI_I2S_DeInit( LCD_SPI );

	/* SPI configuration -------------------------------------------------------*/
	/* If the SPI peripheral is already enabled, don't reconfigure it */
	if( ( LCD_SPI->CR1 & SPI_CR1_SPE ) == 0 )
	{
		SPI_InitStructure.SPI_Direction = SPI_Direction_2Lines_FullDuplex;
		SPI_InitStructure.SPI_Mode = SPI_Mode_Master;
		SPI_InitStructure.SPI_DataSize = SPI_DataSize_8b;
		SPI_InitStructure.SPI_CPOL = SPI_CPOL_Low;
		SPI_InitStructure.SPI_CPHA = SPI_CPHA_1Edge;
		SPI_InitStructure.SPI_NSS = SPI_NSS_Soft;
		/* SPI baudrate is set to 5.6 MHz (PCLK2/SPI_BaudRatePrescaler = 90/16 = 5.625 MHz)
		 to verify these constraints:
		 - ILI9341 LCD SPI interface max baudrate is 10MHz for write and 6.66MHz for read
		 - l3gd20 SPI interface max baudrate is 10MHz for write/read
		 - PCLK2 frequency is set to 90 MHz
		 */
		SPI_InitStructure.SPI_BaudRatePrescaler = SPI_BaudRatePrescaler_16;
		SPI_InitStructure.SPI_FirstBit = SPI_FirstBit_MSB;
		SPI_InitStructure.SPI_CRCPolynomial = 7;
		SPI_Init( LCD_SPI, &SPI_InitStructure );

		/* Enable L3GD20_SPI  */
		SPI_Cmd( LCD_SPI, ENABLE );
	}
}

/**
 * @brief  GPIO config for LTDC.
 * @param  None
 * @retval None
 */
static void LcdGpioAfConfig( void )
{
	GPIO_InitTypeDef GPIO_InitStruct;

	/* Enable GPIOA, GPIOB, GPIOC, GPIOD, GPIOF, GPIOG AHB Clocks */
	RCC_AHB1PeriphClockCmd(
			RCC_AHB1Periph_GPIOA | RCC_AHB1Periph_GPIOB | RCC_AHB1Periph_GPIOC | RCC_AHB1Periph_GPIOD
					| RCC_AHB1Periph_GPIOF | RCC_AHB1Periph_GPIOG, ENABLE );

	/* GPIOs Configuration */
	/*
	 +------------------------+-----------------------+----------------------------+
	 +                       LCD pins assignment                                   +
	 +------------------------+-----------------------+----------------------------+
	 |  LCD_TFT R2 <-> PC.10  |  LCD_TFT G2 <-> PA.06 |  LCD_TFT B2 <-> PD.06      |
	 |  LCD_TFT R3 <-> PB.00  |  LCD_TFT G3 <-> PG.10 |  LCD_TFT B3 <-> PG.11      |
	 |  LCD_TFT R4 <-> PA.11  |  LCD_TFT G4 <-> PB.10 |  LCD_TFT B4 <-> PG.12      |
	 |  LCD_TFT R5 <-> PA.12  |  LCD_TFT G5 <-> PB.11 |  LCD_TFT B5 <-> PA.03      |
	 |  LCD_TFT R6 <-> PB.01  |  LCD_TFT G6 <-> PC.07 |  LCD_TFT B6 <-> PB.08      |
	 |  LCD_TFT R7 <-> PG.06  |  LCD_TFT G7 <-> PD.03 |  LCD_TFT B7 <-> PB.09      |
	 -------------------------------------------------------------------------------
	 |  LCD_TFT HSYNC <-> PC.06  | LCDTFT VSYNC <->  PA.04 |
	 |  LCD_TFT CLK   <-> PG.07  | LCD_TFT DE   <->  PF.10 |
	 -----------------------------------------------------

	 */

	/* GPIOA configuration */
	GPIO_PinAFConfig( GPIOA, GPIO_PinSource3, GPIO_AF_LTDC );
	GPIO_PinAFConfig( GPIOA, GPIO_PinSource4, GPIO_AF_LTDC );
	GPIO_PinAFConfig( GPIOA, GPIO_PinSource6, GPIO_AF_LTDC );
	GPIO_PinAFConfig( GPIOA, GPIO_PinSource11, GPIO_AF_LTDC );
	GPIO_PinAFConfig( GPIOA, GPIO_PinSource12, GPIO_AF_LTDC );

	GPIO_InitStruct.GPIO_Pin = GPIO_Pin_3 | GPIO_Pin_4 | GPIO_Pin_6 | GPIO_Pin_11 | GPIO_Pin_12;

	GPIO_InitStruct.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStruct.GPIO_Mode = GPIO_Mode_AF;
	GPIO_InitStruct.GPIO_OType = GPIO_OType_PP;
	GPIO_InitStruct.GPIO_PuPd = GPIO_PuPd_NOPULL;
	GPIO_Init( GPIOA, &GPIO_InitStruct );

	/* GPIOB configuration */
	GPIO_PinAFConfig( GPIOB, GPIO_PinSource0, 0x09 );
	GPIO_PinAFConfig( GPIOB, GPIO_PinSource1, 0x09 );
	GPIO_PinAFConfig( GPIOB, GPIO_PinSource8, GPIO_AF_LTDC );
	GPIO_PinAFConfig( GPIOB, GPIO_PinSource9, GPIO_AF_LTDC );
	GPIO_PinAFConfig( GPIOB, GPIO_PinSource10, GPIO_AF_LTDC );
	GPIO_PinAFConfig( GPIOB, GPIO_PinSource11, GPIO_AF_LTDC );

	GPIO_InitStruct.GPIO_Pin = GPIO_Pin_0 | GPIO_Pin_1 | GPIO_Pin_8 | GPIO_Pin_9 | GPIO_Pin_10 | GPIO_Pin_11;

	GPIO_Init( GPIOB, &GPIO_InitStruct );

	/* GPIOC configuration */
	GPIO_PinAFConfig( GPIOC, GPIO_PinSource6, GPIO_AF_LTDC );
	GPIO_PinAFConfig( GPIOC, GPIO_PinSource7, GPIO_AF_LTDC );
	GPIO_PinAFConfig( GPIOC, GPIO_PinSource10, GPIO_AF_LTDC );

	GPIO_InitStruct.GPIO_Pin = GPIO_Pin_6 | GPIO_Pin_7 | GPIO_Pin_10;

	GPIO_Init( GPIOC, &GPIO_InitStruct );

	/* GPIOD configuration */
	GPIO_PinAFConfig( GPIOD, GPIO_PinSource3, GPIO_AF_LTDC );
	GPIO_PinAFConfig( GPIOD, GPIO_PinSource6, GPIO_AF_LTDC );

	GPIO_InitStruct.GPIO_Pin = GPIO_Pin_3 | GPIO_Pin_6;

	GPIO_Init( GPIOD, &GPIO_InitStruct );

	/* GPIOF configuration */
	GPIO_PinAFConfig( GPIOF, GPIO_PinSource10, GPIO_AF_LTDC );

	GPIO_InitStruct.GPIO_Pin = GPIO_Pin_10;

	GPIO_Init( GPIOF, &GPIO_InitStruct );

	/* GPIOG configuration */
	GPIO_PinAFConfig( GPIOG, GPIO_PinSource6, GPIO_AF_LTDC );
	GPIO_PinAFConfig( GPIOG, GPIO_PinSource7, GPIO_AF_LTDC );
	GPIO_PinAFConfig( GPIOG, GPIO_PinSource10, 0x09 );
	GPIO_PinAFConfig( GPIOG, GPIO_PinSource11, GPIO_AF_LTDC );
	GPIO_PinAFConfig( GPIOG, GPIO_PinSource12, 0x09 );

	GPIO_InitStruct.GPIO_Pin = GPIO_Pin_6 | GPIO_Pin_7 | GPIO_Pin_10 | GPIO_Pin_11 | GPIO_Pin_12;

	GPIO_Init( GPIOG, &GPIO_InitStruct );

}

/**
 * @brief  Displays a pixel.
 * @param  x: pixel x.
 * @param  y: pixel y.
 * @retval None
 */
static void LcdPutPixel( int16_t x, int16_t y )
{
	if( x < 0 || x > 239 || y < 0 || y > 319 )
	{
		return;
	}
	LcdDrawLine( x, y, 1, LCD_DIR_HORIZONTAL );
}

#ifndef USE_Delay
/**
 * @brief  Inserts a delay time.
 * @param  nCount: specifies the delay time length.
 * @retval None
 */
static void delay( __IO uint32_t nCount )
{
	__IO uint32_t index = 0;
	for( index = nCount; index != 0; index-- )
	{
	}
}
#endif /* USE_Delay*/
/**
 * @}
 */

/**
 * @}
 */

/**
 * @}
 */

/**
 * @}
 */

/**
 * @}
 */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
