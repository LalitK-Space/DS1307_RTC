/*
 * 									DS1307_RTC.c
 *
 *  This file contains DS1307 driver API implementations.
 *
 */

#include "DS1307_RTC.h"

#include<stdint.h>
#include<string.h>

// I2C Global Handle Variable
I2C_Handle_t DS1307_I2CHandle;

/* --Helper Functions-- */
static void DS1307_I2C_PinConfig(void);
static void DS1307_I2C_Config(void);
static void DS1307_Write(uint8_t value, uint8_t RegAddress);
static uint8_t DS1307_Read(uint8_t RegAddress);
static uint8_t Binary_to_BCD(uint8_t value);
static uint8_t BCD_to_Binary(uint8_t value);

/* ------------------------------------------------------------------------------------------------------
 * Name		:	DS1307_Init
 * Description	:	To initialize the DS1307 RTC
 *
 * Parameter 1	:	none (void)
 * Return Type	:	uint8_t ()
 * Note		:	if returns 0, meaning CH is SET (Clock halt is removed, clock is enabled).
 * ------------------------------------------------------------------------------------------------------ */
uint8_t DS1307_Init(void)
{
	/* -Step 1. Initialize I2C Pins- */
	DS1307_I2C_PinConfig();

	/* -Step 2. Initialize I2C Peripheral- */
	DS1307_I2C_Config();

	/* -Step 3. Enable I2C Peripheral- */
	I2C_PeripheralControl(DS1307_I2C_Peripheral, ENABLE);

	/* -Step 4. Enable Time-keeper Registers of DS1307 (Disabled by default)- */
	/*
	 * Address: 0x00 [Seconds Register]
	 * Bit[7] : CH (Clock halt)
	 * Write 0 to Enable clock
	 * */
	DS1307_Write(0x00, DS1307_SECONDS_ADDR);

	/* -Step 5. Ensure CH bit is Cleared (Clock is Enabled)- */
	uint8_t CH_State = DS1307_Read(DS1307_SECONDS_ADDR);

	/* -Step 6. Return CH State (>> 7 because 7th Bit in Time-Keeper Register)- */
	return ((CH_State >> 7) & 0x1);
}



/* ------------------------------------------------------------------------------------------------------
 * Name		:	DS1307_Set_Current_Time
 * Description	:	To set the current time
 *
 * Parameter 1	:	Handle pointer variable
 * Return Type	:	none (void)
 * Note		: Write to DS1307 Registers [Registers: seconds, minutes, and Hours]
 * ------------------------------------------------------------------------------------------------------ */
void DS1307_Set_Current_Time(RTC_Time_h *pRTCTimehandle)
{
	/* -Step 1. Program Seconds- */

	uint8_t seconds;

	// a. Convert User inputed value into BCD format
	seconds = Binary_to_BCD(pRTCTimehandle->seconds);

	// b. Make sure 7th bit is Cleared (CH) [if 1, Clock is Halted]
	seconds &= ~(1 << 7);

	// c. Write into DS1307 Seconds Register
	DS1307_Write(seconds, DS1307_SECONDS_ADDR);

	/* -Step 2. Program Minutes- */

	uint8_t minutes;

	// a. Convert User inputed value into BCD format
	minutes = Binary_to_BCD(pRTCTimehandle->minutes);

	// b. Write into DS1307 Seconds Register
	DS1307_Write(minutes, DS1307_MINUTES_ADDR);

	/* -Step 3. Program Hours- */

	/* [NOTE]
	 * Bit[6] is defined as 12-Hour(HIGH) or 24-Hours(LOW)
	 *
	 * when Bit[6] is HIGH i.e. 12-Hours Mode, Bit[5] is defined as AM/PM bit
	 * When Bit[5] is HIGH -> PM
	 *
	 * In 24-Hour Mode, Bit[5] is the second 10-Hour bit (20 to 23 Hours).
	 * The Hours value must be re-entered whenever the 12/24-Hour Mode is changed
	 *
	 * */

	uint8_t hours;

	// a. Convert User inputed value into BCD format
	hours = Binary_to_BCD(pRTCTimehandle->hours);

	// b. Perform Checks according to [NOTE]
	if (pRTCTimehandle->timeFormat == TIME_FORMAT_24H)
	{
		/* -Format is 24 Hours- */

		// a. Clear Bit[6]: Bit[6] is defined as 24-Hours when LOW
		hours &= ~(1 << 6);

	}
	else
	{
		/* -Format is 12 Hours- */

		// a. SET Bit[6]: Bit[6] is defined as 12-Hour when HIGH
		hours |= (1 << 6);

		// b. Check for AM or PM
		if (pRTCTimehandle->timeFormat == TIME_FORMAT_12H_PM)
		{
			// PM

			// SET Bit[5]: When HIGH -> PM
			hours |= (1 << 5);

		}
		else if (pRTCTimehandle->timeFormat == TIME_FORMAT_12H_AM)
		{
			// AM

			// Clear Bit[5]: When LOW -> AM
			hours &= ~(1 << 5);

		}
		else
		{
			// Nothing
		}

	}

	// c. Write into DS1307 Hours Register
	DS1307_Write(hours, DS1307_HOURS_ADDR);

}



/* ------------------------------------------------------------------------------------------------------
 * Name		:	DS1307_Set_Current_Date
 * Description	:	To set the current date
 *
 * Parameter 1	:	Handle pointer variable
 * Return Type	:	none (void)
 * Note		:	Write to DS1307 Registers [Registers: Date, Day, Month, and year]
 * ------------------------------------------------------------------------------------------------------ */
void DS1307_Set_Current_Date(RTC_Date_h *pRTCDatehandle)
{
	/* -Step 1. Program Date- */

	uint8_t date;

	// a. Convert User inputed value into BCD format
	date = Binary_to_BCD(pRTCDatehandle->date);

	// b. Write into DS1307 Date Register
	DS1307_Write(date, DS1307_DATE_ADDR);

	/* -Step 2. Program Day- */

	uint8_t day;

	// a. Convert User inputed value into BCD format
	day = Binary_to_BCD(pRTCDatehandle->day);

	// b. Write into DS1307 Date Register
	DS1307_Write(day, DS1307_DAY_ADDR);

	/* -Step 3. Program Month- */

	uint8_t month;

	// a. Convert User inputed value into BCD format
	month = Binary_to_BCD(pRTCDatehandle->month);

	// b. Write into DS1307 Date Register
	DS1307_Write(month, DS1307_MONTH_ADDR);

	/* -Step 4. Program Year- */

	uint8_t year;

	// a. Convert User inputed value into BCD format
	year = Binary_to_BCD(pRTCDatehandle->year);

	// b. Write into DS1307 Date Register
	DS1307_Write(year, DS1307_YEAR_ADDR);

}


/* ------------------------------------------------------------------------------------------------------
 * Name		:	DS1307_Get_Current_Time
 * Description	:	To get the current date
 *
 * Parameter 1	:	Handle pointer variable
 * Return Type	:	none (void)
 * Note		:	Read from DS1307 Registers [Registers: seconds, minutes, and Hours]
 * ------------------------------------------------------------------------------------------------------ */
void DS1307_Get_Current_Time(RTC_Time_h *pRTCTimehandle)
{
	/* -Step 1. Get Seconds- */
	uint8_t seconds;

	// a. Read from Seconds Register
	seconds = DS1307_Read(DS1307_SECONDS_ADDR);

	// b. Make sure 7th bit is Cleared (CH) [if 1, Clock is Halted] [NOT required in data]
	seconds &= ~(1 << 7);

	// c. Convert BCD (values from register) to Binary
	seconds = BCD_to_Binary(seconds);

	// d. Copy into seconds member element
	pRTCTimehandle->seconds = seconds;

	/* -Step 2. Get Minutes- */
	uint8_t minutes;

	// a. Read from Minutes Register
	minutes = DS1307_Read(DS1307_MINUTES_ADDR);

	// b. Convert BCD (values from register) to Binary
	minutes = BCD_to_Binary(minutes);

	// c. Copy into minutes member element
	pRTCTimehandle->minutes = minutes;

	/* -Step 3. Get Hours- */
	uint8_t hours;

	// a. Read from Hours Register
	hours = DS1307_Read(DS1307_HOURS_ADDR);

	// b. Checks for Bit[5]: AM/PM and Bit[6]: Time Format
	if (hours & (1 << 6))
	{
		// Bit[6] is SET -> 12-Hour Format

		// Check for AM or PM
		if (hours & (1 << 5))
		{
			// Bit[5] is HIGH -> PM
			pRTCTimehandle->timeFormat = TIME_FORMAT_12H_PM;
		}
		else
		{
			// Bit[5] is LOW -> AM
			pRTCTimehandle->timeFormat = TIME_FORMAT_12H_AM;
		}

	}
	else
	{
		// Bit[6] is Cleared -> 24-Hour Format
		pRTCTimehandle->timeFormat = TIME_FORMAT_24H;
	}

	// c. Discard Bit[5] and Bit[6] [NOT required in data]
	hours &= ~(1 << 5);
	hours &= ~(1 << 6);

	// d. Convert BCD (values from register) to Binary
	hours = BCD_to_Binary(hours);

	// d. Copy into Hours member element
	pRTCTimehandle->hours = hours;

}


/* ------------------------------------------------------------------------------------------------------
 * Name		:	DS1307_Get_Current_Date
 * Description	:	To get the current date
 *
 * Parameter 1	:	Handle pointer variable
 * Return Type	:	none (void)
 * Note		:	Read from DS1307 Registers [Registers: Date, Day, Month, and year]
 * ------------------------------------------------------------------------------------------------------ */
void DS1307_Get_Current_Date(RTC_Date_h *pRTCDatehandle)
{
	/* -Step 1. Get Date- */
	uint8_t date;

	// a. Read from Date Register
	date = DS1307_Read(DS1307_DATE_ADDR);

	// b. Convert BCD (values from register) to Binary
	date = BCD_to_Binary(date);

	// c. Copy into Date member element
	pRTCDatehandle->date = date;


	/* -Step 2. Get Day- */
	uint8_t day;

	// a. Read from Day Register
	day = DS1307_Read(DS1307_DAY_ADDR);

	// b. Convert BCD (values from register) to Binary
	day = BCD_to_Binary(day);

	// c. Copy into Day member element
	pRTCDatehandle->day = day;


	/* -Step 3. Get Month- */
	uint8_t month;

	// a. Read from Month Register
	month = DS1307_Read(DS1307_MONTH_ADDR);

	// b. Convert BCD (values from register) to Binary
	month = BCD_to_Binary(month);

	// c. Copy into Month member element
	pRTCDatehandle->month = month;


	/* -Step 4. Get Year- */
	uint8_t year;

	// a. Read from Month Register
	year = DS1307_Read(DS1307_YEAR_ADDR);

	// b. Convert BCD (values from register) to Binary
	year = BCD_to_Binary(year);

	// c. Copy into Year member element
	pRTCDatehandle->year = year;

}


/* --Helper Functions-- */
/* ------------------------------------------------------------------------------------------------------
 * Name		:	DS1307_I2C_PinConfig
 * Description	:	Helper Functions
 *
 * Parameter 1	:	none (void)
 * Return Type	:	none (void)
 * Note		: To initialize GPIO to behave as I2C Pins (Alternate Functionality)
 * ------------------------------------------------------------------------------------------------------ */
static void DS1307_I2C_PinConfig(void)
{
	// GPIO Handle Variables
	GPIO_Handle_t I2C_SDA;
	GPIO_Handle_t I2C_SCL;

	/* -Initialize the handle variable to ZERO, in order to prevent registers to have random values- */
	memset(&I2C_SDA,0,sizeof(I2C_SDA));
	memset(&I2C_SCL,0,sizeof(I2C_SCL));


	/* -Initialize I2C SDA Line- */

	// Select Port
	I2C_SDA.pGPIOx = DS1307_I2C_GPIO_PORT;

	// Pin Configuration: PIN MODE
	I2C_SDA.GPIO_PinConfig.GPIO_PinMode = GPIO_MODE_ALTFUNC;

	// Pin Configuration: Alternate Functionality
	I2C_SDA.GPIO_PinConfig.GPIO_PinAltFuncMode = 4;	// Alternate Functionality Mode: 4

	// Pin Configuration: PIN NUMBER
	I2C_SDA.GPIO_PinConfig.GPIO_PinNumber = DS1307_I2C_SCL_PIN; // Defined in DS1307_RTC.h

	// Pin Configuration: Output Type
	I2C_SDA.GPIO_PinConfig.GPIO_PinOPType = GPIO_OP_TYPE_OD;	// Open Drain

	// Pin Configuration: Internal Pull-up/down
	I2C_SDA.GPIO_PinConfig.GPIO_PinPuPdControl = DS1307_I2C_PUPD; // Defined in DS1307_RTC.h

	// Pin Configuration: Pin Speed
	I2C_SDA.GPIO_PinConfig.GPIO_PinSpeed = GPIO_SPEED_VERY_HIGH;

	/* -Configure GPIO as I2Cx_SDA- */
	GPIO_Init(&I2C_SDA);

	/* -Initialize I2C SCL Line- */

	// Select Port
	I2C_SCL.pGPIOx = DS1307_I2C_GPIO_PORT;

	// Pin Configuration: PIN MODE
	I2C_SCL.GPIO_PinConfig.GPIO_PinMode = GPIO_MODE_ALTFUNC;

	// Pin Configuration: Alternate Functionality
	I2C_SCL.GPIO_PinConfig.GPIO_PinAltFuncMode = 4;	// Alternate Functionality Mode: 4

	// Pin Configuration: PIN NUMBER
	I2C_SCL.GPIO_PinConfig.GPIO_PinNumber = DS1307_I2C_SDA_PIN; // Defined in DS1307_RTC.h

	// Pin Configuration: Output Type
	I2C_SCL.GPIO_PinConfig.GPIO_PinOPType = GPIO_OP_TYPE_OD;	// Open Drain

	// Pin Configuration: Internal Pull-up/down
	I2C_SCL.GPIO_PinConfig.GPIO_PinPuPdControl = DS1307_I2C_PUPD; // Defined in DS1307_RTC.h

	// Pin Configuration: Pin Speed
	I2C_SCL.GPIO_PinConfig.GPIO_PinSpeed = GPIO_SPEED_VERY_HIGH;

	/* -Configure GPIO as I2Cx_SCl- */
	GPIO_Init(&I2C_SCL);

}


/* ------------------------------------------------------------------------------------------------------
 * Name		:	DS1307_I2C_Config
 * Description	:	Helper Functions
 *
 * Parameter 1	:	none (void)
 * Return Type	:	none (void)
 * Note		: To initialize I2C Peripheral
 * ------------------------------------------------------------------------------------------------------ */
static void DS1307_I2C_Config(void)
{
	/* -- Base Address of the DS1307 I2C Peripheral -- */
	DS1307_I2CHandle.pI2Cx = DS1307_I2C_Peripheral;		// Defined in DS1307_RTC.h

	/* -- Peripheral Configuration -- */
	DS1307_I2CHandle.I2C_Config.I2C_ACK_Control	 =	I2C_ACK_ENABLE;   	// Enable ACKing
	DS1307_I2CHandle.I2C_Config.I2C_Device_Address =  DS1307_I2C_ADDR;	// Defined in DS1307_RTC.h
	DS1307_I2CHandle.I2C_Config.I2C_SCL_Speed	 =  	I2C_SCL_SPEED_SM;	// SCL Speed (Standard)

	/* -- Initialize the I2C Peripheral -- */
	I2C_Init(&DS1307_I2CHandle);


}


/* ------------------------------------------------------------------------------------------------------
 * Name		:	DS1307_Write
 * Description	:	Helper Functions
 *
 * Parameter 1	:	Value to be written (uint8_t)
 * Parameter 2	:	Register Address (where to write) (uint8_t)
 * Return Type	:	none (void)
 * Note		: To write into DS1307 Registers
 * ------------------------------------------------------------------------------------------------------ */
static void DS1307_Write(uint8_t value, uint8_t RegAddress)
{
	uint8_t TxData[2];

	TxData[0] = RegAddress;		// Send First [Device Requirement (Data sheet)]
	TxData[1] = value;

	// I2C Send Data
	I2C_MasterSendData(&DS1307_I2CHandle, TxData,2,DS1307_I2C_ADDR,I2C_REPEATED_START_DI);

}



/* ------------------------------------------------------------------------------------------------------
 * Name		:	DS1307_Read
 * Description	:	Helper Functions
 *
 * Parameter 1	:	Register Address (where to read) (uint8_t)
 * Return Type	:	register value (uint8_t)
 * Note		: To read from DS1307 Registers
 * ------------------------------------------------------------------------------------------------------ */
static uint8_t DS1307_Read(uint8_t RegAddress)
{
	uint8_t RxData;
	/*
	 * Slave (DS1307) will start transmitting data from the memory location pointed by its current address pointer.
	 * > Before reading data, initialize the address pointer to desired address (from where to read)
	 * > First, Send data (desired register address from where to read)
	 * > Then, Read operation (slave will transmit data from that address)
	 *
	 * */

	// Send desired address to read
	I2C_MasterSendData(&DS1307_I2CHandle, &RegAddress, 1, DS1307_I2C_ADDR, I2C_REPEATED_START_DI);

	// I2C Read
	I2C_MasterReceiveData(&DS1307_I2CHandle, &RxData, 1, DS1307_I2C_ADDR, I2C_REPEATED_START_DI);

	return RxData;

}


/* ------------------------------------------------------------------------------------------------------
 * Name		:	Binary_to_BCD
 * Description	:	Helper Functions
 *
 * Parameter 1	:	value (uint8_t)
 * Return Type	:	BCD equivalent of provided binary value.(uint8_t)
 * Note		:	Converts Binary values the user provides into BCD for writing into DS1307 Registers.
 * ------------------------------------------------------------------------------------------------------ */
static uint8_t Binary_to_BCD(uint8_t value)
{
	/*
	 * BCD[x] | BCD[y] = value
	 *
	 * */
	uint8_t x;
	uint8_t y;
	uint8_t BCD;

	if (value >= 10)
	{
		// a. Split the digits: example 10 -> [1][0]
		x = value / 10;		// Integer division
		y = value % 10;

		// b.
		BCD = (uint8_t) ((x << 4) | (y));
	}
	else
	{
		// BCD is equals to value
		BCD = value;
	}

	return BCD;

}


/* ------------------------------------------------------------------------------------------------------
 * Name		:	BCD_to_Binary
 * Description	:	Helper Functions
 *
 * Parameter 1	:	value (uint8_t)
 * Return Type	:	Binary equivalent of provided BCD value.(uint8_t)
 * Note		:	Converts BCD values (returned by I2C Read operation) from DS1307 Registers into Binary format.
 * ------------------------------------------------------------------------------------------------------ */
static uint8_t BCD_to_Binary(uint8_t value)
{
	/*
	 * Binary = (BCD[x] to decimal) + (BCD[y] to decimal)
	 *
	 * */
	uint8_t x;
	uint8_t y;
	uint8_t Binary;

	x = (uint8_t) ((value >> 4) * 10);
	y = (value & (uint8_t) 0x0F);			// Mask Bits[7:4] (they are x) and extract bits[3:0] -> y

	Binary = x + y;

	return Binary;

}
