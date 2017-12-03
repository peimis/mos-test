/*
 * Copyright (c) 2014-2017 Cesanta Software Limited
 * All rights reserved
 */

/*
 * View this file on GitHub:
 * [mgos_pwm.c](https://github.com/mongoose-os-libs/pwm/blob/master/src/mgos_pwm.c)
 */

#include <stdbool.h>
#include "esp_attr.h"
#include "mgos.h"


#define SLEEP_MAGIC	0x10DEBBAA

#define WAKEUP_NORMAL		0
#define WAKEUP_WIFI_OFF		1

/* Variable holding number of times ESP32 restarted since first boot.
 * It is placed into RTC memory using RTC_DATA_ATTR and
 * maintains its value when ESP32 wakes from deep sleep.
 */
RTC_DATA_ATTR int wakeup_count;
RTC_DATA_ATTR int wakeupper;
RTC_DATA_ATTR int wakeup_mode;

int mgos_deepsleep_count(void) {
	return wakeup_count;
}

//
//
void mgos_deepsleep_set_wakeup_mode(int mode)
{
	if (0 == (mode & 1)) {
		wakeup_mode=WAKEUP_NORMAL;
		printf("WAKEUP normal");
	} else {
		wakeup_mode=WAKEUP_WIFI_OFF;
		printf("WAKEUP Wifi OFF");
	}
}

//
//
bool mgos_sleep_wakeup_wifi_disable(void)
{	
	return wakeup_mode;
}


//
//
bool mgos_deepsleep_init(void) {
	if (SLEEP_MAGIC != wakeupper) {
		wakeup_count=0;
		wakeup_mode=WAKEUP_NORMAL;
		wakeupper=SLEEP_MAGIC;
	}
	wakeup_count++;
	
	return true;
}
