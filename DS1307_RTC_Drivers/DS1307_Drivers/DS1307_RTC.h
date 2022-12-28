/*
 * 									DS1307_RTC.h
 *
 * This file contains all the DS1307-related APIs supported by the driver.
 *
 */

#ifndef DS1307_RTC_H_
#define DS1307_RTC_H_

#include <stdint.h>
#include "stm32f407xx.h"
#include "stm32f407xx_gpio_drivers.h"
#include "stm32f407xx_i2c_drivers.h"


/* -- Application Configurable Items  -- */
#define DS1307_I2C_Peripheral		I2C1				// DS1307 is connected to I2C1 Peripheral
#define DS1307_I2C_GPIO_PORT		GPIOB				// DS1307 is connected to GPIO port B
#define DS1307_I2C_SDA_PIN		GPIO_Pin_7			// DS1307 SDA is connected to PB7
#define DS1307_I2C_SCL_PIN		GPIO_Pin_6			// DS1307 SCL is connected to PB6
#define DS1307_I2C_PUPD			GPIO_PIN_PU			// Internal Pull-Up
#define DS1307_I2C_SPEED		I2C_SCL_SPEED_SM		// I2C is in Standard Mode (DO NOT CHANGE)


/* -- Registers Addresses -- */

// Time-keeper Register
#define DS1307_SECONDS_ADDR		0x00
#define DS1307_MINUTES_ADDR		0x01
#define DS1307_HOURS_ADDR		0x02
#define DS1307_DAY_ADDR			0x03
#define DS1307_DATE_ADDR		0x04
#define DS1307_MONTH_ADDR		0x05
#define DS1307_YEAR_ADDR		0x06

/* -- Time Format -- */
#define TIME_FORMAT_12H_AM		0
#define TIME_FORMAT_12H_PM		1
#define TIME_FORMAT_24H			2

/* -- Days -- */
#define SUNDAY				1
#define MONDAY				2
#define TUESDAY				3
#define WEDNESDAY			4
#define THURSDAY			5
#define FRIDAY				6
#define SATURDAY			7

/* -- Device Address (I2C) -- */
#define DS1307_I2C_ADDR			0x68

/* -- To hold Date information -- */
typedef struct
{
	uint8_t date;
	uint8_t day;
	uint8_t month;
	uint8_t year;

}RTC_Date_h;


/* -- To hold Time information -- */
typedef struct
{
	uint8_t seconds;
	uint8_t minutes;
	uint8_t hours;
	uint8_t timeFormat;

}RTC_Time_h;


/* -- APIs Supported by DS1307_RTC driver -- */

// To enable DS1307
uint8_t DS1307_Init(void);

// To initialize: Current Time and Date Information
void DS1307_Set_Current_Time(RTC_Time_h *pRTCTimehandle);
void DS1307_Set_Current_Date(RTC_Date_h *pRTCDatehandle);

// To get: the Current Time and Date Information
void DS1307_Get_Current_Time(RTC_Time_h *pRTCTimehandle);
void DS1307_Get_Current_Date(RTC_Date_h *pRTCDatehandle);


#endif /* DS1307_RTC_H_ */
