/**********************************************************************
* $Id$		lpc177x_8x_mci.c			2011-06-02
*//**
* @file		lpc177x_8x_mci.c
* @brief	Contains all functions support for MCI firmware library
*			on LPC177x_8x
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
**********************************************************************/

#include "LPC177x_8x.h"
#include "lpc_types.h"
#include "lpc177x_8x_mci.h"
#include "lpc177x_8x_gpdma.h"
#include "lpc177x_8x_clkpwr.h"

#define DMA_MCI_SIZE				BLOCK_LENGTH

#define MCI_DMA_WRITE_CHANNEL		(0)
#define MCI_DMA_READ_CHANNEL		(1)

#define _SHIFT(x)						(1 << x)
#define _XSHIFT(x, y)					(x << y)

#define _SHIFT_(x)						(1 >> x)
#define _XSHIFT_(x, y)					(x >> y)


#define MCI_CARDSTATUS_READYFORDATA_P0S		(8)

#define MCI_CARDSTATUS_CURRENTSTATE_POS		(9)
#define MCI_CARDSTATUS_CURRENTSTATE_BMASK	(0x0F)

#define CARDSTATEOF(x)		(_XSHIFT(x, MCI_CARDSTATUS_CURRENTSTATE_POS) & MCI_CARDSTATUS_CURRENTSTATE_BMASK)

#define MCI_CMD8_VOLTAGESUPPLIED_POS		(8)
#define MCI_CMD8_VOLTAGESUPPLIED_BMASK		(0xFF)

#define MCI_CMD8_CHECKPATTERN_POS			(0)
#define MCI_CMD8_CHECKPATTERN_BMASK			(0xFF)

#define MCI_ACMD41_HCS_POS		(30)

#define MCI_OUTPUT_MODE_PUSHPULL		(0)
#define MCI_OUTPUT_MODE_OPENDRAIN		(1)

#define MCI_PWRCTRL_OPENDRAIN_POS		(6)
#define MCI_PWRCTRL_OPENDRAIN_NUMBIT	(1)
#define MCI_PWRCTRL_OPENDRAIN_BMASK		(0x01)

#define MCI_CID_MANUFACTURER_ID_WPOS		(24)	//in word 0
#define MCI_CID_MANUFACTURER_ID_WBMASK		(0xFF)

#define MCI_CID_OEMAPPLICATION_ID_WPOS		(8)		//in word 0
#define MCI_CID_OEMAPPLICATION_ID_WBMASK	(0xFFFF)

#define MCI_CID_PRODUCTNAME_ID_H_WPOS		(0)		//in word 0
#define MCI_CID_PRODUCTNAME_ID_H_WBMASK		(0xFF)

#define MCI_CID_PRODUCTNAME_ID_L_WPOS		(0)		//in word 1
#define MCI_CID_PRODUCTNAME_ID_L_WBMASK		(0xFFFFFFFF)

#define MCI_CID_PRODUCTREVISION_ID_WPOS		(24)	//in word 2
#define MCI_CID_PRODUCTREVISION_ID_WBMASK	(0xFF)

#define MCI_CID_PRODUCTSERIALNUM_ID_H_WPOS		(0)	//in word 2
#define MCI_CID_PRODUCTSERIALNUM_ID_H_WBMASK	(0x00FFFFFF)
#define MCI_CID_PRODUCTSERIALNUM_ID_L_WPOS		(24)	//in word 3
#define MCI_CID_PRODUCTSERIALNUM_ID_L_WBMASK	(0xFF)
#define MCI_CID_PRODUCTSERIALNUM_ID_WBMASK		(0xFFFFFFFF)

#define MCI_CID_RESERVED_ID_WPOS			(20)	//in word 3
#define MCI_CID_RESERVED_ID_WBMASK			(0x1F)

#define MCI_CID_MANUFACTURINGDATE_ID_WPOS	(8)	//in word 3
#define MCI_CID_MANUFACTURINGDATE_ID_WBMASK	(0x0FFF)

#define MCI_CID_CHECKSUM_ID_WPOS			(1)	//in word 3
#define MCI_CID_CHECKSUM_ID_WBMASK			(0x7F)

#define MCI_CID_UNUSED_ID_WPOS				(0)	//in word 3
#define MCI_CID_UNUSED_ID_WBMASK			(0x01)


volatile uint32_t MCI_Block_End_Flag = 0;

volatile uint32_t CardRCA;

volatile en_Mci_CardType MCI_CardType;

// Terminal Counter flag, Error Counter flag for Channel 0
uint32_t dmaWrCh_TermianalCnt, dmaWrCh_ErrorCnt;
uint32_t dmaRdCh_TermianalCnt, dmaRdCh_ErrorCnt;

uint32_t MCI_ReadFifo(uint32_t * dest);
uint32_t MCI_WriteFifo(uint32_t * src);


volatile uint8_t* dataSrcBlock;
volatile uint8_t* dataDestBlock;

volatile uint32_t txBlockCnt=0, rxBlockCnt=0;

/******************************************************************************
** Function name:		MCI_ReadFifo related
**
** Descriptions:
**
**
** parameters:			None
** Returned value:		None
**
******************************************************************************/
uint32_t MCI_ReadFifo(uint32_t * dest)
{
	//copy 8 words (32 bytes) from FIFO
	memcpy(dest, LPC_MCI->FIFO, 32);

	return 0;
}

/******************************************************************************
** Function name:		MCI_WriteFifo related
**
** Descriptions:
**
**
** parameters:			None
** Returned value:		None
**
******************************************************************************/
uint32_t MCI_WriteFifo(uint32_t * src)
{
	//copy 8 words (32 bytes) from FIFO
	memcpy(LPC_MCI->FIFO, src, 32);

	return 0;
}


/******************************************************************************
** Function name:		MCI_Interrupt related
**
** Descriptions:		MCI interrupt handler and related APIs
**
**
** parameters:			None
** Returned value:		None
**
******************************************************************************/
void MCI_TXEnable( void )
{
#if MCI_DMA_ENABLED
	LPC_MCI->MASK0 |= ((DATA_END_INT_MASK)|(ERR_TX_INT_MASK));	/* Enable TX interrupts only */
#else
	LPC_MCI->MASK0 |= ((FIFO_TX_INT_MASK)|(DATA_END_INT_MASK)|(ERR_TX_INT_MASK));	/* FIFO TX interrupts only */
#endif

	return;
}
/*****************************************************************/

void MCI_TXDisable( void )
{
#if MCI_DMA_ENABLED
	LPC_MCI->MASK0 &= ~((DATA_END_INT_MASK)|(ERR_TX_INT_MASK));	/* Enable TX interrupts only */
#else
	LPC_MCI->MASK0 &= ~((FIFO_TX_INT_MASK)|(DATA_END_INT_MASK)|(ERR_TX_INT_MASK));	/* FIFO TX interrupts only */
#endif

	return;
}

/*****************************************************************/
void MCI_RXEnable( void )
{
#if MCI_DMA_ENABLED
	LPC_MCI->MASK0 |= ((DATA_END_INT_MASK)|(ERR_RX_INT_MASK));	/* Enable RX interrupts only */
#else
	LPC_MCI->MASK0 |= ((FIFO_RX_INT_MASK)|(DATA_END_INT_MASK)|(ERR_RX_INT_MASK));	/* FIFO RX interrupts only */
#endif

	return;
}

/*****************************************************************/

void MCI_RXDisable( void )
{
#if MCI_DMA_ENABLED
	LPC_MCI->MASK0 &= ~((DATA_END_INT_MASK)|(ERR_RX_INT_MASK));	/* Enable TX interrupts only */
#else
	LPC_MCI->MASK0 &= ~((FIFO_RX_INT_MASK)|(DATA_END_INT_MASK)|(ERR_RX_INT_MASK));	/* FIFO TX interrupts only */
#endif

	return;
}

/******************************************************************************
** Function name:		MCI_CheckStatus
**
** Descriptions:		MCI Check status before and after the block read and
**						write. Right after the block read and write, this routine
**						is important that, even the FIFO is empty, complete
**						block has been sent, but, data is still being written
**						to the card, this routine is to ensure that the data
**						has been written based on the state of the card, not
**						by the length being set.
**
** parameters:			None
** Returned value:		TRUE or FALSE
**
******************************************************************************/
uint32_t MCI_CheckStatus(void)
{
	uint32_t respValue, retval = MCI_FUNC_FAILED;

	while (1)
	{
		if (MCI_GetCardStatus(&respValue) != MCI_FUNC_OK)
		{
			break;
		}
		else
		{
			/* The only valid state is TRANS per MMC and SD state diagram.
			RCV state may be seen, but, it happens only when TX_ACTIVE or
			RX_ACTIVE occurs before the WRITE_BLOCK and READ_BLOCK cmds are
			being sent, which is not a valid sequence. */
			if(!(respValue & _SHIFT(MCI_CARDSTATUS_READYFORDATA_P0S)))
			{
				retval = MCI_FUNC_NOT_READY;
			}
			else if(CARDSTATEOF(respValue) != MCI_CARDSTATE_TRAN)
			{
				/* Should be in STANDBY state now and ready */
				retval = MCI_FUNC_ERR_STATE;
			}
			else
			{
				return MCI_FUNC_OK;
			}
		}
	}

	return retval;
}

/******************************************************************************
** Function name:		MCI_CmdProcess
**
** Descriptions:		Called by MCI interrupt handler
**						To simplify the process, for card initialization, the
**						CMD interrupts are disabled.
**
**
** parameters:			None
** Returned value:		None
**
******************************************************************************/
void MCI_CmdProcess( void )
{
	uint32_t MCIStatus;

	MCIStatus = LPC_MCI->STATUS;

	if ( MCIStatus &  MCI_CMD_CRC_FAIL )
	{
		LPC_MCI->CLEAR =  MCI_CMD_CRC_FAIL;
	}

	if ( MCIStatus &  MCI_CMD_TIMEOUT )
	{
		LPC_MCI->CLEAR =  MCI_CMD_TIMEOUT;
	}

	/* Cmd Resp End or Cmd Sent */
	if ( MCIStatus &  MCI_CMD_RESP_END )
	{
		LPC_MCI->CLEAR =  MCI_CMD_RESP_END;
	}

	if ( MCIStatus &  MCI_CMD_SENT )
	{
		LPC_MCI->CLEAR =  MCI_CMD_SENT;
	}

	if ( MCIStatus &  MCI_CMD_ACTIVE )
	{
		LPC_MCI->CLEAR =  MCI_CMD_ACTIVE;
	}

	return;
}

/******************************************************************************
** Function name:		MCI_DataErrorProcess
**
** Descriptions:		Called by MCI interrupt handler
**						Process data error.
**
** parameters:			None
** Returned value:		None
**
******************************************************************************/
void MCI_DataErrorProcess( void )
{
	uint32_t MCIStatus;

	MCIStatus = LPC_MCI->STATUS;

	if ( MCIStatus &  MCI_DATA_CRC_FAIL )
	{
		LPC_MCI->CLEAR = MCI_DATA_CRC_FAIL;
	}

	if ( MCIStatus &  MCI_DATA_TIMEOUT )
	{
		LPC_MCI->CLEAR =  MCI_DATA_TIMEOUT;
	}

	/* Underrun or overrun */
	if ( MCIStatus &  MCI_TX_UNDERRUN )
	{
		LPC_MCI->CLEAR = MCI_TX_UNDERRUN;
	}

	if ( MCIStatus &  MCI_RX_OVERRUN )
	{
		LPC_MCI->CLEAR =  MCI_RX_OVERRUN;
	}

	/* Start bit error on data signal */
	if ( MCIStatus &  MCI_START_BIT_ERR )
	{
		LPC_MCI->CLEAR =  MCI_START_BIT_ERR;
	}

	return;
}

/******************************************************************************
** Function name:		MCI_DATA_END_InterruptService
**
** Descriptions:		Called by MCI interrupt handler
**						This is the last interrupt module processing
**                      the block write and	read to and from the MM card.
**
**                      FIFO interrupts are also used when DMA is disabled
**						This routine simply clears the
**                      MCI_Block_End_Flag, and increments counters for debug
**
** parameters:			None
** Returned value:		None
**
******************************************************************************/
void MCI_DATA_END_InterruptService( void )
{
	uint32_t MCIStatus;

	MCIStatus = LPC_MCI->STATUS;
	if ( MCIStatus &  MCI_DATA_END )		/* Data end, and Data block end  */
	{
		LPC_MCI->CLEAR = MCI_DATA_END;

		return;
	}

	if ( MCIStatus &  MCI_DATA_BLK_END )
	{
		LPC_MCI->CLEAR =  MCI_DATA_BLK_END;

		MCI_TXDisable();

		MCI_Block_End_Flag = 0;

		return;
	}

	/* Tx active  */
	if ( MCIStatus & MCI_TX_ACTIVE )
	{

	}

	/* Rx active  */
	if ( MCIStatus & MCI_RX_ACTIVE )
	{

	}

	return;
}

/******************************************************************************
** Function name:	MCI_FIFOInterruptService
**
** Descriptions:	Called by MCI interrupt handler when using FIFO
**					interrupts and DMA is disabled
**
**
** parameters:			None
** Returned value:		None
**
******************************************************************************/
void MCI_FIFOInterruptService( void )
{
#if !MCI_DMA_ENABLED
	uint32_t MCIStatus;

	MCIStatus = LPC_MCI->STATUS;

	if ( MCIStatus & (FIFO_TX_INT_MASK ) )
	{
		/* empty is multiple of 512 block size */
		if ( MCIStatus & MCI_TX_HALF_EMPTY )
		{
			//There's no data, return
			if(dataSrcBlock == NULL)
				return;

			/* write 8 words to fifo */
			MCI_WriteFifo((uint32_t *)&dataSrcBlock[txBlockCnt]);

			txBlockCnt += 32;
		}

		if (txBlockCnt == BLOCK_LENGTH)	/* block complete */
		{
			txBlockCnt = 0;

			/* disable FIFO int until next block write */
			LPC_MCI->MASK0 &= ~(FIFO_TX_INT_MASK);

			/* wait for SD card to complete sending data i.e MCI_DATA_BLK_END interrupt */
		}
	}
	else if ( MCIStatus & (FIFO_RX_INT_MASK) )
	{
		/* if using RX_HALF_FULL remove one ReadFIFO below */
		if ( MCIStatus & MCI_RX_HALF_FULL )
		{
			//There's no store data, return
			if(dataDestBlock == NULL)
				return;

			/* read 8 words from fifo */
			MCI_ReadFifo((uint32_t *)&dataDestBlock[rxBlockCnt]);

			rxBlockCnt += 32;
		}

		/* block complete */
		if (rxBlockCnt == BLOCK_LENGTH)
		{
			rxBlockCnt = 0;
		}
	}
#endif

	return;
}

/*********************************************************************//**
 * @brief		GPDMA interrupt handler sub-routine
 * @param[in]	None
 * @return 		None
 **********************************************************************/
void DMA_IRQHandler (void)
{
	// check GPDMA interrupt on channel 0
	if (GPDMA_IntGetStatus(GPDMA_STAT_INT, MCI_DMA_WRITE_CHANNEL))
	{
		//check interrupt status on channel 0
		// Check counter terminal status
		if(GPDMA_IntGetStatus(GPDMA_STAT_INTTC, MCI_DMA_WRITE_CHANNEL))
		{
			// Clear terminate counter Interrupt pending
			GPDMA_ClearIntPending (GPDMA_STATCLR_INTTC, MCI_DMA_WRITE_CHANNEL);

			dmaWrCh_TermianalCnt++;
		}
		if (GPDMA_IntGetStatus(GPDMA_STAT_INTERR, MCI_DMA_WRITE_CHANNEL))
		{
			// Clear error counter Interrupt pending
			GPDMA_ClearIntPending (GPDMA_STATCLR_INTERR, MCI_DMA_WRITE_CHANNEL);

			dmaWrCh_ErrorCnt++;
		}
	}
	else if (GPDMA_IntGetStatus(GPDMA_STAT_INT, MCI_DMA_READ_CHANNEL))
	{
		//check interrupt status on channel 0
		// Check counter terminal status
		if(GPDMA_IntGetStatus(GPDMA_STAT_INTTC, MCI_DMA_READ_CHANNEL))
		{
			// Clear terminate counter Interrupt pending
			GPDMA_ClearIntPending (GPDMA_STATCLR_INTTC, MCI_DMA_READ_CHANNEL);

			dmaRdCh_TermianalCnt++;
		}
		if (GPDMA_IntGetStatus(GPDMA_STAT_INTERR, MCI_DMA_READ_CHANNEL))
		{
			// Clear error counter Interrupt pending
			GPDMA_ClearIntPending (GPDMA_STATCLR_INTERR, MCI_DMA_READ_CHANNEL);

			dmaRdCh_ErrorCnt++;
		}
	}
}


/******************************************************************************
** Function name:		SDMMC_IRQHandler
**
** Descriptions:		MCI interrupt handler
**						The handler to handle the block data write and read
**						not for the commands.
**
** parameters:			None
** Returned value:		None
**
******************************************************************************/
void MCI_IRQHandler (void)
{
	uint32_t MCI_Status;

	MCI_Status = LPC_MCI->STATUS;

	/* handle MCI_STATUS interrupt */
	if ( MCI_Status & DATA_ERR_INT_MASK )
	{
		MCI_DataErrorProcess();

		return;
	}

	if ( MCI_Status & DATA_END_INT_MASK )
	{
		MCI_DATA_END_InterruptService();

		return;
	}
	else if ( MCI_Status & FIFO_INT_MASK )
	{
		MCI_FIFOInterruptService();

		return;
	}
	else if ( MCI_Status & CMD_INT_MASK )
	{
		MCI_CmdProcess();

		return;
	}
}

/******************************************************************************
** Function name:		MCI_Set_MCIClock
**
** Descriptions:		Set MCI clock rate, during initialization phase < 400K
**						during data phase < 20Mhz.
**
** parameters:			Clock rate to be set
** Returned value:		None
**
******************************************************************************/
void MCI_Set_MCIClock( uint32_t ClockRate )
{
	uint32_t i, ClkValue = 0;

	if ( ClockRate == SLOW_RATE )
	{
		/* slow clock */
		ClkValue |= MCLKDIV_SLOW;
	}
	else if (ClockRate == NORMAL_RATE)
	{
		/* normal clock */
		ClkValue |= MCLKDIV_NORMAL;
	}

	LPC_MCI->CLOCK &= ~(0xFF); /* clear clock divider */

	LPC_MCI->CLOCK |= (1 << 8)  | ClkValue;

	for ( i = 0; i < 0x10; i++ );	/* delay 3MCLK + 2PCLK before next write */

	return;
}

/******************************************************************************
** Function name:		SD_Set_BusWidth
**
** Descriptions:		1-bit bus or 4-bit bus.
**
** parameters:			bus width
** Returned value:		TRUE or FALSE
**
******************************************************************************/
uint32_t SD_Set_BusWidth( uint32_t width )
{
	uint32_t i;

	for ( i = 0; i < 0x10; i++ );	/* delay 3MCLK + 2PCLK  */

	if ( width == SD_1_BIT )
	{
		LPC_MCI->CLOCK &=  ~(1 << 11);	/* 1 bit bus */
	}
	else if ( width == SD_4_BIT )
	{
		LPC_MCI->CLOCK |=  (1 << 11);/* 4 bit bus */
	}

	if ( MCI_Cmd_SendAcmdBusWidth( BUS_WIDTH_4BITS ) != MCI_FUNC_OK )
	{
		return(MCI_FUNC_FAILED);
	}

	return MCI_FUNC_OK;
}

/******************************************************************************
** Function name:		MCI_Init
**
** Descriptions:		Set MCI clock and power registers, setup VIC for
**						data interrupt.
**
** parameters:			powerActiveLevel: the power level that will make the card powered
**
** Returned value:		true or fase, if VIC table is full, return false
**
******************************************************************************/
uint32_t MCI_Init(uint8_t powerActiveLevel )
{
	uint32_t i, retval;

	MCI_CardType = MCI_CARD_UNKNOWN;
	dataSrcBlock = NULL;
	dataDestBlock = NULL;

	LPC_SC->PCONP |= ( 1 << 28 );			/* Enable clock to the MCI block */

	if ( LPC_MCI->CLOCK & (1 << 8) )
	{
		LPC_MCI->CLOCK &= ~(1 << 8);
	}

	if ( LPC_MCI->POWER & 0x02 )
	{
		LPC_MCI->POWER = 0x00;
	}

	for ( i = 0; i < 0x1000; i++ );

	/* Disable all interrupts for now */
	LPC_MCI->MASK0 = 0;

	//SD_CLK
	PINSEL_ConfigPin(1, 2, 2);

	//SD_CMD
	PINSEL_ConfigPin(1, 3, 2);

	//SD_PWR
	PINSEL_ConfigPin(1, 5, 2);

	//SD_DAT_0
	PINSEL_ConfigPin(1, 6, 2);

	//SD_DAT_1
	PINSEL_ConfigPin(1, 7, 2);

	//SD_DAT_2
	PINSEL_ConfigPin(1, 11, 2);

	//SD_DAT_3
	PINSEL_ConfigPin(1, 12, 2);

	// SD_PWR is active high (follows the output of the SD Card interface block).
	if(powerActiveLevel == LOW_LVL)
	{
		LPC_SC->SCS &= ~ 0x08;//Becase on EA board SD_PWR is active low
	}
	else
	{
		LPC_SC->SCS |= 0x08;
	}

	//Setting for timeout problem
	LPC_MCI->DATATMR = 0x1FFFFFFF;

	/*set up clocking default mode, clear any registers as needed */
	LPC_MCI->COMMAND = 0;
	LPC_MCI->DATACTRL = 0;
	LPC_MCI->CLEAR = 0x7FF;		/* clear all pending interrupts */

	LPC_MCI->POWER = 0x02;		/* power up */
	while ( !(LPC_MCI->POWER & 0x02) );

	for ( i = 0; i < 0x100; i++ );

	/* During identification phase, the clock should be less than
	400Khz. Once we pass this phase, the normal clock can be set up
	to 25Mhz on SD card and 20Mhz on MMC card. */
	MCI_Set_MCIClock( SLOW_RATE );

	LPC_MCI->POWER |= 0x01;		/* bit 1 is set already, from power up to power on */

	for ( i = 0; i < 0x2000; i++ );

	NVIC_EnableIRQ(MCI_IRQn);

	retval = MCI_CardInit();

	/* During the initialization phase, to simplify the process, the CMD related
	interrupts are disabled. The DATA related interrupts are enabled when
	the FIFOs are used and just before WRITE_BLOCK READ_BLOCK cmds are issues, and
	disabled after the data block has been written and read. Please also note,
	before WRITE_BLOCK only TX related data interrupts are enabled, and before
	READ_BLOCK only RX related data interrupts are enabled. */
	return MCI_FUNC_OK;
}

/******************************************************************************
** Function name:		MCI_SetOutputMode
**
** Descriptions:
**
** parameters:			None
** Returned value:		Card type.
**
******************************************************************************/
void MCI_SetOutputMode(uint32_t mode)
{
	if(mode == MCI_OUTPUT_MODE_OPENDRAIN)
	{
		/* Set Open Drain output control for MMC */
		LPC_MCI->POWER |= (1 << MCI_PWRCTRL_OPENDRAIN_POS);
	}
	else
	{
		/* Clear Open Drain output control for SD */
		LPC_MCI->POWER &= ~(1 << MCI_PWRCTRL_OPENDRAIN_POS);
	}
}


/******************************************************************************
** Function name:		MCI_SendCmd
**
** Descriptions:		The routine is used to send a CMD to the card
**
** parameters:			CmdIndex, Argument, ExpectResp Flag, AllowTimeout flag
** Returned value:		None
**
******************************************************************************/
void MCI_SendCmd(uint32_t CmdIndex, uint32_t Argument, uint32_t ExpectResp, uint32_t AllowTimeout)
{
	uint32_t i, CmdData = 0;
	uint32_t CmdStatus;

	/* the command engine must be disabled when we modify the argument
	or the peripheral resends */
	while ( (CmdStatus = LPC_MCI->STATUS) & MCI_CMD_ACTIVE )	/* Command in progress. */
	{
		LPC_MCI->COMMAND = 0;
		LPC_MCI->CLEAR = CmdStatus | MCI_CMD_ACTIVE;
	}

	for ( i = 0; i < 0x100; i++ );

	/*set the command details, the CmdIndex should 0 through 0x3F only */
	CmdData |= (CmdIndex & 0x3F);	/* bit 0 through 5 only */

	if ( ExpectResp == EXPECT_NO_RESP )			/* no response */
	{
		CmdData &= ~((1 << 6) | (1 << 7));		/* Clear long response bit as well */
	}
	else if ( ExpectResp == EXPECT_SHORT_RESP )	/* expect short response */
	{
		CmdData |= (1 << 6);
	}
	else if ( ExpectResp == EXPECT_LONG_RESP )	/* expect long response */
	{
		CmdData |= (1 << 6) | (1 << 7);
	}

	if ( AllowTimeout )			/* allow timeout or not */
	{
		CmdData |= (1 << 8);
	}
	else
	{
		CmdData &= ~(1 << 8);
	}

	/*send the command*/
	CmdData |= (1 << 10);		/* This bit needs to be set last. */

	LPC_MCI->ARGUMENT = Argument;	/* Set the argument first, finally command */

	LPC_MCI->COMMAND = CmdData;

	return;
}

/******************************************************************************
** Function name:		MCI_GetCmdResp
**
** Descriptions:		Get response from the card. This module is always used
**						in pair with MCI_SendCmd()
**
** parameters:			Expected cmd data, expect response flag, pointer to the
**						response
**						Expected cmd data should be the same as that in SendCmd()
**						expect response flag could be	EXPECT_NO_RESP
**														EXPECT_SHORT_RESP
**														EXPECT_LONG_RESP
**						if GetCmdResp() is 0, check the pointer to the response
**						field to get the response value, if GetCmdResp() returns
**						non-zero, no need to check the response field, just resend
**						command or bailout.
** Returned value:		Response status, 0 is valid response.
**
******************************************************************************/
uint32_t MCI_GetCmdResp(uint32_t ExpectCmdData, uint32_t ExpectResp, uint32_t *CmdResp)
{
	uint32_t CmdRespStatus = 0;
	uint32_t LastCmdIndex;

	if ( ExpectResp == EXPECT_NO_RESP )
	{
		return (0);
	}

	while (1)
	{
		// Get the status of the component
		CmdRespStatus = LPC_MCI->STATUS;

		if ( CmdRespStatus & (MCI_CMD_TIMEOUT) )
		{
			LPC_MCI->CLEAR = CmdRespStatus | MCI_CMD_TIMEOUT;

			LPC_MCI->COMMAND = 0;
			LPC_MCI->ARGUMENT = 0xFFFFFFFF;

			return (CmdRespStatus);
		}

		if (  CmdRespStatus & MCI_CMD_CRC_FAIL )
		{
			LPC_MCI->CLEAR = CmdRespStatus | MCI_CMD_CRC_FAIL;
			LastCmdIndex = LPC_MCI->COMMAND & 0x003F;

			if ( (LastCmdIndex == CMD1_SEND_OP_COND) || (LastCmdIndex == ACMD41_SEND_APP_OP_COND)
													|| (LastCmdIndex == CMD12_STOP_TRANSMISSION) )
			{
				LPC_MCI->COMMAND = 0;
				LPC_MCI->ARGUMENT = 0xFFFFFFFF;
				break;			/* ignore CRC error if it's a resp for SEND_OP_COND
								or STOP_TRANSMISSION. */
			}
			else
			{
				return (CmdRespStatus);
			}
		}
		else if (CmdRespStatus & MCI_CMD_RESP_END)
		{
			LPC_MCI->CLEAR = CmdRespStatus | MCI_CMD_RESP_END;
			break;	/* cmd response is received, expecting response */
		}
	}

	if ((LPC_MCI->RESP_CMD & 0x3F) != ExpectCmdData)
	{
		/* If the response is not R1, in the response field, the Expected Cmd data
		won't be the same as the CMD data in SendCmd(). Below four cmds have
		R2 or R3 response. We don't need to check if MCI_RESP_CMD is the same
		as the Expected or not. */
		if ((ExpectCmdData != CMD1_SEND_OP_COND) && (ExpectCmdData != ACMD41_SEND_APP_OP_COND)
					&& (ExpectCmdData != CMD2_ALL_SEND_CID) && (ExpectCmdData != CMD9_SEND_CSD))
		{
			CmdRespStatus = INVALID_RESPONSE;	/* Reuse error status */
			return (INVALID_RESPONSE);
		}
	}

	/* Read MCI_RESP0 register assuming it's not long response. */
	if (CmdResp != NULL)
	{
		if (ExpectResp == EXPECT_SHORT_RESP)
		{
			*(CmdResp + 0) = LPC_MCI->RESP0;
			*(CmdResp + 1) = 0;
			*(CmdResp + 2) = 0;
			*(CmdResp + 3) = 0;
		}
		else if (ExpectResp == EXPECT_LONG_RESP)
		{
			*(CmdResp + 0) = LPC_MCI->RESP0;
			*(CmdResp + 1) = LPC_MCI->RESP1;
			*(CmdResp + 2) = LPC_MCI->RESP2;
			*(CmdResp + 3) = LPC_MCI->RESP3;
		}
	}

	return (0);
}

/******************************************************************************
** Function name:		MCI_CmdResp
**
** Descriptions:
**
** parameters:			None
** Returned value:
**
******************************************************************************/
uint32_t MCI_CmdResp(uint32_t CmdIndex, uint32_t Argument,
								uint32_t ExpectResp, uint32_t *CmdResp, uint32_t AllowTimeout)
{
	uint32_t respStatus;
	uint32_t respValue[4];

	MCI_SendCmd(CmdIndex, Argument, ExpectResp, 0);

	if((CmdResp != NULL) || (ExpectResp != EXPECT_NO_RESP))
	{
		respStatus = MCI_GetCmdResp(CmdIndex, ExpectResp, CmdResp);
	}

	return respStatus;
}


/******************************************************************************
** Function name:		MCI_CardReset
**
** Descriptions:		To reset the card, the CMD0 is used and then the card is
**						in idle, . This is the very first command to be sent to
**						initialize either MMC or SD card.
**
** parameters:			None
** Returned value:		Always MCI_FUNC_OK
**
******************************************************************************/
uint32_t MCI_CardReset(void)
{
	/* Send CMD0 command repeatedly until the response is back correctly */
	MCI_SendCmd(CMD0_GO_IDLE_STATE, 0x00000000, EXPECT_NO_RESP, 0);

	return MCI_FUNC_OK;
}

/******************************************************************************
** Function name:		MCI_Cmd_SendOpCond
**
** Descriptions:		CMD1 for MMC
**
** parameters:			None
** Returned value:		true or false, true if card has response back before
**						timeout, false is timeout on the command.
**
******************************************************************************/
uint32_t MCI_Cmd_SendOpCond( void )
{
	uint32_t i, retryCount;
	uint32_t respStatus;
	uint32_t respValue[4];

	uint32_t retval = MCI_FUNC_FAILED;

	retryCount = 0x200;			/* reset retry counter */

	while ( retryCount > 0 )
	{
		respStatus = MCI_CmdResp(CMD1_SEND_OP_COND, OCR_INDEX, EXPECT_SHORT_RESP, (uint32_t *)&respValue[0], 0);

		if(respStatus & MCI_CMD_TIMEOUT)
		{
			retval = MCI_FUNC_TIMEOUT;
		}
		else if ((respValue[0] & 0x80000000) == 0)
		{
			//The card has not finished the power up routine
			retval = MCI_FUNC_BUS_NOT_IDLE;
		}
		else
		{
			retval = MCI_FUNC_OK;
			break;
		}

		for ( i = 0; i < 0x20; i++ );

		retryCount--;
	}

	return(retval);
}


/******************************************************************************
** Function name:		MCI_Cmd_SendIfCond
**
** Descriptions:		CMD55, before sending an ACMD, call this routine first
**
** parameters:			None
** Returned value:		true or false, true if card has responded before timeout.
**						false is timeout.
**
******************************************************************************/
uint32_t MCI_Cmd_SendIfCond(void)
{
	uint32_t i, retryCount;
	uint32_t CmdArgument;
	uint32_t respStatus;
	uint32_t respValue[4];

	uint32_t retval = MCI_FUNC_FAILED;

	uint8_t voltageSupplied = 0x01;//in range 2.7-3.6V
	uint8_t checkPattern = 0xAA;

	CmdArgument = (voltageSupplied << MCI_CMD8_VOLTAGESUPPLIED_POS) | checkPattern;

	retryCount = 20;

	while ( retryCount > 0 )
	{
		respStatus = MCI_CmdResp(CMD8_SEND_IF_COND, CmdArgument, EXPECT_SHORT_RESP, (uint32_t *)&respValue[0], 0);

		if(respStatus & MCI_CMD_TIMEOUT)
		{
			//Consider as no response
			retval = MCI_FUNC_TIMEOUT;
		}
		else if ((respValue[0] & MCI_CMD8_CHECKPATTERN_BMASK) != checkPattern)
		{
			retval = MCI_FUNC_FAILED;
		}
		else if (((respValue[0] >> MCI_CMD8_VOLTAGESUPPLIED_POS) & MCI_CMD8_VOLTAGESUPPLIED_BMASK)
						!= voltageSupplied)
		{
			retval = MCI_FUNC_BAD_PARAMETERS;
		}
		else
		{
			retval = MCI_FUNC_OK;
			break;
		}

		for ( i = 0; i < 0x20; i++ );

		retryCount--;
	}

	return retval;
}


/******************************************************************************
** Function name:		MCI_Cmd_SendACMD
**
** Descriptions:		CMD55, before sending an ACMD, call this routine first
**
** parameters:			None
** Returned value:		true or false, true if card has responded before timeout.
**						false is timeout.
**
******************************************************************************/
uint32_t MCI_Cmd_SendACMD( void )
{
	uint32_t i, retryCount;
	uint32_t CmdArgument;
	uint32_t respStatus;
	uint32_t respValue[4];

	uint32_t retval = MCI_FUNC_FAILED;

	if (MCI_CardType == MCI_SD_CARD)
	{
		CmdArgument = CardRCA;	/* Use the address from SET_RELATIVE_ADDR cmd */
	}
	else			/* if MMC or unknown card type, use 0x0. */
	{
		CmdArgument = 0x00000000;
	}

	retryCount = 20;

	while ( retryCount > 0 )
	{
		respStatus = MCI_CmdResp(CMD55_APP_CMD, CmdArgument, EXPECT_SHORT_RESP, (uint32_t *)&respValue[0], 0);

		if(respStatus != 0)
		{
			retval = MCI_FUNC_FAILED;
		}
		else if (respValue[0] & CARD_STATUS_ACMD_ENABLE)
		{
			retval = MCI_FUNC_OK;
			break;
		}
		else
		{
			retval = MCI_FUNC_NOT_READY;
		}

		for ( i = 0; i < 0x20; i++ );

		retryCount--;
	}

	return retval;
}

/******************************************************************************
** Function name:		MCI_Cmd_SendAcmdOpCond
**
** Descriptions:		If Send_OP_Cond is timeout, it's not a MMC card, try
**						this combination to see if we can communicate with
**						a SD card.
**
** parameters:			hcsVal: to input the Host Capacity Support
** Returned value:		true or false, true if card has been initialized.
**
******************************************************************************/
uint32_t MCI_Cmd_SendAcmdOpCond(uint8_t hcsVal)
{
	uint32_t i, retryCount;
	uint32_t respStatus, argument;
	uint32_t respValue[4];

	uint32_t retval = MCI_FUNC_FAILED;

	argument = OCR_INDEX | (argument << MCI_ACMD41_HCS_POS);

	/* timeout on SEND_OP_COND command on MMC, now, try SEND_APP_OP_COND
	command to SD */
	retryCount = 0x200;			/* reset retry counter */

	while ( retryCount > 0 )
	{
		/* Clear Open Drain output control for SD */
		MCI_SetOutputMode(MCI_OUTPUT_MODE_PUSHPULL);

		for ( i = 0; i < 0x3000; i++ );

		if ((retval = MCI_Cmd_SendACMD()) == MCI_FUNC_OK)
		{
			respStatus = MCI_CmdResp(ACMD41_SEND_APP_OP_COND, argument, EXPECT_SHORT_RESP, (uint32_t *)&respValue[0], 0);

			if(respStatus & MCI_CMD_TIMEOUT)
			{
				retval = MCI_FUNC_TIMEOUT;
			}
			else if (!(respValue[0] & 0x80000000))
			{
				retval = MCI_FUNC_BUS_NOT_IDLE;
			}
			else
			{
				retval = MCI_FUNC_OK;
				break;
			}
		}

		for ( i = 0; i < 0x20; i++ );

		retryCount--;
	}

	return retval;
}

#if 0
/******************************************************************************
** Function name:		MCI_CardInit
**
** Descriptions:		Try CMD1 first for MMC, if it's timeout, try CMD55
**						and CMD41 for SD, if both failed, initialization faliure,
**						bailout with unknown card type. Otherwise, return the
**						card type, either MMC or SD.
**
** parameters:			None
** Returned value:		Card type.
**
******************************************************************************/
uint32_t MCI_CardInit( void )
{
	uint32_t i, retval = MCI_FUNC_FAILED;

	MCI_CardType = MCI_CARD_UNKNOWN;

	if (MCI_CardReset() != MCI_FUNC_OK)
	{
		return MCI_FUNC_FAILED;
	}

	/* Set Open Drain output control for MMC */
	MCI_SetOutputMode(MCI_OUTPUT_MODE_OPENDRAIN);

	for ( i = 0; i < 0x3000; i++ );

	/* Try CMD1 first for MMC, if it's timeout, try CMD55 and CMD41 for SD,
	if both failed, initialization faIlure, bailout. */
	if (MCI_Cmd_SendOpCond() == MCI_FUNC_OK)
	{
		MCI_CardType = MCI_MMC_CARD;

		return MCI_CardType;	/* Found the card, it's a MMC */
	}

	/* Clear Open Drain output control for SD */
	MCI_SetOutputMode(MCI_OUTPUT_MODE_PUSHPULL);

	if (MCI_Cmd_SendAcmdOpCond(0) == MCI_FUNC_OK)
	{
		MCI_CardType = MCI_SD_CARD;

		return MCI_CardType;	/* Found the card, it's a SD */
	}

	/* tried both MMC and SD card, give up */
	return MCI_CardType;
}
#else
/******************************************************************************
** Function name:		MCI_CardInit
**
** Descriptions:		Try CMD1 first for MMC, if it's timeout, try CMD55
**						and CMD41 for SD, if both failed, initialization faliure,
**						bailout with unknown card type. Otherwise, return the
**						card type, either MMC or SD.
**						This is followed Figure 4-2: Card Initialization and
**						Identification Flow (SD mode) in Physical Layer Simplified
**						Specification Version 2.00 document
**
** parameters:			None
** Returned value:		Card type.
**
******************************************************************************/
uint32_t MCI_CardInit( void )
{
	uint32_t CmdArgument;
	uint32_t i, retval = MCI_FUNC_FAILED;

	MCI_CardType = MCI_CARD_UNKNOWN;

	if (MCI_CardReset() != MCI_FUNC_OK)
	{
		return MCI_FUNC_FAILED;
	}

	/* Clear Open Drain output control for SD */
	MCI_SetOutputMode(MCI_OUTPUT_MODE_PUSHPULL);

	for ( i = 0; i < 0x3000; i++ );

	retval = MCI_Cmd_SendIfCond();

	if(retval == MCI_FUNC_BAD_PARAMETERS)
	{
		//Unknow card is unusable
		return retval;
	}

	if(retval == MCI_FUNC_OK)
	{
		//Check in case of High Capacity Supporting Host
		if ((retval = MCI_Cmd_SendAcmdOpCond(1)) == MCI_FUNC_OK)
		{
			MCI_CardType = MCI_SD_CARD;//Support High Capacity

			return MCI_CardType;	/* Found the card, it's a hD */
		}
	}

	if(retval != MCI_FUNC_OK)
	{
		//Check in case of Standard Capacity Supporting Host
		if ((retval = MCI_Cmd_SendAcmdOpCond(0)) == MCI_FUNC_OK)
		{
			MCI_CardType = MCI_SD_CARD;//Support Standard Capacity only

			return MCI_CardType;	/* Found the card, it's a SD */
		}
	}

	if(retval != MCI_FUNC_OK)
	{
		/* Set Open Drain output control for MMC */
		MCI_SetOutputMode(MCI_OUTPUT_MODE_OPENDRAIN);

		for ( i = 0; i < 0x3000; i++ );

		/* Try CMD1 first for MMC, if it's timeout, try CMD55 and CMD41 for SD,
		if both failed, initialization faIlure, bailout. */
		if (MCI_Cmd_SendOpCond() == MCI_FUNC_OK)
		{
			MCI_CardType = MCI_MMC_CARD;

			return MCI_CardType;	/* Found the card, it's a MMC */
		}
	}

	/* tried both MMC and SD card, give up */
	return MCI_CardType;
}

#endif

/******************************************************************************
** Function name:		MCI_GetCardType
**
** Descriptions:
**
** parameters:			None
** Returned value:		Card type.
**
******************************************************************************/
en_Mci_CardType MCI_GetCardType(void)
{
	return MCI_CardType;
}

/******************************************************************************
** Function name:		MCI_GetCID
**
** Descriptions:		Send CMD2, CMD2_ALL_SEND_CID
**
** parameters:			None
** Returned value:		If not timeout, return true.
**
******************************************************************************/
uint32_t MCI_GetCID(st_Mci_CardId* cidValue)
{
	uint32_t i, retryCount;
	uint32_t respStatus;
	uint32_t respValue[4];

	/* This command is normally after CMD1(MMC) or ACMD41(SD). */
	retryCount = 0x200;// 0x20;			/* reset retry counter */

	while ( retryCount > 0 )
	{
#if 1
		respStatus = MCI_CmdResp(CMD2_ALL_SEND_CID, 0, EXPECT_LONG_RESP, (uint32_t *)&respValue[0], 0);

		/* bit 0 and bit 2 must be zero, or it's timeout or CRC error */
		//if ((!(respStatus & MCI_CMD_TIMEOUT)) && (!(respStatus & MCI_CMD_CRC_FAIL)))
		if (!(respStatus & MCI_CMD_TIMEOUT))
		{
			// Parsing the data retrieved
			if(cidValue != NULL)
			{
				cidValue->MID = (respValue[0] >> MCI_CID_MANUFACTURER_ID_WPOS) & MCI_CID_MANUFACTURER_ID_WBMASK;

				cidValue->OID = (respValue[0] >> MCI_CID_OEMAPPLICATION_ID_WPOS) & MCI_CID_OEMAPPLICATION_ID_WBMASK;

				cidValue->PNM_H = (respValue[0] >> MCI_CID_PRODUCTNAME_ID_H_WPOS) & MCI_CID_PRODUCTNAME_ID_H_WBMASK;

				cidValue->PNM_L = (respValue[1] >> MCI_CID_PRODUCTNAME_ID_L_WPOS) & MCI_CID_PRODUCTNAME_ID_L_WBMASK;

				cidValue->PRV = (respValue[2] >> MCI_CID_PRODUCTREVISION_ID_WPOS) & MCI_CID_PRODUCTREVISION_ID_WBMASK;

				cidValue->PSN = (((respValue[2] >> MCI_CID_PRODUCTSERIALNUM_ID_H_WPOS) & MCI_CID_PRODUCTSERIALNUM_ID_H_WBMASK) << 8)
											| (respValue[3] >> MCI_CID_PRODUCTSERIALNUM_ID_L_WPOS) & MCI_CID_PRODUCTSERIALNUM_ID_L_WBMASK;

				cidValue->reserved = (respValue[3] >> MCI_CID_RESERVED_ID_WPOS) & MCI_CID_RESERVED_ID_WBMASK;

				cidValue->MDT = (respValue[3] >> MCI_CID_MANUFACTURINGDATE_ID_WPOS) & MCI_CID_MANUFACTURINGDATE_ID_WBMASK;

				cidValue->CRC = (respValue[3] >> MCI_CID_CHECKSUM_ID_WPOS) & MCI_CID_CHECKSUM_ID_WBMASK;

				cidValue->unused = (respValue[3] >> MCI_CID_UNUSED_ID_WPOS) & MCI_CID_UNUSED_ID_WBMASK;

			}

			return  MCI_FUNC_OK;	/* response is back and correct. */
		}
#else
		respCmdValue[0] = 0x00000000;
		respCmdValue[1] = 0x00000000;
		respCmdValue[2] = 0x00000000;
		respCmdValue[3] = 0x00000000;

		respStatus = MCI_CmdResp(CMD2_ALL_SEND_CID, 0, EXPECT_LONG_RESP, (uint32_t *)&respCmdValue[0], 0);

		/* bit 0 and bit 2 must be zero, or it's timeout or CRC error */
		if (!(respStatus & MCI_CMD_TIMEOUT))
		{
			// Parsing the data retrieved
			if(cidValue != NULL)
			{
				cidValue->MID = (respCmdValue[0] >> MCI_CID_MANUFACTURER_ID_WPOS) & MCI_CID_MANUFACTURER_ID_WBMASK;

				cidValue->OID = (respCmdValue[0] >> MCI_CID_OEMAPPLICATION_ID_WPOS) & MCI_CID_OEMAPPLICATION_ID_WBMASK;

				cidValue->PNM_H = (respCmdValue[0] >> MCI_CID_PRODUCTNAME_ID_H_WPOS) & MCI_CID_PRODUCTNAME_ID_H_WBMASK;

				cidValue->PNM_L = (respCmdValue[1] >> MCI_CID_PRODUCTNAME_ID_L_WPOS) & MCI_CID_PRODUCTNAME_ID_L_WBMASK;

				cidValue->PRV = (respCmdValue[2] >> MCI_CID_PRODUCTREVISION_ID_WPOS) & MCI_CID_PRODUCTREVISION_ID_WBMASK;

				cidValue->PSN = (((respCmdValue[2] >> MCI_CID_PRODUCTSERIALNUM_ID_H_WPOS) & MCI_CID_PRODUCTSERIALNUM_ID_H_WBMASK) << 8)
											| (respCmdValue[3] >> MCI_CID_PRODUCTSERIALNUM_ID_L_WPOS) & MCI_CID_PRODUCTSERIALNUM_ID_L_WBMASK;

				cidValue->reserved = (respCmdValue[3] >> MCI_CID_RESERVED_ID_WPOS) & MCI_CID_RESERVED_ID_WBMASK;

				cidValue->MDT = (respCmdValue[3] >> MCI_CID_MANUFACTURINGDATE_ID_WPOS) & MCI_CID_MANUFACTURINGDATE_ID_WBMASK;

				cidValue->CRC = (respCmdValue[3] >> MCI_CID_CHECKSUM_ID_WPOS) & MCI_CID_CHECKSUM_ID_WBMASK;

				cidValue->unused = (respCmdValue[3] >> MCI_CID_UNUSED_ID_WPOS) & MCI_CID_UNUSED_ID_WBMASK;

			}

			return	MCI_FUNC_OK; /* response is back and correct. */
		}
#endif

		for ( i = 0; i < 0x20; i++ );

		retryCount--;
	}

	return MCI_FUNC_TIMEOUT;
}


/******************************************************************************
** Function name:		MCI_SetCardAddress
**
** Descriptions:		Send CMD3, STE_RELATIVE_ADDR, should after CMD2
**
** parameters:			None
** Returned value:		TRUE if response is back before timeout.
**
******************************************************************************/
uint32_t MCI_SetCardAddress( void )
{
	uint32_t i, retryCount;
	uint32_t respStatus;
	uint32_t respValue[4];
	uint32_t CmdArgument;

	uint32_t retval = MCI_FUNC_FAILED;

	/* If it's a SD card, SET_RELATIVE_ADDR is to get the address
	from the card and use this value in RCA, if it's a MMC, set default
	RCA addr. 0x00010000. */
	if (MCI_CardType == MCI_SD_CARD)
	{
		CmdArgument = 0;
	}
	else			/* If it's unknown or MMC_CARD, fix the RCA address */
	{
		CmdArgument = 0x00010000;
	}

	retryCount = 0x20;			/* reset retry counter */

	while ( retryCount > 0 )
	{
		/* Send CMD3 command repeatedly until the response is back correctly */
		respStatus = MCI_CmdResp(CMD3_SET_RELATIVE_ADDR, 0, EXPECT_SHORT_RESP, (uint32_t *)&respValue[0], 0);

		if(respStatus & MCI_CMD_TIMEOUT)
		{
			retval = MCI_FUNC_TIMEOUT;
		}
		else if(!(respValue[0] & _SHIFT(MCI_CARDSTATUS_READYFORDATA_P0S)))
		{
			retval = MCI_FUNC_NOT_READY;
		}
		else if((CARDSTATEOF(respValue[0]) != MCI_CARDSTATE_IDENDTIFIED))
		{
			retval = MCI_FUNC_ERR_STATE;
		}
		else
		{
			CardRCA = respValue[0] & 0xFFFF0000;	/* Save the RCA value from SD card */

			return (MCI_FUNC_OK);	/* response is back and correct. */
		}

		for ( i = 0; i < 0x20; i++ );

		retryCount--;
	}

	return retval;
}

/******************************************************************************
** Function name:		MCI_GetCSD
**
** Descriptions:		CMD9, SEND_CSD cmd, it should be sent only at
**						STBY state and after CMD3. See MMC and SD spec. state
**						diagram.
**
** parameters:			None
** Returned value:		Response value
**
******************************************************************************/
uint32_t MCI_GetCSD(uint32_t* csdVal)
{
	uint32_t i, retryCount;
	uint32_t respStatus;
	uint32_t respValue[4];
	uint32_t CmdArgument;

	if (MCI_CardType == MCI_SD_CARD)
	{
		CmdArgument = CardRCA;
	}
	else			/* if MMC or unknown card type, use default RCA addr. */
	{
		CmdArgument = 0x00010000;
	}

	retryCount = 0x20;

	while ( retryCount > 0 )
	{
		/* Send SET_BLOCK_LEN command before read and write */
		LPC_MCI->CLEAR |= (MCI_CMD_TIMEOUT | MCI_CMD_CRC_FAIL | MCI_CMD_RESP_END);

		respStatus = MCI_CmdResp(CMD9_SEND_CSD, CmdArgument, EXPECT_LONG_RESP, (uint32_t *)&respValue[0], 0);

		if ( !respStatus )
		{
			if(csdVal != NULL)
			{
				csdVal[0] = respValue[0];
				csdVal[1] = respValue[1];
				csdVal[2] = respValue[2];
				csdVal[3] = respValue[3];
			}

			return (MCI_FUNC_OK);
		}

		for ( i = 0; i < 0x20; i++ );

		retryCount--;
	}

	return (MCI_FUNC_FAILED);
}

/******************************************************************************
** Function name:		MCI_Cmd_SelectCard
**
** Descriptions:		CMD7, SELECT_CARD, should be after CMD9, the state
**						will be inter-changed between STBY and TRANS after
**						this cmd.
**
** parameters:			None
** Returned value:		return false if response times out.
**
******************************************************************************/
uint32_t MCI_Cmd_SelectCard( void )
{
	uint32_t i, retryCount;
	uint32_t respStatus;
	uint32_t respValue[4];
	uint32_t CmdArgument;

	uint32_t retval = MCI_FUNC_FAILED;

	if (MCI_CardType == MCI_SD_CARD)
	{
		CmdArgument = CardRCA;
	}
	else			/* if MMC or unknown card type, use default RCA addr. */
	{
		CmdArgument = 0x00010000;
	}

	retryCount = 0x20;

	while ( retryCount > 0 )
	{
		/* Send CMD7_SELECT_CARD command before read and write */
		LPC_MCI->CLEAR |= (MCI_CMD_TIMEOUT | MCI_CMD_CRC_FAIL | MCI_CMD_RESP_END);

		respStatus = MCI_CmdResp(CMD7_SELECT_CARD, CmdArgument, EXPECT_SHORT_RESP, (uint32_t *)&respValue[0], 0);

		if(respStatus)
		{
			retval = MCI_FUNC_FAILED;
		}
		else if(!(respValue[0] & _SHIFT(MCI_CARDSTATUS_READYFORDATA_P0S)))
		{
			retval = MCI_FUNC_NOT_READY;
		}
		else if(CARDSTATEOF(respValue[0]) != MCI_CARDSTATE_STBY)
		{
			/* Should be in STANDBY state now and ready */
			retval = MCI_FUNC_ERR_STATE;
		}
		else
		{
			return MCI_FUNC_OK;
		}

		for ( i = 0; i < 0x20; i++ );

		retryCount--;
	}

	return retval;
}

/******************************************************************************
** Function name:		MCI_GetCardStatus
**
** Descriptions:		CMD13, SEND_STATUS, the most important cmd to
**						debug the state machine of the card.
**
** parameters:			None
** Returned value:		Response value(card status), true if the ready bit
**						is set in the card status register, if timeout, return
**						INVALID_RESPONSE 0xFFFFFFFF.
**
******************************************************************************/
uint32_t MCI_GetCardStatus(uint32_t* cardStatus)
{
	uint32_t retryCount;
	uint32_t respStatus;
	uint32_t respValue[4];
	uint32_t CmdArgument;

	uint32_t retval = MCI_FUNC_FAILED;

	if (MCI_CardType == MCI_SD_CARD)
	{
		CmdArgument = CardRCA;
	}
	else			/* if MMC or unknown card type, use default RCA addr. */
	{
		CmdArgument = 0x00010000;
	}

	/* Note that, since it's called after the block write and read, this timeout
	is important based on the clock you set for the data communication. */
	retryCount = 0x2000;

	while ( retryCount > 0 )
	{
		/* Send SELECT_CARD command before read and write */
		LPC_MCI->CLEAR |= (MCI_CMD_TIMEOUT | MCI_CMD_CRC_FAIL | MCI_CMD_RESP_END);

		respStatus = MCI_CmdResp(CMD13_SEND_STATUS, CmdArgument, EXPECT_SHORT_RESP, (uint32_t *)&respValue[0], 0);

		if(respStatus)
		{
			retval = MCI_FUNC_FAILED;
		}
		else if(!(respValue[0] & _SHIFT(MCI_CARDSTATUS_READYFORDATA_P0S)))
		{
			retval = MCI_FUNC_NOT_READY;
		}
		else
		{
			/* The ready bit should be set, it should be in either TRAN or RCV state now */
			if(cardStatus != NULL)
			{
				*cardStatus = respValue[0];
			}

			return MCI_FUNC_OK;
		}

		retryCount--;
	}

	return retval;
}

/******************************************************************************
** Function name:		MCI_SetBlockLen
**
** Descriptions:		CMD16, SET_BLOCKLEN, called after CMD7(SELECT_CARD)
**						called in the TRANS state.
**
** parameters:			The length of the data block to be written or read.
** Returned value:		true or false, return TRUE if ready bit is set, and it's
**						in TRANS state.
**
******************************************************************************/
uint32_t MCI_SetBlockLen(uint32_t blockLength)
{
	uint32_t i, retryCount;
	uint32_t respStatus;
	uint32_t respValue[4];

	uint32_t retval = MCI_FUNC_FAILED;

	retryCount = 0x20;
	while ( retryCount > 0 )
	{
		/* Send SET_BLOCK_LEN command before read and write */
		LPC_MCI->CLEAR |= (MCI_CMD_TIMEOUT | MCI_CMD_CRC_FAIL | MCI_CMD_RESP_END);

		respStatus = MCI_CmdResp(CMD16_SET_BLOCK_LEN, blockLength, EXPECT_SHORT_RESP, (uint32_t *)&respValue[0], 0);

		if(respStatus)
		{
			retval = MCI_FUNC_FAILED;
		}
		else if(!(respValue[0] & _SHIFT(MCI_CARDSTATUS_READYFORDATA_P0S)))
		{
			retval = MCI_FUNC_NOT_READY;
		}
		else if((CARDSTATEOF(respValue[0]) != MCI_CARDSTATE_TRAN))
		{
			retval = MCI_FUNC_ERR_STATE;
		}
		else
		{
			return MCI_FUNC_OK;
		}

		for ( i = 0; i < 0x20; i++ );

		retryCount--;
	}

	return retval;
}

/******************************************************************************
** Function name:		MCI_Cmd_SendAcmdBusWidth
**
** Descriptions:		ACMD6, SET_BUS_WIDTH, if it's SD card, we can
**						use the 4-bit bus instead of 1-bit. This cmd
**						can only be called during TRANS state.
**						Since it's a ACMD, CMD55 APP_CMD needs to be
**						sent out first.
**
** parameters:			Bus width value, 1-bit is 0, 4-bit is 10
** Returned value:		true or false, true if the card is still in the
**						TRANS state after the cmd.
**
******************************************************************************/
uint32_t MCI_Cmd_SendAcmdBusWidth( uint32_t buswidth )
{
	uint32_t i, retryCount;
	uint32_t respStatus;
	uint32_t respValue[4];

	uint32_t retval = MCI_FUNC_FAILED;

	retryCount = 0x20;			/* reset retry counter */

	while ( retryCount > 0 )
	{
		if (MCI_Cmd_SendACMD() == MCI_FUNC_OK)
		{
			respStatus = MCI_CmdResp(CMD6_SET_ACMD_BUS_WIDTH, buswidth, EXPECT_SHORT_RESP, (uint32_t *)&respValue[0], 0);

			if(respStatus)
			{
				retval = MCI_FUNC_FAILED;
			}
			else if(!(respValue[0] & _SHIFT(MCI_CARDSTATUS_READYFORDATA_P0S)))
			{
				retval = MCI_FUNC_NOT_READY;
			}
			else if((CARDSTATEOF(respValue[0]) != MCI_CARDSTATE_TRAN))
			{
				retval = MCI_FUNC_ERR_STATE;
			}
			else
			{
				return MCI_FUNC_OK;
			}
		}

		for ( i = 0; i < 0x20; i++ );

		retryCount--;
	}

	return retval;
}

/******************************************************************************
** Function name:		MCI_GetCardType
**
** Descriptions:
**
** parameters:			None
** Returned value:		Card type.
**
******************************************************************************/
uint32_t MCI_GetBlockEndFlag(void)
{
	return MCI_Block_End_Flag;
}


/******************************************************************************
** Function name:		MCI_Cmd_StopTransmission
**
** Descriptions:		CMD12, STOP_TRANSMISSION. if that happens, the card is
**						maybe in a unknown state that need a warm reset.
**
** parameters:			None
** Returned value:		true or false, true if, at least, the card status
**						shows ready bit is set.
**
******************************************************************************/
uint32_t MCI_Cmd_StopTransmission( void )
{
	uint32_t i, retryCount;
	uint32_t respStatus;
	uint32_t respValue[4];

	uint32_t retval = MCI_FUNC_FAILED;

	retryCount = 0x20;

	while ( retryCount > 0 )
	{
		LPC_MCI->CLEAR = 0x7FF;

		respStatus = MCI_CmdResp(CMD12_STOP_TRANSMISSION, 0x00000000, EXPECT_SHORT_RESP, (uint32_t *)&respValue[0], 0);

		if(respStatus)
		{
			retval = MCI_FUNC_FAILED;
		}
		else if(!(respValue[0] & _SHIFT(MCI_CARDSTATUS_READYFORDATA_P0S)))
		{
			retval = MCI_FUNC_NOT_READY;
		}
		else
		{
			return MCI_FUNC_OK;
		}

		for ( i = 0; i < 0x20; i++ );

		retryCount--;
	}

	return retval;
}

/******************************************************************************
** Function name:		MCI_Cmd_WriteBlock
**
** Descriptions:		CMD24, WRITE_BLOCK, send this cmd in the TRANS state
**						to write a block of data to the card.
**
** parameters:			block number
** Returned value:		Response value
**
******************************************************************************/
uint32_t MCI_Cmd_WriteBlock( uint32_t blockNum )
{
	uint32_t i, retryCount;
	uint32_t respStatus;
	uint32_t respValue[4];

	uint32_t retval = MCI_FUNC_FAILED;

	retryCount = 0x20;
	while ( retryCount > 0 )
	{
		LPC_MCI->CLEAR = 0x7FF;

		respStatus = MCI_CmdResp(CMD24_WRITE_BLOCK, blockNum * BLOCK_LENGTH, EXPECT_SHORT_RESP, (uint32_t *)&respValue[0], 0);

		if(respStatus)
		{
			retval = MCI_FUNC_FAILED;
		}
		else if(!(respValue[0] & _SHIFT(MCI_CARDSTATUS_READYFORDATA_P0S)))
		{
			retval = MCI_FUNC_NOT_READY;
		}
		else if((CARDSTATEOF(respValue[0]) != MCI_CARDSTATE_TRAN))
		{
			retval = MCI_FUNC_ERR_STATE;
		}
		else
		{
			/* ready and in TRAN state */
			return MCI_FUNC_OK;
		}

		for ( i = 0; i < 0x20; i++ );

		retryCount--;
	}

	return retval;				/* Fatal error */
}

/******************************************************************************
** Function name:		MCI_Cmd_ReadSingleBlock
**
** Descriptions:		CMD17, CMD17_READ_SINGLE_BLOCK, send this cmd in the TRANS
**						state to read a block of data from the card.
**
** parameters:			block number
** Returned value:		Response value
**
******************************************************************************/
uint32_t MCI_Cmd_ReadSingleBlock( uint32_t blockNum )
{
	uint32_t i, retryCount;
	uint32_t respStatus;
	uint32_t respValue[4];

	uint32_t retval = MCI_FUNC_FAILED;

	retryCount = 0x20;
	while ( retryCount > 0 )
	{
		LPC_MCI->CLEAR = 0x7FF;

		respStatus = MCI_CmdResp(CMD17_READ_SINGLE_BLOCK, blockNum * BLOCK_LENGTH, EXPECT_SHORT_RESP, (uint32_t *)&respValue[0], 0);

		if(respStatus)
		{
			retval = MCI_FUNC_FAILED;
		}
		else if(!(respValue[0] & _SHIFT(MCI_CARDSTATUS_READYFORDATA_P0S)))
		{
			retval = MCI_FUNC_NOT_READY;
		}
		else if((CARDSTATEOF(respValue[0]) != MCI_CARDSTATE_TRAN))
		{
			retval = MCI_FUNC_ERR_STATE;
		}
		else
		{
			/* ready and in TRAN state */
			return MCI_FUNC_OK;
		}

		for ( i = 0; i < 0x20; i++ );

		retryCount--;
	}

	return retval;					/* Fatal error */
}


/******************************************************************************
** Function name:		MCI_WriteBlock
**
** Descriptions:		Set MCI data control register, data length and data
**						timeout, send WRITE_BLOCK cmd, finally, enable
**						interrupt. On completion of WRITE_BLOCK cmd, TX_ACTIVE
**						interrupt will occurs, data can be written continuously
**						into the FIFO until the block data length is reached.
**
** parameters:			block number
** Returned value:		true or false, if cmd times out, return false and no
**						need to continue.
**
******************************************************************************/
uint32_t MCI_WriteBlock(uint8_t* memblock, uint32_t blockNum )
{
	uint32_t i;
	uint32_t DataCtrl = 0;

#if MCI_DMA_ENABLED
	GPDMA_Channel_CFG_Type GPDMACfg;
#endif

	dataSrcBlock = memblock;

	LPC_MCI->CLEAR = 0x7FF;

	LPC_MCI->DATACTRL = 0;

	for ( i = 0; i < 0x10; i++ );

	/* Below status check is redundant, but ensure card is in TRANS state
	before writing and reading to from the card. */
	if (MCI_CheckStatus() != MCI_FUNC_OK)
	{
		MCI_Cmd_StopTransmission();

		return(MCI_FUNC_FAILED);
	}

	LPC_MCI->DATATMR = DATA_TIMER_VALUE;

	LPC_MCI->DATALEN = BLOCK_LENGTH;

	MCI_Block_End_Flag = 1;

	MCI_TXEnable();

	if (MCI_Cmd_WriteBlock( blockNum ) != MCI_FUNC_OK)
	{
		return ( MCI_FUNC_FAILED );
	}

#if MCI_DMA_ENABLED
	GPDMACfg.ChannelNum = MCI_DMA_WRITE_CHANNEL;
	// Source memory
	GPDMACfg.SrcMemAddr = (uint32_t)dataSrcBlock;
	// Destination memory
	GPDMACfg.DstMemAddr = 0;
	// Transfer size
	GPDMACfg.TransferSize = DMA_MCI_SIZE;
	// Transfer width
	GPDMACfg.TransferWidth = GPDMA_WIDTH_WORD;
	// Transfer type
	GPDMACfg.TransferType = GPDMA_TRANSFERTYPE_M2P;
	// Source connection - unused
	GPDMACfg.SrcConn = 0;
	// Destination connection - unused
	GPDMACfg.DstConn = GPDMA_CONN_MCI;
	// Linker List Item - unused
	GPDMACfg.DMALLI = 0;

	// Setup channel with given parameter
	GPDMA_Setup(&GPDMACfg);

	/* Reset terminal counter */
	dmaWrCh_TermianalCnt = 0;

	/* Reset Error counter */
	dmaWrCh_ErrorCnt = 0;

	// Enable GPDMA channel
	GPDMA_ChannelCmd(MCI_DMA_WRITE_CHANNEL, ENABLE);

	//LPC_GPDMACH0->CConfig |= 0xC001 | (0x00 << 1) | (0x01 << 6) | (0x05 << 11);

	/* Write, block transfer, DMA, and data length */
	DataCtrl |= ((1 << 0) | (1 << 3) | (DATA_BLOCK_LEN << 4));
#else
	/* Write, block transfer, and data length */
	DataCtrl |= ((1 << 0) | (DATA_BLOCK_LEN << 4));
#endif

	LPC_MCI->DATACTRL = DataCtrl;

	for ( i = 0; i < 0x10; i++ );

	return MCI_FUNC_OK;
}

/******************************************************************************
** Function name:		MCI_ReadBlock
**
** Descriptions:		Set MCI data control register, data length and data
**						timeout, send CMD17_READ_SINGLE_BLOCK cmd, finally, enable
**						interrupt. On completion of READ_SINGLE_BLOCK cmd,
**						RX_ACTIVE interrupt will occurs, data can be read
**						continuously into the FIFO until the block data
**						length is reached.
**
** parameters:			block number
** Returned value:		true or false, if cmd times out, return false and no
**						need to continue.
**
**
******************************************************************************/
uint32_t MCI_ReadBlock(uint8_t* destBlock, uint32_t blockNum )
{
	uint32_t i;
	uint32_t DataCtrl = 0;

#if MCI_DMA_ENABLED
	GPDMA_Channel_CFG_Type GPDMACfg;
#endif

	dataDestBlock = destBlock;

	LPC_MCI->CLEAR = 0x7FF;

	LPC_MCI->DATACTRL = 0;

	for ( i = 0; i < 0x10; i++ );

	/* Below status check is redundant, but ensure card is in TRANS state
	before writing and reading to from the card. */
	if ( MCI_CheckStatus() != MCI_FUNC_OK )
	{
		MCI_Cmd_StopTransmission();

		return(MCI_FUNC_FAILED);
	}

	MCI_RXEnable();

	LPC_MCI->DATATMR = DATA_TIMER_VALUE;

	LPC_MCI->DATALEN = BLOCK_LENGTH;

	MCI_Block_End_Flag = 1;

	if ( MCI_Cmd_ReadSingleBlock( blockNum ) != MCI_FUNC_OK )
	{
		return MCI_FUNC_FAILED;
	}

#if MCI_DMA_ENABLED
	GPDMACfg.ChannelNum = MCI_DMA_READ_CHANNEL;
	// Source memory
	GPDMACfg.SrcMemAddr = 0;
	// Destination memory
	GPDMACfg.DstMemAddr = (uint32_t)dataDestBlock;
	// Transfer size
	GPDMACfg.TransferSize = DMA_MCI_SIZE;
	// Transfer width
	GPDMACfg.TransferWidth = GPDMA_WIDTH_WORD;
	// Transfer type
	GPDMACfg.TransferType = GPDMA_TRANSFERTYPE_P2M;
	// Source connection - unused
	GPDMACfg.SrcConn = GPDMA_CONN_MCI;
	// Destination connection - unused
	GPDMACfg.DstConn = 0;
	// Linker List Item - unused
	GPDMACfg.DMALLI = 0;

	// Setup channel with given parameter
	GPDMA_Setup(&GPDMACfg);

	/* Reset terminal counter */
	dmaWrCh_TermianalCnt = 0;

	/* Reset Error counter */
	dmaWrCh_ErrorCnt = 0;

	// Enable GPDMA channel
	GPDMA_ChannelCmd(MCI_DMA_WRITE_CHANNEL, ENABLE);

	//LPC_GPDMACH1->CConfig |= 0xC001 | (0x01 << 1) | (0x00 << 6) | (0x06 << 11);

	/* Write, block transfer, DMA, and data length */
	DataCtrl |= ((1 << 0) | (1 << 1) | (1 << 3) | (DATA_BLOCK_LEN << 4));
#else
	/* Read, enable, block transfer, and data length */
	DataCtrl |= ((1 << 0) | (1 << 1) | (DATA_BLOCK_LEN << 4));
#endif

	LPC_MCI->DATACTRL = DataCtrl;

	for ( i = 0; i < 0x10; i++ );

	return MCI_FUNC_OK;
}

/*****************************************************************************
**                            End Of File
******************************************************************************/
