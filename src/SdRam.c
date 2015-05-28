/**
 ******************************************************************************
 * @file    stm32f429i_discovery_sdram.c
 * @author  MCD Application Team
 * @version V1.0.1
 * @date    28-October-2013
 * @brief   This file provides a set of functions needed to drive the
 *          IS42S16400J SDRAM memory mounted on STM32F429I-DISCO Kit.
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

/* Includes ------------------------------------------------------------------*/
#include "SdRam.h"
#include "stm32f4xx.h"
/** @addtogroup Utilities
 * @{
 */

/** @addtogroup STM32F4_DISCOVERY
 * @{
 */

/** @addtogroup STM32429I_DISCO
 * @{
 */

/** @addtogroup STM32429I_DISCO_SDRAM
 * @brief     This file provides a set of functions needed to drive the
 *            IS42S16400J SDRAM memory mounted on STM32429I-DISCO board.
 * @{
 */

/** @defgroup STM32429I_DISCO_SDRAM_Private_Functions
 * @{
 */

#ifndef USE_Delay
static void delay( __IO uint32_t nCount );
#endif /* USE_Delay*/

/**
 * @brief  Configures the FMC and GPIOs to interface with the SDRAM memory.
 *         This function must be called before any read/write operation
 *         on the SDRAM.
 * @param  None
 * @retval None
 */
void SdRamInit( void )
{
	FMC_SDRAMInitTypeDef FmcSdRamCfg;
	FMC_SDRAMTimingInitTypeDef FmcSdRamTimCfg;

	/* GPIO configuration for FMC SDRAM bank */
	SdRamIoConfig();

	/* Enable FMC clock */
	RCC_AHB3PeriphClockCmd( RCC_AHB3Periph_FMC, ENABLE );

	/* FMC Configuration ---------------------------------------------------------*/
	/* FMC SDRAM Bank configuration */
	/* Timing configuration for 90 Mhz of SD clock frequency (180Mhz/2) */
	/* TMRD: 2 Clock cycles */
	FmcSdRamTimCfg.FMC_LoadToActiveDelay = 2;
	/* TXSR: min=70ns (7x11.11ns) */
	FmcSdRamTimCfg.FMC_ExitSelfRefreshDelay = 7;
	/* TRAS: min=42ns (4x11.11ns) max=120k (ns) */
	FmcSdRamTimCfg.FMC_SelfRefreshTime = 4;
	/* TRC:  min=70 (7x11.11ns) */
	FmcSdRamTimCfg.FMC_RowCycleDelay = 7;
	/* TWR:  min=1+ 7ns (1+1x11.11ns) */
	FmcSdRamTimCfg.FMC_WriteRecoveryTime = 2;
	/* TRP:  20ns => 2x11.11ns */
	FmcSdRamTimCfg.FMC_RPDelay = 2;
	/* TRCD: 20ns => 2x11.11ns */
	FmcSdRamTimCfg.FMC_RCDDelay = 2;

	/* FMC SDRAM control configuration */
	FmcSdRamCfg.FMC_Bank = FMC_Bank2_SDRAM;
	/* Row addressing: [7:0] */
	FmcSdRamCfg.FMC_ColumnBitsNumber = FMC_ColumnBits_Number_8b;
	/* Column addressing: [11:0] */
	FmcSdRamCfg.FMC_RowBitsNumber = FMC_RowBits_Number_12b;
	FmcSdRamCfg.FMC_SDMemoryDataWidth = SDRAM_MEMORY_WIDTH;
	FmcSdRamCfg.FMC_InternalBankNumber = FMC_InternalBank_Number_4;
	FmcSdRamCfg.FMC_CASLatency = SDRAM_CAS_LATENCY;
	FmcSdRamCfg.FMC_WriteProtection = FMC_Write_Protection_Disable;
	FmcSdRamCfg.FMC_SDClockPeriod = SDCLOCK_PERIOD;
	FmcSdRamCfg.FMC_ReadBurst = SDRAM_READBURST;
	FmcSdRamCfg.FMC_ReadPipeDelay = FMC_ReadPipe_Delay_1;
	FmcSdRamCfg.FMC_SDRAMTimingStruct = &FmcSdRamTimCfg;

	/* FMC SDRAM bank initialization */
	FMC_SDRAMInit( &FmcSdRamCfg );

	/* FMC SDRAM device initialization sequence */
	SdRamInitSeq();

}

/**
 * @brief  Configures all SDRAM memory I/Os pins.
 * @param  None.
 * @retval None.
 */
void SdRamIoConfig( void )
{
	GPIO_InitTypeDef GpioCfg;

	/* Enable GPIOs clock */
	RCC_AHB1PeriphClockCmd(
			RCC_AHB1Periph_GPIOB | RCC_AHB1Periph_GPIOC | RCC_AHB1Periph_GPIOD
					| RCC_AHB1Periph_GPIOE | RCC_AHB1Periph_GPIOF
					| RCC_AHB1Periph_GPIOG, ENABLE );

	/*-- GPIOs Configuration -----------------------------------------------------*/
	/*
	 +-------------------+--------------------+--------------------+--------------------+
	 +                       SDRAM pins assignment                                      +
	 +-------------------+--------------------+--------------------+--------------------+
	 | PD0  <-> FMC_D2   | PE0  <-> FMC_NBL0  | PF0  <-> FMC_A0    | PG0  <-> FMC_A10   |
	 | PD1  <-> FMC_D3   | PE1  <-> FMC_NBL1  | PF1  <-> FMC_A1    | PG1  <-> FMC_A11   |
	 | PD8  <-> FMC_D13  | PE7  <-> FMC_D4    | PF2  <-> FMC_A2    | PG8  <-> FMC_SDCLK |
	 | PD9  <-> FMC_D14  | PE8  <-> FMC_D5    | PF3  <-> FMC_A3    | PG15 <-> FMC_NCAS  |
	 | PD10 <-> FMC_D15  | PE9  <-> FMC_D6    | PF4  <-> FMC_A4    |--------------------+
	 | PD14 <-> FMC_D0   | PE10 <-> FMC_D7    | PF5  <-> FMC_A5    |
	 | PD15 <-> FMC_D1   | PE11 <-> FMC_D8    | PF11 <-> FMC_NRAS  |
	 +-------------------| PE12 <-> FMC_D9    | PF12 <-> FMC_A6    |
	 | PE13 <-> FMC_D10   | PF13 <-> FMC_A7    |
	 | PE14 <-> FMC_D11   | PF14 <-> FMC_A8    |
	 | PE15 <-> FMC_D12   | PF15 <-> FMC_A9    |
	 +-------------------+--------------------+--------------------+
	 | PB5 <-> FMC_SDCKE1|
	 | PB6 <-> FMC_SDNE1 |
	 | PC0 <-> FMC_SDNWE |
	 +-------------------+

	 */

	/* Common GPIO configuration */
	GpioCfg.GPIO_Mode = GPIO_Mode_AF;
	GpioCfg.GPIO_Speed = GPIO_Speed_50MHz;
	GpioCfg.GPIO_OType = GPIO_OType_PP;
	GpioCfg.GPIO_PuPd = GPIO_PuPd_NOPULL;

	/* GPIOB configuration */
	GPIO_PinAFConfig( GPIOB, GPIO_PinSource5, GPIO_AF_FMC );
	GPIO_PinAFConfig( GPIOB, GPIO_PinSource6, GPIO_AF_FMC );

	GpioCfg.GPIO_Pin = GPIO_Pin_5 | GPIO_Pin_6;

	GPIO_Init( GPIOB, &GpioCfg );

	/* GPIOC configuration */
	GPIO_PinAFConfig( GPIOC, GPIO_PinSource0, GPIO_AF_FMC );

	GpioCfg.GPIO_Pin = GPIO_Pin_0;

	GPIO_Init( GPIOC, &GpioCfg );

	/* GPIOD configuration */
	GPIO_PinAFConfig( GPIOD, GPIO_PinSource0, GPIO_AF_FMC );
	GPIO_PinAFConfig( GPIOD, GPIO_PinSource1, GPIO_AF_FMC );
	GPIO_PinAFConfig( GPIOD, GPIO_PinSource8, GPIO_AF_FMC );
	GPIO_PinAFConfig( GPIOD, GPIO_PinSource9, GPIO_AF_FMC );
	GPIO_PinAFConfig( GPIOD, GPIO_PinSource10, GPIO_AF_FMC );
	GPIO_PinAFConfig( GPIOD, GPIO_PinSource14, GPIO_AF_FMC );
	GPIO_PinAFConfig( GPIOD, GPIO_PinSource15, GPIO_AF_FMC );

	GpioCfg.GPIO_Pin = GPIO_Pin_0 | GPIO_Pin_1 | GPIO_Pin_8
			| GPIO_Pin_9 | GPIO_Pin_10 | GPIO_Pin_14 | GPIO_Pin_15;

	GPIO_Init( GPIOD, &GpioCfg );

	/* GPIOE configuration */
	GPIO_PinAFConfig( GPIOE, GPIO_PinSource0, GPIO_AF_FMC );
	GPIO_PinAFConfig( GPIOE, GPIO_PinSource1, GPIO_AF_FMC );
	GPIO_PinAFConfig( GPIOE, GPIO_PinSource7, GPIO_AF_FMC );
	GPIO_PinAFConfig( GPIOE, GPIO_PinSource8, GPIO_AF_FMC );
	GPIO_PinAFConfig( GPIOE, GPIO_PinSource9, GPIO_AF_FMC );
	GPIO_PinAFConfig( GPIOE, GPIO_PinSource10, GPIO_AF_FMC );
	GPIO_PinAFConfig( GPIOE, GPIO_PinSource11, GPIO_AF_FMC );
	GPIO_PinAFConfig( GPIOE, GPIO_PinSource12, GPIO_AF_FMC );
	GPIO_PinAFConfig( GPIOE, GPIO_PinSource13, GPIO_AF_FMC );
	GPIO_PinAFConfig( GPIOE, GPIO_PinSource14, GPIO_AF_FMC );
	GPIO_PinAFConfig( GPIOE, GPIO_PinSource15, GPIO_AF_FMC );

	GpioCfg.GPIO_Pin = GPIO_Pin_0 | GPIO_Pin_1 | GPIO_Pin_7
			| GPIO_Pin_8 | GPIO_Pin_9 | GPIO_Pin_10 | GPIO_Pin_11 | GPIO_Pin_12
			| GPIO_Pin_13 | GPIO_Pin_14 | GPIO_Pin_15;

	GPIO_Init( GPIOE, &GpioCfg );

	/* GPIOF configuration */
	GPIO_PinAFConfig( GPIOF, GPIO_PinSource0, GPIO_AF_FMC );
	GPIO_PinAFConfig( GPIOF, GPIO_PinSource1, GPIO_AF_FMC );
	GPIO_PinAFConfig( GPIOF, GPIO_PinSource2, GPIO_AF_FMC );
	GPIO_PinAFConfig( GPIOF, GPIO_PinSource3, GPIO_AF_FMC );
	GPIO_PinAFConfig( GPIOF, GPIO_PinSource4, GPIO_AF_FMC );
	GPIO_PinAFConfig( GPIOF, GPIO_PinSource5, GPIO_AF_FMC );
	GPIO_PinAFConfig( GPIOF, GPIO_PinSource11, GPIO_AF_FMC );
	GPIO_PinAFConfig( GPIOF, GPIO_PinSource12, GPIO_AF_FMC );
	GPIO_PinAFConfig( GPIOF, GPIO_PinSource13, GPIO_AF_FMC );
	GPIO_PinAFConfig( GPIOF, GPIO_PinSource14, GPIO_AF_FMC );
	GPIO_PinAFConfig( GPIOF, GPIO_PinSource15, GPIO_AF_FMC );

	GpioCfg.GPIO_Pin = GPIO_Pin_0 | GPIO_Pin_1 | GPIO_Pin_2
			| GPIO_Pin_3 | GPIO_Pin_4 | GPIO_Pin_5 | GPIO_Pin_11 | GPIO_Pin_12
			| GPIO_Pin_13 | GPIO_Pin_14 | GPIO_Pin_15;

	GPIO_Init( GPIOF, &GpioCfg );

	/* GPIOG configuration */
	GPIO_PinAFConfig( GPIOG, GPIO_PinSource0, GPIO_AF_FMC );
	GPIO_PinAFConfig( GPIOG, GPIO_PinSource1, GPIO_AF_FMC );
	GPIO_PinAFConfig( GPIOG, GPIO_PinSource4, GPIO_AF_FMC );
	GPIO_PinAFConfig( GPIOG, GPIO_PinSource5, GPIO_AF_FMC );
	GPIO_PinAFConfig( GPIOG, GPIO_PinSource8, GPIO_AF_FMC );
	GPIO_PinAFConfig( GPIOG, GPIO_PinSource15, GPIO_AF_FMC );

	GpioCfg.GPIO_Pin = GPIO_Pin_0 | GPIO_Pin_1 | GPIO_Pin_4
			| GPIO_Pin_5 | GPIO_Pin_8 | GPIO_Pin_15;

	GPIO_Init( GPIOG, &GpioCfg );
}

/**
 * @brief  Executes the SDRAM memory initialization sequence.
 * @param  None.
 * @retval None.
 */
void SdRamInitSeq( void )
{
	FMC_SDRAMCommandTypeDef SdRamCmdCfg;
	uint32_t tmpr = 0;

	/* Step 3 --------------------------------------------------------------------*/
	/* Configure a clock configuration enable command */
	SdRamCmdCfg.FMC_CommandMode = FMC_Command_Mode_CLK_Enabled;
	SdRamCmdCfg.FMC_CommandTarget = FMC_Command_Target_bank2;
	SdRamCmdCfg.FMC_AutoRefreshNumber = 1;
	SdRamCmdCfg.FMC_ModeRegisterDefinition = 0;
	/* Wait until the SDRAM controller is ready */
	while( FMC_GetFlagStatus( FMC_Bank2_SDRAM, FMC_FLAG_Busy ) != RESET )
	{
	}
	/* Send the command */
	FMC_SDRAMCmdConfig( &SdRamCmdCfg );

	/* Step 4 --------------------------------------------------------------------*/
	/* Insert 100 ms delay */
	__Delay( 10 );

	/* Step 5 --------------------------------------------------------------------*/
	/* Configure a PALL (precharge all) command */
	SdRamCmdCfg.FMC_CommandMode = FMC_Command_Mode_PALL;
	SdRamCmdCfg.FMC_CommandTarget = FMC_Command_Target_bank2;
	SdRamCmdCfg.FMC_AutoRefreshNumber = 1;
	SdRamCmdCfg.FMC_ModeRegisterDefinition = 0;
	/* Wait until the SDRAM controller is ready */
	while( FMC_GetFlagStatus( FMC_Bank2_SDRAM, FMC_FLAG_Busy ) != RESET )
	{
	}
	/* Send the command */
	FMC_SDRAMCmdConfig( &SdRamCmdCfg );

	/* Step 6 --------------------------------------------------------------------*/
	/* Configure a Auto-Refresh command */
	SdRamCmdCfg.FMC_CommandMode = FMC_Command_Mode_AutoRefresh;
	SdRamCmdCfg.FMC_CommandTarget = FMC_Command_Target_bank2;
	SdRamCmdCfg.FMC_AutoRefreshNumber = 4;
	SdRamCmdCfg.FMC_ModeRegisterDefinition = 0;
	/* Wait until the SDRAM controller is ready */
	while( FMC_GetFlagStatus( FMC_Bank2_SDRAM, FMC_FLAG_Busy ) != RESET )
	{
	}
	/* Send the  first command */
	FMC_SDRAMCmdConfig( &SdRamCmdCfg );

	/* Wait until the SDRAM controller is ready */
	while( FMC_GetFlagStatus( FMC_Bank2_SDRAM, FMC_FLAG_Busy ) != RESET )
	{
	}
	/* Send the second command */
	FMC_SDRAMCmdConfig( &SdRamCmdCfg );

	/* Step 7 --------------------------------------------------------------------*/
	/* Program the external memory mode register */
	tmpr = (uint32_t) SDRAM_MODEREG_BURST_LENGTH_2 |
	SDRAM_MODEREG_BURST_TYPE_SEQUENTIAL |
	SDRAM_MODEREG_CAS_LATENCY_3 |
	SDRAM_MODEREG_OPERATING_MODE_STANDARD |
	SDRAM_MODEREG_WRITEBURST_MODE_SINGLE;

	/* Configure a load Mode register command*/
	SdRamCmdCfg.FMC_CommandMode = FMC_Command_Mode_LoadMode;
	SdRamCmdCfg.FMC_CommandTarget = FMC_Command_Target_bank2;
	SdRamCmdCfg.FMC_AutoRefreshNumber = 1;
	SdRamCmdCfg.FMC_ModeRegisterDefinition = tmpr;
	/* Wait until the SDRAM controller is ready */
	while( FMC_GetFlagStatus( FMC_Bank2_SDRAM, FMC_FLAG_Busy ) != RESET )
	{
	}
	/* Send the command */
	FMC_SDRAMCmdConfig( &SdRamCmdCfg );

	/* Step 8 --------------------------------------------------------------------*/

	/* Set the refresh rate counter */
	/* (15.62 us x Freq) - 20 */
	/* Set the device refresh counter */
	FMC_SetRefreshCount( 1386 );
	/* Wait until the SDRAM controller is ready */
	while( FMC_GetFlagStatus( FMC_Bank2_SDRAM, FMC_FLAG_Busy ) != RESET )
	{
	}

	/* Disable write protection */
	FMC_SDRAMWriteProtectionConfig( FMC_Bank2_SDRAM, DISABLE );

	/* Wait until the SDRAM controller is ready */
	while( FMC_GetFlagStatus( FMC_Bank2_SDRAM, FMC_FLAG_Busy ) != RESET )
	{
	}
}

/**
 * @brief  Writes a Entire-word buffer to the SDRAM memory.
 * @param  pBuffer: pointer to buffer.
 * @param  uwWriteAddress: SDRAM memory internal address from which the data will be
 *         written.
 * @param  uwBufferSize: number of words to write.
 * @retval None.
 */
void SdRamWrite( uint32_t* pBuffer, uint32_t uwWriteAddress,
		uint32_t uwBufferSize )
{
	__IO uint32_t write_pointer = (uint32_t) uwWriteAddress;

	/* While there is data to write */
	for( ; uwBufferSize != 0; uwBufferSize-- )
	{
		/* Transfer data to the memory */
		*(uint32_t *) ( SDRAM_BANK_ADDR + write_pointer ) = *pBuffer++;

		/* Increment the address*/
		write_pointer += 4;
	}

}

/**
 * @brief  Reads data buffer from the SDRAM memory.
 * @param  pBuffer: pointer to buffer.
 * @param  ReadAddress: SDRAM memory internal address from which the data will be
 *         read.
 * @param  uwBufferSize: number of words to write.
 * @retval None.
 */
void SdRamRead( uint32_t* pBuffer, uint32_t uwReadAddress,
		uint32_t uwBufferSize )
{
	__IO uint32_t write_pointer = (uint32_t) uwReadAddress;

	/* Wait until the SDRAM controller is ready */
	while( FMC_GetFlagStatus( FMC_Bank2_SDRAM, FMC_FLAG_Busy ) != RESET )
	{
	}

	/* Read data */
	for( ; uwBufferSize != 0x00; uwBufferSize-- )
	{
		*pBuffer++ = *(__IO uint32_t *) ( SDRAM_BANK_ADDR + write_pointer );

		/* Increment the address*/
		write_pointer += 4;
	}
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
	for( index = ( 100000 * nCount ); index != 0; index-- )
	{
	}
}
#endif /* USE_Delay */

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
