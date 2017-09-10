/*
    FreeRTOS V7.1.0 - Copyright (C) 2011 Real Time Engineers Ltd.
	

    ***************************************************************************
     *                                                                       *
     *    FreeRTOS tutorial books are available in pdf and paperback.        *
     *    Complete, revised, and edited pdf reference manuals are also       *
     *    available.                                                         *
     *                                                                       *
     *    Purchasing FreeRTOS documentation will not only help you, by       *
     *    ensuring you get running as quickly as possible and with an        *
     *    in-depth knowledge of how to use FreeRTOS, it will also help       *
     *    the FreeRTOS project to continue with its mission of providing     *
     *    professional grade, cross platform, de facto standard solutions    *
     *    for microcontrollers - completely free of charge!                  *
     *                                                                       *
     *    >>> See http://www.FreeRTOS.org/Documentation for details. <<<     *
     *                                                                       *
     *    Thank you for using FreeRTOS, and thank you for your support!      *
     *                                                                       *
    ***************************************************************************


    This file is part of the FreeRTOS distribution.

    FreeRTOS is free software; you can redistribute it and/or modify it under
    the terms of the GNU General Public License (version 2) as published by the
    Free Software Foundation AND MODIFIED BY the FreeRTOS exception.
    >>>NOTE<<< The modification to the GPL is included to allow you to
    distribute a combined work that includes FreeRTOS without being obliged to
    provide the source code for proprietary components outside of the FreeRTOS
    kernel.  FreeRTOS is distributed in the hope that it will be useful, but
    WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY
    or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public License for
    more details. You should have received a copy of the GNU General Public
    License and the FreeRTOS license exception along with FreeRTOS; if not it
    can be viewed here: http://www.freertos.org/a00114.html and also obtained
    by writing to Richard Barry, contact details for whom are available on the
    FreeRTOS WEB site.

    1 tab == 4 spaces!

    http://www.FreeRTOS.org - Documentation, latest information, license and
    contact details.

    http://www.SafeRTOS.com - A version that is certified for use in safety
    critical systems.

    http://www.OpenRTOS.com - Commercial support, development, porting,
    licensing and training services.
*/

/* Standard includes. */
#include "string.h"

/* FreeRTOS includes. */
#include "FreeRTOS.h"
#include "task.h"
#include "queue.h"

/* lwIP includes. */
#include "lwip/opt.h"
#include "lwip/def.h"
#include "lwip/mem.h"
#include "lwip/pbuf.h"
#include "lwip/sys.h"
#include <lwip/stats.h>
#include <lwip/snmp.h>
#include "netif/etharp.h"

/* Driver includes. */
#include "lpc17xx_emac.h"
//#include "lpc177x_8x_emac.h"
#include "lpc177x_8x_pinsel.h"
#include "lpc177x_8x_gpio.h"

/* Define those to better describe your network interface. */
#define IFNAME0 'e'
#define IFNAME1 'l'

/* When a packet is ready to be sent, if it cannot be sent immediately then
 * the task performing the transmit will block for netifTX_BUFFER_FREE_WAIT
 * milliseconds.  It will do this a maximum of netifMAX_TX_ATTEMPTS before
 * giving up.
 */
#define netifTX_BUFFER_FREE_WAIT	( ( portTickType ) 2UL / portTICK_RATE_MS )
#define netifMAX_TX_ATTEMPTS		( 5 )

#define netifMAX_MTU 1500

struct xEthernetIf
{
	struct eth_addr *ethaddr;
	/* Add whatever per-interface state that is needed here. */
};

/*
 * Copy the received data into a pbuf.
 */
static struct pbuf *prvLowLevelInput( void );

/*
 * Send data from a pbuf to the hardware.
 */
static err_t prvLowLevelOutput( struct netif *pxNetIf, struct pbuf *p );

/*
 * Perform any hardware and/or driver initialisation necessary.
 */
static void prvLowLevelInit( struct netif *pxNetIf );

/*
 * CMSIS name for the MAC interrupt handler.
 */
void ENET_IRQHandler( void );

/*
 * Keeps a file scope copy of the netif structure in use, so it can be accessed
 * from the ISR.
 */
static struct netif *pxNetIfInUse = NULL;

/*-----------------------------------------------------------*/

/**
 * In this function, the hardware should be initialized.
 * Called from ethernetif_init().
 *
 * @param pxNetIf the already initialized lwip network interface structure
 *		for this etherpxNetIf
 */
static void prvLowLevelInit( struct netif *pxNetIf )
{
portBASE_TYPE xStatus;
EMAC_CFG_Type Emac_Config;

	/* set MAC hardware address length */
	pxNetIf->hwaddr_len = ETHARP_HWADDR_LEN;

	/* device capabilities */
	pxNetIf->flags = NETIF_FLAG_BROADCAST | NETIF_FLAG_ETHARP | NETIF_FLAG_LINK_UP;

	/* maximum transfer unit */
	pxNetIf->mtu = netifMAX_MTU;

	/* Broadcast capability */
	pxNetIf->flags = NETIF_FLAG_BROADCAST | NETIF_FLAG_ETHARP | NETIF_FLAG_LINK_UP;

	/* Enable Ethernet Pins */
	/*
	 * Enable P1 Ethernet Pins:
	 * P1.0 - ENET_TXD0
	 * P1.1 - ENET_TXD1
	 * P1.4 - ENET_TX_EN
	 * P1.8 - ENET_CRS
	 * P1.9 - ENET_RXD0
	 * P1.10 - ENET_RXD1
	 * P1.14 - ENET_RX_ER
	 * P1.15 - ENET_REF_CLK
	 * P1.16 - ENET_MDC
	 * P1.17 - ENET_MDIO
	 */
	/* on rev. 'A' and later, P1.6 should NOT be set. */
	PINSEL_ConfigPin(1,0,1);
	PINSEL_ConfigPin(1,1,1);
	PINSEL_ConfigPin(1,4,1);
	PINSEL_ConfigPin(1,8,1);
	PINSEL_ConfigPin(1,9,1);
	PINSEL_ConfigPin(1,10,1);
	PINSEL_ConfigPin(1,14,1);
	PINSEL_ConfigPin(1,15,1);
	PINSEL_ConfigPin(1,16,1);
	PINSEL_ConfigPin(1,17,1);

	Emac_Config.Mode = EMAC_MODE_AUTO;
	Emac_Config.pbEMAC_Addr = pxNetIf->hwaddr;
	xStatus = EMAC_Init( &Emac_Config );

	if( xStatus != ERROR )
	{
		/* Enable the interrupt and set its priority to the minimum
		interrupt priority.  */
		pxNetIfInUse = pxNetIf;
		NVIC_SetPriority( ENET_IRQn, configEMAC_INTERRUPT_PRIORITY );
		NVIC_EnableIRQ( ENET_IRQn );
	}

	configASSERT( xStatus != ERROR );
}

/**
 * This function should do the actual transmission of the packet. The packet is
 * contained in the pbuf that is passed to the function. This pbuf
 * might be chained.
 *
 * @param pxNetIf the lwip network interface structure for this etherpxNetIf
 * @param p the MAC packet to send (e.g. IP packet including MAC addresses and type)
 * @return ERR_OK if the packet could be sent
 *		 an err_t value if the packet couldn't be sent
 *
 * @note Returning ERR_MEM here if a DMA queue of your MAC is full can lead to
 *	   strange results. You might consider waiting for space in the DMA queue
 *	   to become available since the stack doesn't retry to send a packet
 *	   dropped because of memory failure (except for the TCP timers).
 */
static err_t prvLowLevelOutput( struct netif *pxNetIf, struct pbuf *p )
{

	/* This is taken from lwIP example code and therefore does not conform
	to the FreeRTOS coding standard. */

struct pbuf *q;
uint8_t *pucChar;
struct eth_hdr *pxHeader;
err_t xReturn = ERR_BUF;
int32_t x;
extern uint8_t *EMAC_NextPacketToSend( void );
void EMAC_StartTransmitNextBuffer( uint32_t ulLength );

	( void ) pxNetIf;

	/* Attempt to obtain access to a Tx buffer. */
	for( x = 0; x < netifMAX_TX_ATTEMPTS; x++ )
	{
		if( EMAC_CheckTransmitIndex() == TRUE )
		{
			/* Will the data fit in the Tx buffer? */
			if( p->tot_len < EMAC_ETH_MAX_FLEN )
			{
				/* Get a pointer to the Tx buffer that is now known to be free. */
				pucChar = EMAC_NextPacketToSend();

				/* Copy the data into the Tx buffer. */
				for( q = p; q != NULL; q = q->next )
				{
					/* Send the data from the pbuf to the interface, one pbuf at a
					time. The size of the data in each pbuf is kept in the ->len
					variable. */
					/* send data from(q->payload, q->len); */
					LWIP_DEBUGF( NETIF_DEBUG, ( "NETIF: send pucChar %p q->payload %p q->len %i q->next %p\n", pucChar, q->payload, ( int ) q->len, ( void* ) q->next ) );
					if( q == p )
					{
						memcpy( pucChar, &( ( char * ) q->payload )[ ETH_PAD_SIZE ], q->len - ETH_PAD_SIZE );
						pucChar += q->len - ETH_PAD_SIZE;
					}
					else
					{
						memcpy( pucChar, q->payload, q->len );
						pucChar += q->len;
					}
				}

				/* Initiate the Tx. */
				EMAC_StartTransmitNextBuffer( p->tot_len - ETH_PAD_SIZE );

				LINK_STATS_INC( link.xmit );
				snmp_add_ifoutoctets( pxNetIf, usTotalLength );
				pxHeader = ( struct eth_hdr * )p->payload;

				if( ( pxHeader->dest.addr[ 0 ] & 1 ) != 0 )
				{
					/* broadcast or multicast packet*/
					snmp_inc_ifoutnucastpkts( pxNetIf );
				}
				else
				{
					/* unicast packet */
					snmp_inc_ifoutucastpkts( pxNetIf );
				}

				/* The Tx has been initiated. */
				xReturn = ERR_OK;
			}
			else
			{
				configASSERT( ( volatile void * ) 0 );
			}

			break;
		}
		else
		{
			vTaskDelay( netifTX_BUFFER_FREE_WAIT );
		}
	}

	configASSERT( xReturn == ERR_OK );
	if( xReturn != ERR_OK )
	{
		LINK_STATS_INC( link.memerr );
		LINK_STATS_INC( link.drop );
		snmp_inc_ifoutdiscards( pxNetIf );
	}

	return xReturn;
}

/**
 * Should allocate a pbuf and transfer the bytes of the incoming
 * packet from the interface into the pbuf.
 *
 * @param pxNetIf the lwip network interface structure for this etherpxNetIf
 * @return a pbuf filled with the received packet (including MAC header)
 *		 NULL on memory error
 */
static struct pbuf *prvLowLevelInput( void )
{
struct pbuf *p = NULL, *q;
const uint32_t ulCRCLength = 4UL;
uint32_t ulDataLength;
extern uint8_t *EMAC_NextPacketToRead( void );
uint8_t *pucReceivedData;

	/* Obtain the length, minus the CRC.  The CRC is four bytes
	but the length is already minus 1. */
	ulDataLength = EMAC_GetReceiveDataSize() - ( ulCRCLength - 1UL );

	if( ulDataLength > 0U )
	{
		#if ETH_PAD_SIZE
			len += ETH_PAD_SIZE; /* allow room for Ethernet padding */
		#endif

		/* Allocate a pbuf chain of pbufs from the pool. */
		p = pbuf_alloc( PBUF_RAW, ulDataLength, PBUF_POOL );

		if( p != NULL ) 
		{
			#if ETH_PAD_SIZE
				pbuf_header( p, -ETH_PAD_SIZE ); /* drop the padding word */
			#endif

			/* Iterate over the pbuf chain until we have read the entire
			packet into the pbuf. */
			ulDataLength = 0UL;
			pucReceivedData = EMAC_NextPacketToRead();
			for( q = p; q != NULL; q = q->next ) 
			{
				/* Read enough bytes to fill this pbuf in the chain. The
				available data in the pbuf is given by the q->len variable.
				This does not necessarily have to be a memcpy, you can also preallocate
				pbufs for a DMA-enabled MAC and after receiving truncate it to the
				actually received size. In this case, ensure the usTotalLength member of the
				pbuf is the sum of the chained pbuf len members. */
				memcpy( q->payload, &( pucReceivedData[ ulDataLength ] ), q->len );
				ulDataLength += q->len;
			}

			#if ETH_PAD_SIZE
				pbuf_header( p, ETH_PAD_SIZE ); /* reclaim the padding word */
			#endif

			LINK_STATS_INC( link.recv );
		}
	}

	return p;  
}

/**
 * Should be called at the beginning of the program to set up the
 * network interface. It calls the function prvLowLevelInit() to do the
 * actual setup of the hardware.
 *
 * This function should be passed as a parameter to pxNetIf_add().
 *
 * @param pxNetIf the lwip network interface structure for this etherpxNetIf
 * @return ERR_OK if the loopif is initialized
 *		 ERR_MEM if private data couldn't be allocated
 *		 any other err_t on error
 */
err_t ethernetif_init( struct netif *pxNetIf )
{
err_t xReturn = ERR_OK;

	/* This is taken from lwIP example code and therefore does not conform
	to the FreeRTOS coding standard. */
	
struct xEthernetIf *pxEthernetIf;

	LWIP_ASSERT( "pxNetIf != NULL", ( pxNetIf != NULL ) );
	
	pxEthernetIf = mem_malloc( sizeof( struct xEthernetIf ) );
	if( pxEthernetIf == NULL ) 
	{
		LWIP_DEBUGF(NETIF_DEBUG, ( "ethernetif_init: out of memory\n" ) );
		xReturn = ERR_MEM;
	}
	else
	{
		#if LWIP_NETIF_HOSTNAME
		{
			/* Initialize interface hostname */
			pxNetIf->hostname = "lwip";
		}
		#endif /* LWIP_NETIF_HOSTNAME */

		pxNetIf->state = pxEthernetIf;
		pxNetIf->name[ 0 ] = IFNAME0;
		pxNetIf->name[ 1 ] = IFNAME1;

		/* We directly use etharp_output() here to save a function call.
		* You can instead declare your own function an call etharp_output()
		* from it if you have to do some checks before sending (e.g. if link
		* is available...) */
		pxNetIf->output = etharp_output;
		pxNetIf->flags = NETIF_FLAG_BROADCAST | NETIF_FLAG_ETHARP | NETIF_FLAG_IGMP;
		pxNetIf->hwaddr_len = ETHARP_HWADDR_LEN;
		pxNetIf->mtu = netifMAX_MTU;
		pxNetIf->linkoutput = prvLowLevelOutput;

		pxEthernetIf->ethaddr = ( struct eth_addr * ) &( pxNetIf->hwaddr[ 0 ] );

		/* initialize the hardware */
		prvLowLevelInit( pxNetIf );
	}

	return xReturn;
}
/*-----------------------------------------------------------*/

void ENET_IRQHandler( void )
{
uint32_t ulInterruptCause;
extern volatile portBASE_TYPE xInsideISR;
struct pbuf *p;
struct eth_hdr *pxHeader;

	xInsideISR++;

	while( ( ulInterruptCause = LPC_EMAC->IntStatus ) != 0 )
	{
		/* Clear the interrupt. */
		LPC_EMAC->IntClear = ulInterruptCause;

		/* Clear fatal error conditions.  */
		if( ( ulInterruptCause & EMAC_INT_TX_UNDERRUN ) != 0U )
		{
			LPC_EMAC->Command |= EMAC_CR_TX_RES;
		}

		if( ( ulInterruptCause & EMAC_INT_RX_DONE ) != 0UL )
		{
			/* A packet has been received. */
			if( EMAC_CheckReceiveIndex() != FALSE )
			{
				if( EMAC_CheckReceiveDataStatus( EMAC_RINFO_LAST_FLAG ) == SET )
				{
					/* move received packet into a new pbuf */
					p = prvLowLevelInput();

					/* no packet could be read, silently ignore this */
					if( p != NULL )
					{
						/* points to packet payload, which starts with an Ethernet header */
						pxHeader = p->payload;

						switch( htons( pxHeader->type ) )
						{
							/* IP or ARP packet? */
							case ETHTYPE_IP:
							case ETHTYPE_ARP:
												/* Full packet send to tcpip_thread to process */
												configASSERT( pxNetIfInUse );
												if( pxNetIfInUse->input( p, pxNetIfInUse ) != ERR_OK )
												{
													LWIP_DEBUGF(NETIF_DEBUG, ( "ethernetif_input: IP input error\n" ) );
													pbuf_free(p);
													p = NULL;
												}
												break;

							default:
												pbuf_free( p );
												p = NULL;
							break;
						}
					}
					else
					{
						configASSERT( ( volatile void * ) NULL );
					}
				}

				/* Release the frame. */
				EMAC_UpdateRxConsumeIndex();
			}
		}

		if( ( ulInterruptCause & EMAC_INT_TX_DONE ) != 0UL )
		{
			/* Nothing to do here. */
		}
	}

	xInsideISR--;
}
/**
 * Return address of Ethernet transmit buffer
 * @return pointer to the beginning of buffer
 */
void *pvApplicationGetEMACTxBufferAddress(void)
{
	static union su_buff {
		xEMACTxBuffer_t buf;
		uint64_t align;
	} TxBuf;

	return (void *)TxBuf.buf;
}
/*-----------------------------------------------------------*/

/**
 * Return address of Ethernet receive buffer
 * @return pointer to the beginning of buffer
 */
void *pvApplicationGetEMACRxBufferAddress(void)
{
	static union su_buff {
		xEMACRxBuffer_t buf;
		uint64_t align;
	} RxBuf;

	return (void *)RxBuf.buf;
}
/*-----------------------------------------------------------*/

