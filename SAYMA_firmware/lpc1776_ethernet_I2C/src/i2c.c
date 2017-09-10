/*
 * i2c.c
 *
 *  Created on: Jun 11, 2017
 *      Author: wizath
 */

#include "i2c.h"

SI324Info InitTable[] = {
		{  0, 0x34},	/* Register 0 */
		{  1, 0xE4},	/* Register 1 */
		{  2, 0xA2},	/* Register 2 */
		{  3, 0x55},	/* Register 3 */
		{  4, 0x92},	/* Register 4 */
		{  5, 0xAD},	/* Register 5 */
		{  6, 0x3F},	/* Register 6 */
		{  7, 0x2A},	/* Register 7 */
		{  8, 0x00},	/* Register 8 */
		{  9, 0xC0},	/* Register 9 */
		{ 10, 0x00},	/* Register 10 */
		{ 11, 0x40},	/* Register 11 */
		{ 19, 0x29},	/* Register 19 */
		{ 20, 0x3E},	/* Register 20 */
		{ 21, 0xFE},	/* Register 21 */
		{ 22, 0xDF},	/* Register 22 */
		{ 23, 0x1F},	/* Register 23 */
		{ 24, 0x3F},	/* Register 24 */
		{ 25, 0x60},	/* Register 25 */
		{ 31, 0x00},	/* Register 31 */
		{ 32, 0x00},	/* Register 32 */
		{ 33, 0x07},	/* Register 33 */
		{ 34, 0x00},	/* Register 34 */
		{ 35, 0x00},	/* Register 35 */
		{ 36, 0x07},	/* Register 36 */
		{ 40, 0x80},	/* Register 40 */
		{ 41, 0x01},	/* Register 41 */
		{ 42, 0x6B},	/* Register 42 */
		{ 43, 0x00},	/* Register 43 */
		{ 44, 0x00},	/* Register 44 */
		{ 45, 0x0C},	/* Register 45 */
		{ 46, 0x00},	/* Register 46 */
		{ 47, 0x00},	/* Register 47 */
		{ 48, 0x0C},	/* Register 48 */
		{ 55, 0x00},	/* Register 55 */
		{131, 0x1F},	/* Register 131 */
		{132, 0x02},	/* Register 132 */
		{137, 0x01},	/* Register 137 */
		{138, 0x0F},	/* Register 138 */
		{139, 0xFF},	/* Register 139 */
		{142, 0x00},	/* Register 142 */
		{143, 0x00},	/* Register 143 */
		{136, 0x40}		/* Register 136 */





//		100MHz free run
//				{  0, 0x74},	/* Register 0 */
//				{  1, 0xE1},	/* Register 1 */
//				{  2, 0x32},	/* Register 2 */
//				{  3, 0x55},	/* Register 3 */
//				{  4, 0x92},	/* Register 4 */
//				{  5, 0xAD},	/* Register 5 */
//				{  6, 0x3F},	/* Register 6 */
//				{  7, 0x2A},	/* Register 7 */
//				{  8, 0x00},	/* Register 8 */
//				{  9, 0xC0},	/* Register 9 */
//				{ 10, 0x00},	/* Register 10 */
//				{ 11, 0x40},	/* Register 11 */
//				{ 19, 0x29},	/* Register 19 */
//				{ 20, 0x3E},	/* Register 20 */
//				{ 21, 0xFE},	/* Register 21 */
//				{ 22, 0xDF},	/* Register 22 */
//				{ 23, 0x1F},	/* Register 23 */
//				{ 24, 0x3F},	/* Register 24 */
//				{ 25, 0xA0},	/* Register 25 */
//				{ 31, 0x00},	/* Register 31 */
//				{ 32, 0x00},	/* Register 32 */
//				{ 33, 0x05},	/* Register 33 */
//				{ 34, 0x00},	/* Register 34 */
//				{ 35, 0x00},	/* Register 35 */
//				{ 36, 0x05},	/* Register 36 */
//				{ 40, 0x80},	/* Register 40 */
//				{ 41, 0xA3},	/* Register 41 */
//				{ 42, 0xE5},	/* Register 42 */
//				{ 43, 0x00},	/* Register 43 */
//				{ 44, 0x06},	/* Register 44 */
//				{ 45, 0x11},	/* Register 45 */
//				{ 46, 0x00},	/* Register 46 */
//				{ 47, 0x1B},	/* Register 47 */
//				{ 48, 0xBF},	/* Register 48 */
//				{ 55, 0x00},	/* Register 55 */
//				{131, 0x1F},	/* Register 131 */
//				{132, 0x02},	/* Register 132 */
//				{137, 0x01},	/* Register 137 */
//				{138, 0x0F},	/* Register 138 */
//				{139, 0xFF},	/* Register 139 */
//				{142, 0x00},	/* Register 142 */
//				{143, 0x00},	/* Register 143 */
//				{136, 0x40}		/* Register 136 */

};



SI324Info InitTable_RTM[] = {
		//		100MHz free run
				{  0, 0x54},	/* Register 0 */
				{  1, 0xE1},	/* Register 1 */
				{  2, 0x82},	/* Register 2 */
				{  3, 0x55},	/* Register 3 */
				{  4, 0x92},	/* Register 4 */
				{  5, 0xAD},	/* Register 5 */
				{  6, 0x2D},	/* Register 6 */
				{  7, 0x2A},	/* Register 7 */
				{  8, 0x00},	/* Register 8 */
				{  9, 0xC0},	/* Register 9 */
				{ 10, 0x00},	/* Register 10 */
				{ 11, 0x40},	/* Register 11 */
				{ 19, 0x29},	/* Register 19 */
				{ 20, 0x3E},	/* Register 20 */
				{ 21, 0xFE},	/* Register 21 */
				{ 22, 0xDF},	/* Register 22 */
				{ 23, 0x1F},	/* Register 23 */
				{ 24, 0x3F},	/* Register 24 */
				{ 25, 0x60},	/* Register 25 */
				{ 31, 0x00},	/* Register 31 */
				{ 32, 0x00},	/* Register 32 */
				{ 33, 0x05},	/* Register 33 */
				{ 34, 0x00},	/* Register 34 */
				{ 35, 0x00},	/* Register 35 */
				{ 36, 0x05},	/* Register 36 */
				{ 40, 0x60},	/* Register 40 */
				{ 41, 0x0A},	/* Register 41 */
				{ 42, 0xA9},	/* Register 42 */
				{ 43, 0x00},	/* Register 43 */
				{ 44, 0x00},	/* Register 44 */
				{ 45, 0x5A},	/* Register 45 */
				{ 46, 0x00},	/* Register 46 */
				{ 47, 0x01},	/* Register 47 */
				{ 48, 0x9F},	/* Register 48 */
				{ 55, 0x00},	/* Register 55 */
				{131, 0x1F},	/* Register 131 */
				{132, 0x02},	/* Register 132 */
				{137, 0x01},	/* Register 137 */
				{138, 0x0F},	/* Register 138 */
				{139, 0xFF},	/* Register 139 */
				{142, 0x00},	/* Register 142 */
				{143, 0x00},	/* Register 143 */
				{136, 0x40}		/* Register 136 */
};

void i2c_init()
{
	PINSEL_ConfigPin(0, 0, 3);
	PINSEL_ConfigPin(0, 1, 3);
	PINSEL_SetOpenDrainMode(0, 0, ENABLE);
	PINSEL_SetOpenDrainMode(0, 1, ENABLE);
	PINSEL_SetI2CMode(0, 0, PINSEL_I2CMODE_OPENDRAINIO);
	PINSEL_SetI2CMode(0, 1, PINSEL_I2CMODE_OPENDRAINIO);
	I2C_Init(I2C_1, 100000);
	I2C_Cmd(I2C_1, ENABLE);
}

uint8_t i2c_write(uint8_t slaveAddr, uint8_t * buff, uint8_t len)
{
	I2C_M_SETUP_Type xfer = { 0 };

	uint8_t retry_count = 10;

	xfer.sl_addr7bit = slaveAddr;
	xfer.tx_data = buff;
	xfer.tx_length = len;
	xfer.rx_length = 0;

	while (I2C_MasterTransferData(I2C_1, &xfer, I2C_TRANSFER_POLLING) != SUCCESS)
	{
		retry_count--;
		if (!retry_count) break;
	}

//	while (I2C_MasterTransferData(I2C_1, &xfer, I2C_TRANSFER_POLLING) != SUCCESS) {}
	return len - xfer.tx_count;
}

uint8_t i2c_read(uint8_t slaveAddr, uint8_t * buff, uint8_t len)
{
	I2C_M_SETUP_Type xfer = { 0 };
	uint8_t retry_count = 10;

	xfer.sl_addr7bit = slaveAddr;
	xfer.rx_data = buff;
	xfer.rx_length = len;
	xfer.tx_length = 0;

	while (I2C_MasterTransferData(I2C_1, &xfer, I2C_TRANSFER_POLLING) != SUCCESS)
	{
		retry_count--;
		if (!retry_count) break;
	}

	return len - xfer.rx_count;
}





/* Function that probes all available slaves connected to an I2C bus */
void i2c_probe_slaves(void)
{
	int i;
	uint8_t ch[2];

	printf("Probing available I2C devices...\r\n");
	printf("\r\n\t00\t01\t02\t03\t04\t05\t06\t07\t08\t09\t0A\t0B\t0C\t0D\t0E\t0F");
	printf("\r\n==\t==\t==\t==\t==\t==\t==\t==\t==\t==\t==\t==\t==\t==\t==\t==\t==\t==\t==\t==\t==\t==\t==\t==\t==\t==");
	for (i = 0; i <= 0x7F; i++) {
		if (!(i & 0x0F))
		{
			printf("\r\n%02X  ", i >> 4);
		}

		if (i <= 7 || i > 0x78)
		{
			printf("\t");
			continue;
		}

		I2C_M_SETUP_Type xfer = { 0 };

		xfer.sl_addr7bit = i;
		xfer.rx_data = ch;
		xfer.rx_length = 1 + (i == 0x48);

		I2C_MasterTransferData(I2C_1, &xfer, I2C_TRANSFER_POLLING);

		if(xfer.rx_count > 0)
		{
			printf("\t%02X", i);
		} else {
			printf("\t---");
		}
	}
	printf("\r\n");
}

void i2c_tcaselect(uint8_t i)
{
	i2c_write(TCAADDR, &i, 1);
}


void i2c_probe_slaves2(void)
{
	uint8_t ch[2];

	printf("TCAScanner start\n");

	for (uint8_t t = 0; t < 8; t++)
	{
		i2c_tcaselect(t);
	    printf("TCA Port #%d\n", t);
	    for (uint8_t addr = 0; addr<=127; addr++)
	    {
	    	if (addr == TCAADDR) continue;
	    	I2C_M_SETUP_Type xfer = { 0 };

			xfer.sl_addr7bit = addr;
			xfer.rx_data = ch;
			xfer.rx_length = 1; // only one byte response

			I2C_MasterTransferData(I2C_1, &xfer, I2C_TRANSFER_POLLING);

			if(xfer.rx_count > 0)
			{
				printf("Found I2C %02X", addr);
			}
	    }
	}
}


void i2c_setSI5324(void)
{
	uint8_t WrBuffer[2];
	int Index;


	// set proper MUX address
	WrBuffer[0] = 16;
	i2c_write(TCAADDR,WrBuffer, 1); //select channel 4 only

	for (Index = 0; Index < sizeof(InitTable)/2; Index++) {
		WrBuffer[0] = InitTable[Index].RegIndex;
		WrBuffer[1] = InitTable[Index].Value;

		// TODO CHECK SI5324 ADDRESS
		i2c_write(IIC_SLAVE_ADDR, WrBuffer, 2);
	}
	WrBuffer[0] = 0;
	i2c_write(TCAADDR,WrBuffer, 1); //deselect switch
}

void i2c_setSI5324_RTM(void)
{
	uint8_t WrBuffer[2];
	int Index;


	// set proper MUX address
	WrBuffer[0] = 128;
	i2c_write(TCAADDR, WrBuffer, 1); //select channel 7 only
	// set proper MUX RTM address
	WrBuffer[0] = 32;
	i2c_write(TCARTMADDR ,WrBuffer, 1); //select channel 5 only


	for (Index = 0; Index < sizeof(InitTable_RTM)/2; Index++) {
		WrBuffer[0] = InitTable_RTM[Index].RegIndex;
		WrBuffer[1] = InitTable_RTM[Index].Value;

		// TODO CHECK SI5324 ADDRESS
		i2c_write(IIC_SLAVE_ADDR, WrBuffer, 2);
	}


	WrBuffer[0] = 0;
	i2c_write(TCARTMADDR ,WrBuffer, 1); //deselect switch
	// set proper MUX address
	WrBuffer[0] = 0;
	i2c_write(TCAADDR, WrBuffer, 1); //deselect channel
}


void i2c_RTM_PWRON(void)
{
	uint8_t WrBuffer[2];
	int Index;


	// set proper MUX address
	WrBuffer[0] = 128;
	i2c_write(TCAADDR, WrBuffer, 1); //select channel 7 only
	// set proper MUX RTM address
	WrBuffer[0] = 1<<7 | 0<<6 | 0<<5 | 0<<4 | 0<<3 | 0<<2 | 0<<1 | 1;
	i2c_write(0x3E ,WrBuffer, 1); //set IO extender reset, LEDs and power
	delay_ms(200);
	WrBuffer[0] = 1<<7 | 0<<6 | 1<<5 | 1<<4 | 0<<3 | 0<<2 | 0<<1 | 1;
	i2c_write(0x3E ,WrBuffer, 1); //set IO extender, LEDs and power

	// set proper MUX address
	WrBuffer[0] = 0;
	i2c_write(TCAADDR, WrBuffer, 1); //deselect channel
}
