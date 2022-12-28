/*
 *
 *									 01_DS1307_RTC_Basic.c
 *
 */

#include <stdio.h>
#include "DS1307_RTC.h"

/* -- To initialize the semi-hosting features -- */
extern void initialise_monitor_handles(void);

/* -- Helper Functions -- */
char* get_DayofWeek(uint8_t day);			// To get current day of the week
char* Time_to_String(RTC_Time_h *pRTCTime); 		// To convert time information into a string [hh:mm:ss]
char* Date_to_String(RTC_Date_h *pRTCDate); 		// To convert date information into a string [dd-mm-yy]


int main(void)
{

	/* -- To enable Semi-Hosting [before using any printfs] -- */
	initialise_monitor_handles();

	printf("DS1307 RTC: Basic Functionality. \n");

	uint8_t initORfail;

	RTC_Date_h currentDate;
	RTC_Time_h currentTime;

	char *AMorPM;

	// Initialize DS1307
	initORfail = DS1307_Init();

	if (initORfail)
	{
		printf("DS1307 Initialization Failed. [Exit Manually]\n ");
		while(1);
	}

	// Program current time and date
	currentDate.date = 27;
	currentDate.day = TUESDAY;
	currentDate.month = 12;
	currentDate.year = 22;		// JUST last 2 digits

	currentTime.hours = 10;
	currentTime.minutes = 25;
	currentTime.seconds = 1;
	currentTime.timeFormat = TIME_FORMAT_12H_PM;

	/* -- Program Current Time and Date -- */
	DS1307_Set_Current_Date(&currentDate);
	DS1307_Set_Current_Time(&currentTime);


	/* -- Get Current Time and Date -- */
	DS1307_Get_Current_Date(&currentDate);
	DS1307_Get_Current_Time(&currentTime);

	if (currentTime.timeFormat != TIME_FORMAT_24H)
	{
		// Print Time with AM or PM details
		if (currentTime.timeFormat == TIME_FORMAT_12H_PM)
		{
			// PM
			AMorPM = "PM";
		}
		else
		{
			// AM
			AMorPM = "AM";
		}

		printf("Current time = %s %s\n", Time_to_String(&currentTime), AMorPM);
	}
	else
	{
		// 24 Hours Format
		printf("Current time = %s\n", Time_to_String(&currentTime));
	}

	// Print Date
	printf("Current Date = %s <%s> \n",Date_to_String(&currentDate), get_DayofWeek(currentDate.day));

	return 0;
}


/* ------------------------------------------------------------------------------------------------------
 * Name		:	Number_to_String
 * Description	:	To convert number into string
 *
 * Parameter 1	:	number to convert (uint8_t)
 * Parameter 2	:	Buffer (char *)
 * Return Type	:	none (void)
 * Note		:
 * ------------------------------------------------------------------------------------------------------ */
void Number_to_String(uint8_t number, char* buff)
{
	if (number < 10)
	{
		buff[0] = '0';
		buff[1] = number + 48;	// ASCII
	}
	else if (number >= 10 && number < 99)
	{
		buff[0] = (number / 10) + 48;	// Split and add 48
		buff[1] = (number % 10) + 48;

	}

}


/* ------------------------------------------------------------------------------------------------------
 * Name		:	Time_to_String
 * Description	:	To convert time information from DS1307 into string
 *
 * Parameter 1	:	Handle pointer variable (RTC_Time_h)
 * Return Type	:	none (void)
 * Note		:	Also, converts string in hh:mm:ss format
 * ------------------------------------------------------------------------------------------------------ */
char* Time_to_String(RTC_Time_h *pRTCTime)
{
	// Format -> hh:mm:ss [9 characters]

	static char timeBuff[9];

	timeBuff[2] = ':';
	timeBuff[5] = ':';

	Number_to_String(pRTCTime->hours, timeBuff);
	Number_to_String(pRTCTime->minutes, &timeBuff[3]);
	Number_to_String(pRTCTime->seconds, &timeBuff[6]);

	timeBuff[8] = '\0';

	return timeBuff;

}


/* ------------------------------------------------------------------------------------------------------
 * Name		:	Date_to_String
 * Description	:	To convert date information from DS1307 into string
 *
 * Parameter 1	:	Handle pointer variable (RTC_Date_h)
 * Return Type	:	none (void)
 * Note		:	Also, converts string in dd-mm-yy format
 * ------------------------------------------------------------------------------------------------------ */
char* Date_to_String(RTC_Date_h *pRTCDate)
{
	// Format -> dd-mm-yy
	static char dateBuff[9];

	dateBuff[2] = '-';
	dateBuff[5] = '-';

	Number_to_String(pRTCDate->date,dateBuff);
	Number_to_String(pRTCDate->month,&dateBuff[3]);
	Number_to_String(pRTCDate->year,&dateBuff[6]);

	dateBuff[8] = '\0';

	return dateBuff;

}


/* ------------------------------------------------------------------------------------------------------
 * Name		:	get_DayofWeek
 * Description	:	To get days of week
 *
 * Parameter 1	:	day (place) (uint8_t)
 * Return Type	:	day of week (char *)
 * Note		:
 * ------------------------------------------------------------------------------------------------------ */
char* get_DayofWeek(uint8_t day)
{
	char* Days[] = {"Sunday","Monday","Tuesday","Wednesday","Thursday","Friday","Saturday"};

	return Days[day - 1];
}
