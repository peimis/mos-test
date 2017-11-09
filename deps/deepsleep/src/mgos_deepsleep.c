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
/* Variable holding number of times ESP32 restarted since first boot.
 * It is placed into RTC memory using RTC_DATA_ATTR and
 * maintains its value when ESP32 wakes from deep sleep.
 */
RTC_DATA_ATTR int wakeup_count;
RTC_DATA_ATTR int wakeupper;


int mgos_deepsleep_count(void) {
	return wakeup_count;
}


//
//
bool mgos_deepsleep_init(void) {
	if (SLEEP_MAGIC != wakeupper) {
		wakeup_count=0;
		wakeupper=SLEEP_MAGIC;
	}
	wakeup_count++;
	
	return true;
}
