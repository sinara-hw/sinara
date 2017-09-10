/* Standard includes. */
#include <string.h>

#include "lpc177x_8x_uart.h"
#include "lpc177x_8x_pinsel.h"
#include "lpc177x_8x_emac.h"
#include "lpc177x_8x_i2c.h"
#include "lpc177x_8x_gpio.h"

/* FreeRTOS includes. */
#include "FreeRTOS.h"
#include "task.h"
#include "semphr.h"

/* lwIP core includes */
#include "lwip/opt.h"
#include "lwip/tcpip.h"

/* lwIP netif includes */
#include "netif/etharp.h"
#include "lwip/netif.h"
#include "lwip/dhcp.h"
#include "lwip/inet.h"
#include "lwip/dns.h"

#include "i2c.h"

#define DEBUGSTR(...) taskENTER_CRITICAL(); \
					  printf(__VA_ARGS__); \
					  taskEXIT_CRITICAL()

extern err_t ethernetif_init(struct netif *xNetIf);
extern int32_t EMAC_CheckPHYStatus(uint32_t ulPHYState);
extern void http_server_netconn_init(void);

static struct netif net_iface;

static void iface_callback(struct netif *iface)
{
	char ip[16] = {0}, gw[16] = {0}, mask[16] = {0};
	strcpy(ip, inet_ntoa(iface->ip_addr));
	strcpy(gw, inet_ntoa(iface->gw));
	strcpy(mask, inet_ntoa(iface->netmask));

	printf("Network interface is up, ip = %s, gw = %s, mask = %s\n", ip, gw, mask);
}

void delay_ms(unsigned int ms)
{
    unsigned int i,j;

    for(i=0;i<ms;i++)
         for(j=0;j<20000;j++);
}


void psu_init (void) {
	//power supplies
				LPC_GPIO1->DIR |= (1 << 26);//EN RTM 12V
				LPC_GPIO1->DIR |= (1 << 28);//EN VCCINT
				LPC_GPIO1->DIR |= (1 << 29);//EN P5V0
				LPC_GPIO1->DIR |= (1 << 30);//EN RTM MP
				LPC_GPIO1->DIR |= (1 << 31);//EN PSU CH


				LPC_GPIO4->DIR |= (1 << 18);// scansta  reset
				LPC_GPIO4->DIR |= (1 << 25);//lpsel0 PROG master
				LPC_GPIO4->DIR |= (1 << 26);//lpsel1 MMC JTAG
				LPC_GPIO4->DIR |= (1 << 27);//lpsel2 FPGA JTAG
				LPC_GPIO4->DIR |= (1 << 28);//lpsel3 FMC1JTAG
				LPC_GPIO4->DIR |= (1 << 29);//lpsel4 FMC2 JTAG
			  	LPC_GPIO4->DIR |= (1 << 30);//lpsel5 RTM JTAG
			  	LPC_GPIO4->DIR |= (1 << 31);//lpsel6 PS JTAG
			  	LPC_GPIO4->DIR |= (1 << 19);//stitcher mode
			  	LPC_GPIO0->DIR &= ~(1 << 29);     // Make  the Port pin input - RTM present
			  	LPC_GPIO0->DIR &= ~(1 << 5);     // Make  the Port pins inputs - DONE pin



				LPC_GPIO1->SET |= (1 << 28); //EN VCCINT
				LPC_GPIO1->SET |= (1 << 29); //EN P5V0
				LPC_GPIO1->SET |= (1 << 31); //EN PSU CH
				delay_ms(200); //wait for power supplies to boot


				//LEDs
				LPC_GPIO3->DIR |= (1 << 5);
				LPC_GPIO3->DIR |= (1 << 4);

				//SCANSTA config
			  	LPC_GPIO4->CLR |= (1 << 18); //keep scansta in reset
			  	delay_ms(10);
			  	LPC_GPIO4->CLR |= (1 << 25);//lpsel0 keep unseleted - inactive master port becomes LSP0
			  	LPC_GPIO4->CLR |= (1 << 26);//lpsel1 MMC JTAG
			  	LPC_GPIO4->SET |= (1 << 27);//lpsel2 FPGA JTAG
			  	LPC_GPIO4->CLR |= (1 << 28);//lpsel3 FMC1 JTAG
			  	LPC_GPIO4->CLR |= (1 << 29);//lpsel4 FMC2 JTAG
			//  	LPC_GPIO4->CLR |= (1 << 30);//lpsel5 RTM JTAG
			  	LPC_GPIO4->CLR |= (1 << 31);//lpsel6 PS JTAG
			  	LPC_GPIO4->CLR |= (1 << 19);//stitcher mode
	            //check if RTM is connected  RTM_PSn is low
		            if (LPC_GPIO0->PIN &(1 << 29))
						{
		            	LPC_GPIO4->CLR |= (1 << 30);//lpsel5 RTM JTAG
						}
		            else
		            {
		            	LPC_GPIO4->SET |= (1 << 30);//lpsel5 RTM JTAG
		            }


			 	delay_ms(10);
			  	LPC_GPIO4->SET |= (1 << 18); //initialize SCANSTA




				//I2C switch ADDR1=0
				LPC_GPIO2->DIR |= (1 << 29);
			  	LPC_GPIO2->CLR |= (1 << 29);

			  	//I2C switch ADDR2=0
				LPC_GPIO2->DIR |= (1 << 28);
			  	LPC_GPIO2->CLR |= (1 << 28);
				// i2c mux 0x70 address


				LPC_GPIO1->CLR |= (1 << 30); //RTM 3.3MP power off
		   		LPC_GPIO1->CLR |= (1 << 26);// 12V power off
		        LPC_GPIO1->DIR |= (1 << 26); //RTM 12V PWR pin
		        LPC_GPIO1->DIR |= (1 << 30); //RTM 3.3MP pin


		//// init ETH PHY
		//		//RESET PHY
		//				LPC_GPIO0->DIR |= (1 << 23);
		//			  	LPC_GPIO0->CLR |= (1 << 23);
		//		//SET RGMII = 0 -> MII mode
		//			  	LPC_GPIO2->DIR |= (1 << 2);
		//			  	LPC_GPIO2->CLR |= (1 << 2);
		//		//PHY SPD 0 = 1
		//				LPC_GPIO0->DIR |= (1 << 25);
		//				LPC_GPIO0->SET |= (1 << 25);
		//		//PHY SPD 1 = 0
		//				LPC_GPIO0->DIR |= (1 << 24);
		//				LPC_GPIO0->CLR |= (1 << 24);
		//		//CFG DDR = 0
		//				LPC_GPIO0->DIR |= (1 << 26);
		//				LPC_GPIO0->CLR |= (1 << 26);
		//		//un-RESET PHY
		//				LPC_GPIO0->DIR |= (1 << 23);
		//				LPC_GPIO0->SET |= (1 << 23);
		//		// MII LED ON
		//				LPC_GPIO0->DIR |= (1 << 31);
		//				LPC_GPIO0->SET |= (1 << 31);
		//


						//RESET PHY
								LPC_GPIO0->DIR |= (1 << 23);
							  	LPC_GPIO0->CLR |= (1 << 23);
						//SET RGMII = 1 -> RGMII mode
							  	LPC_GPIO2->DIR |= (1 << 2);
							  	LPC_GPIO2->SET |= (1 << 2);
						//PHY SPD 0 = 0
								LPC_GPIO0->DIR |= (1 << 25);
								LPC_GPIO0->CLR |= (1 << 25);
						//PHY SPD 1 = 1
								LPC_GPIO0->DIR |= (1 << 24);
								LPC_GPIO0->SET |= (1 << 24);
						//CFG DDR = 1
								LPC_GPIO0->DIR |= (1 << 26);
								LPC_GPIO0->SET |= (1 << 26);
						//un-RESET PHY
								LPC_GPIO0->DIR |= (1 << 23);
								LPC_GPIO0->SET |= (1 << 23);
						// MII LED ON
								LPC_GPIO0->DIR |= (1 << 31);
								LPC_GPIO0->CLR |= (1 << 31);

}


/* Called from the TCP/IP thread */
void network_init(void *arg)
{
	ip_addr_t ipaddr, netmask, gw;

	/* Tell main thread TCP/IP init is done */
	*(s32_t *) arg = 1;

#if LWIP_DHCP
	IP4_ADDR(&gw, 0, 0, 0, 0);
	IP4_ADDR(&ipaddr, 0, 0, 0, 0);
	IP4_ADDR(&netmask, 0, 0, 0, 0);
#else
	IP4_ADDR(&gw, 10, 1, 10, 1);
	IP4_ADDR(&ipaddr, 10, 1, 10, 234);
	IP4_ADDR(&netmask, 255, 255, 255, 0);
#endif

	netif_set_default(netif_add(&net_iface, &ipaddr, &netmask, &gw, NULL, ethernetif_init, tcpip_input));
	netif_set_status_callback(&net_iface, iface_callback);

#if LWIP_DHCP
	dhcp_start(&net_iface);
#else
	netif_set_up(&net_iface);
#endif
}

/* LWIP kickoff and PHY link monitor thread */
static void vSetupIFTask(void *pvParameters) {
	volatile s32_t tcpipdone = 0;
	int32_t physts, phystsprev;

	DEBUGSTR("LWIP HTTP Web Server FreeRTOS Demo...\r\n");

	/* Wait until the TCP/IP thread is finished before
	   continuing or wierd things may happen */
	DEBUGSTR("Waiting for TCPIP thread to initialize...\r\n");
	tcpip_init(network_init, (void *) &tcpipdone);

	while (!tcpipdone)
	{
		vTaskDelay(configTICK_RATE_HZ / 1000);
	}

	DEBUGSTR("Starting LWIP HTTP server...\r\n");

	/* Initialize and start application */
	http_server_netconn_init();

	/* This loop monitors the PHY link and will handle cable events
	   via the PHY driver. */
	phystsprev = 0;
	while (1)
	{
		physts = EMAC_CheckPHYStatus(EMAC_PHY_STAT_LINK);

		if (physts != phystsprev)
		{
			phystsprev = physts;
			if (physts == 1)
			{
				printf("Net up\n");
				tcpip_callback_with_block((tcpip_callback_fn) netif_set_link_up,
														  (void *) &net_iface, 1);
			} else {
				printf("Net down\n");
				tcpip_callback_with_block((tcpip_callback_fn) netif_set_link_down,
														  (void *) &net_iface, 1);
			}
		}

		vTaskDelay(configTICK_RATE_HZ / 4);
	}
}


static void vblinkIFTask(void *pvParameters)
{
   while(1)
	{
	   	   	   	   	   LPC_GPIO3->CLR |= (1 << 5);    // Make  the Port pins as high
	   		        	LPC_GPIO3->SET |= (1 << 4);
	   		        	vTaskDelay(configTICK_RATE_HZ / 5);

	   		            LPC_GPIO3->SET |= (1 << 5);     // Make  the Port pins as low
	   		            LPC_GPIO3->CLR |= (1 << 4);
	   		            vTaskDelay(configTICK_RATE_HZ / 5);
	   		           // printf("test \n");

	   		            //check if FPGA is programmed
	   		            //DONE line

	   		            if (LPC_GPIO0->PIN &(1 << 5))
	   						{
	   						//un-RESET PHY
	   						LPC_GPIO0->DIR |= (1 << 23);
	   						LPC_GPIO0->SET |= (1 << 23);
	   						// MII LED ON
	   						LPC_GPIO0->DIR |= (1 << 31);
	   						LPC_GPIO0->SET |= (1 << 31);
	   						}
	   		            else
	   		            {

	   		            	//RESET PHY
	   						LPC_GPIO0->DIR |= (1 << 23);
	   						LPC_GPIO0->CLR |= (1 << 23);
	   						// MII LED OFF
	   						LPC_GPIO0->DIR |= (1 << 31);
	   						LPC_GPIO0->CLR |= (1 << 31);
	   		            }





	   		            //check if RTM is connected
		   		            //RTM_PSn is low

		   		            if (LPC_GPIO0->PIN &(1 << 29))
		   						{
		   						//3.3MP power off
		   						LPC_GPIO1->CLR |= (1 << 30);
		   						// 12V power off
		   						LPC_GPIO1->CLR |= (1 << 26);

		   						}
		   		            else
		   		            {
		   						//3.3MP power on
		   						LPC_GPIO1->SET |= (1 << 30);
		   						// 12V power 0n
		   						LPC_GPIO1->SET |= (1 << 26);

		   		            }
	}
   //vTaskDelay(configTICK_RATE_HZ / 4);
}

static void prvSetupHardware(void)
{
	UART_CFG_Type UARTConfigStruct;

	SystemCoreClockUpdate();
	PINSEL_ConfigPin (0, 2, 1);
	PINSEL_ConfigPin (0, 3, 1);
	UART_ConfigStructInit(&UARTConfigStruct);
	// Re-configure baudrate to 115200bps
	UARTConfigStruct.Baud_rate = 115200u;

	LPC_GPIO2->DIR |= (1 << 26);

	// Initialize DEBUG_UART_PORT peripheral with given to corresponding parameter
	UART_Init(LPC_UART0, &UARTConfigStruct);

	// Enable UART Transmit
	UART_TxCmd(LPC_UART0, ENABLE);
}

/**
 * Program entry point 
 */
int main(void)
{
	prvSetupHardware();


	psu_init();
	delay_ms(200);
	i2c_init();
	//i2c_probe_slaves2(); // probe i2cmux slaves at every channel

	//write SI5324 register values
	i2c_setSI5324();


       //check if RTM is connected
           //RTM_PSn is low

           if (LPC_GPIO0->PIN &(1 << 29))
				{
				//3.3MP power off
				LPC_GPIO1->CLR |= (1 << 30);
				// 12V power off
				LPC_GPIO1->CLR |= (1 << 26);

				}
           else
           {
				//3.3MP power on
				LPC_GPIO1->SET |= (1 << 30);
				// 12V power 0n
				LPC_GPIO1->SET |= (1 << 26);
				delay_ms(200);
				// enable DC/DC
				i2c_RTM_PWRON();
			    delay_ms(1000);
				i2c_setSI5324_RTM();

           }



	/* First record in log also initialize it */
 	printf("Started. CPU clock = %lu MHz, SDRAM clock = %lu Mhz\n", SystemCoreClock / 1000000u, EMCClock / 1000000u);

	/* Add another thread for initializing physical interface. This
	   is delayed from the main LWIP initialization. */
	xTaskCreate(vSetupIFTask, (signed char *) "SetupIFx", configMINIMAL_STACK_SIZE, NULL, (tskIDLE_PRIORITY + 2UL), (xTaskHandle *) NULL);
	//blink LED task
	xTaskCreate(vblinkIFTask, (signed char *) "BlinkIFx", configMINIMAL_STACK_SIZE, NULL, (tskIDLE_PRIORITY + 1UL), (xTaskHandle *) NULL);




	vTaskStartScheduler();
	return 1;
}

void vApplicationStackOverflowHook( xTaskHandle pxTask, signed char *pcTaskName )
{
	/* Bad thing =( Print some useful info and reboot */
	printf("Stack overflow in %s\n", pcTaskName);
	for (;;){}
}

void vApplicationIdleHook(void)
{
}


void vApplicationTickHook(void)
{
}

void HardFault_Handler(void)
{
	/* Very bad thing =( Print some useful info and reboot */
	printf("HardFault CFSR = %X  BFAR = %X\n", SCB->CFSR, SCB->BFAR);
	for (;;){}
}

void NMI_Handler(void)
{
	printf("NMI\n");
	for (;;){}
}

void MemManage_Handler(void)
{
	printf("MemManage\n");
	for (;;){}
}

void BusFault_Handler(void)
{
	printf("BusFault\n");
	for (;;){}
}

void UsageFault_Handler(void)
{
	printf("UsageFault\n");
	for (;;){}
}


void DebugMon_Handler(void)
{
	printf("DebugMon\n");
	for (;;){}
}

