/**********************************************************************
* $Id$		sdram_k4s561632j.c			2011-06-02
*//**
* @file		sdram_k4s561632j.c
* @brief	Contains all functions support for SAMSUNG K4S561632J
*			(supported on LPC1788 IAR Olimex Start Kit Rev.B)
* @version	1.0
* @date		02. June. 2011
* @author	NXP MCU SW Application Team
*
* Copyright(C) 2011, NXP Semiconductor
* All rights reserved.
*
***********************************************************************
* Software that is described herein is for illustrative purposes only
* which provides customers with programming information regarding the
* products. This software is supplied "AS IS" without any warranties.
* NXP Semiconductors assumes no responsibility or liability for the
* use of the software, conveys no license or title under any patent,
* copyright, or mask work right to the product. NXP Semiconductors
* reserves the right to make changes in the software without
* notification. NXP Semiconductors also make no representation or
* warranty that such application will be suitable for the specified
* use without further testing or modification.
* Permission to use, copy, modify, and distribute this software and its
* documentation is hereby granted, under NXP Semiconductors'
* relevant copyright in the software, without fee, provided that it
* is used in conjunction with NXP Semiconductors microcontrollers.  This
* copyright, permission, and disclaimer notice must appear in all copies of
* this code.
**********************************************************************/
//#include "bsp.h"
#ifdef __BUILD_WITH_EXAMPLE__
#include "lpc177x_8x_libcfg.h"
#else
#include "lpc177x_8x_libcfg_default.h"
#endif /* __BUILD_WITH_EXAMPLE__ */

#ifdef _EMC

#include "lpc177x_8x_emc.h"
#include "lpc177x_8x_clkpwr.h"
#include "lpc177x_8x_pinsel.h"
#include "lpc177x_8x_timer.h"
#include "sdram_k4s561632j.h"

/* Public Functions ----------------------------------------------------------- */
/** @addtogroup Sdram_K4S561632J
 * @{
 */

/*********************************************************************//**
 * @brief 		Initialize external SDRAM memory Micron K4S561632J
 *				256Mbit(8M x 32)
 * @param[in]	None
 * @return 		None
 **********************************************************************/
void SDRAMInit( void )
{

    volatile uint32_t i;
	volatile unsigned long Dummy;
	LPC_IOCON->P2_16 = 0x21;
	LPC_IOCON->P2_17 = 0x21;
	LPC_IOCON->P2_18 = 0x21;
	LPC_IOCON->P2_20 = 0x21;
	LPC_IOCON->P2_24 = 0x21;
	LPC_IOCON->P2_28 = 0x21;
	LPC_IOCON->P2_29 = 0x21; //
	LPC_IOCON->P2_30 = 0x21; //
	LPC_IOCON->P2_31 = 0x21;
	LPC_IOCON->P3_0 = 0x21;
	LPC_IOCON->P3_1 = 0x21;
	LPC_IOCON->P3_2 = 0x21;
	LPC_IOCON->P3_3 = 0x21;
	LPC_IOCON->P3_4 = 0x21;
	LPC_IOCON->P3_5 = 0x21;
	LPC_IOCON->P3_6 = 0x21;
	LPC_IOCON->P3_7 = 0x21;
	LPC_IOCON->P3_8 = 0x21;
	LPC_IOCON->P3_9 = 0x21;
	LPC_IOCON->P3_10 = 0x21;
	LPC_IOCON->P3_11 = 0x21;
	LPC_IOCON->P3_12 = 0x21;
	LPC_IOCON->P3_13 = 0x21;
	LPC_IOCON->P3_14 = 0x21;
	LPC_IOCON->P3_15 = 0x21;
	LPC_IOCON->P3_16 = 0x21;//
	LPC_IOCON->P3_17 = 0x21;
	LPC_IOCON->P3_18 = 0x21;
	LPC_IOCON->P3_19 = 0x21;
	LPC_IOCON->P3_20 = 0x21;
	LPC_IOCON->P3_21 = 0x21;
	LPC_IOCON->P3_22 = 0x21;
	LPC_IOCON->P3_23 = 0x21;
	LPC_IOCON->P3_24 = 0x21;
	LPC_IOCON->P3_25 = 0x21;
	LPC_IOCON->P3_26 = 0x21;
	LPC_IOCON->P3_27 = 0x21;
	LPC_IOCON->P3_28 = 0x21;
	LPC_IOCON->P3_29 = 0x21;
	LPC_IOCON->P3_30 = 0x21;
	LPC_IOCON->P3_31 = 0x21;//
	LPC_IOCON->P4_0 = 0x21;
	LPC_IOCON->P4_1 = 0x21;
	LPC_IOCON->P4_2 = 0x21;
	LPC_IOCON->P4_3 = 0x21;
	LPC_IOCON->P4_4 = 0x21;
	LPC_IOCON->P4_5 = 0x21;
	LPC_IOCON->P4_6 = 0x21;
	LPC_IOCON->P4_7 = 0x21;
	LPC_IOCON->P4_8 = 0x21;
	LPC_IOCON->P4_9 = 0x21;
	LPC_IOCON->P4_10 = 0x21;
	LPC_IOCON->P4_11 = 0x21;
	LPC_IOCON->P4_12 = 0x21;
	LPC_IOCON->P4_13 = 0x21;
	LPC_IOCON->P4_14 = 0x21;
	LPC_IOCON->P4_25 = 0x21;
	//	EMC_Init();
	// Init SDRAM controller
	LPC_SC->PCONP |= 0x00000800;
	/*Init SDRAM controller*/
	/*Set data read delay*/LPC_SC->EMCDLYCTL |= (8 << 0);
	LPC_SC->EMCDLYCTL |= (8 << 8);

	LPC_SC->EMCDLYCTL |= (8 << 16);

	LPC_EMC->Control = 1;
	LPC_EMC->DynamicReadConfig = 1;
	LPC_EMC->DynamicRasCas0 = 0;
	LPC_EMC->DynamicRasCas0 |= (3 << 8);
	LPC_EMC->DynamicRasCas0 |= (3 << 0);
	LPC_EMC->DynamicRP = P2C(SDRAM_TRP);
	LPC_EMC->DynamicRAS = P2C(SDRAM_TRAS);
	LPC_EMC->DynamicSREX = P2C(SDRAM_TXSR);
	LPC_EMC->DynamicAPR = SDRAM_TAPR;
	LPC_EMC->DynamicDAL = SDRAM_TDAL + P2C(SDRAM_TRP);
	LPC_EMC->DynamicWR = SDRAM_TWR + 0;
	LPC_EMC->DynamicRC = P2C(SDRAM_TRC);
	LPC_EMC->DynamicRFC = P2C(SDRAM_TRFC);
	LPC_EMC->DynamicXSR = P2C(SDRAM_TXSR);
	LPC_EMC->DynamicRRD = P2C(SDRAM_TRRD);
	LPC_EMC->DynamicMRD = SDRAM_TMRD;

	// 13 row, 9 - col, SDRAM
	//	LPC_EMC->DynamicConfig0 = 0x0004680;
	LPC_EMC->DynamicConfig0 = 0x0000680;
	// JEDEC General SDRAM Initialization Sequence
	// DELAY to allow power and clocks to stabilize ~100 us
	// NOP
	LPC_EMC->DynamicControl = 0x0183;
	for (i = 200 * 30; i; i--);
	// PALL
	LPC_EMC->DynamicControl = 0x0103;
	LPC_EMC->DynamicRefresh = 2;
	for (i = 256; i; --i); // > 128 clk
	LPC_EMC->DynamicRefresh = P2C(SDRAM_REFRESH) >> 4;
	// COMM
	LPC_EMC->DynamicControl = 0x00000083; /* Issue MODE command */
	//	Dummy = *((volatile uint32_t *)(SDRAM_BASE_ADDR | (0x32<<13)));
	Dummy = *((volatile uint32_t *) (SDRAM_BASE_ADDR | (0x33 << 12)));
	// NORM
	//	LPC_EMC->DynamicControl = 0x0000;
	LPC_EMC->DynamicControl = 0x0002;
	LPC_EMC->DynamicConfig0 |= ((1 << 19) | (0 << 20));
	for (i = 100000; i; i--);
	//  _DBD32(P2C(7000));
	//  EMC_DynMemConfigB(0,0);
}
#endif /*_EMC*/

/**
 * @}
 */

/*********************************************************************************
**                            End Of File
*********************************************************************************/
