/*
 * Copyright (c) 2014-2017 Cesanta Software Limited
 * All rights reserved
 */
#include <stdbool.h>

#include "esp_sleep.h"
// #include "esp_deep_sleep.h"

#include "common/cs_dbg.h"


#include "common/platform.h"

#include "mgos_app.h"
#include "mgos_hal.h"
#include "mgos_hooks.h"
#include "mgos_sys_config.h"


bool mgos_deepsleep_enable_gpio_wakeup(int gpio)
{
	return true;
}


bool mgos_deepsleep_ms(int ms)
{
	if (ms < 0) return false;

//	LOG(LL_WARN, ("=== esp_deep_sleep(%d) ===", ms));

    printf("config Timer\n");
//    esp_deep_sleep_enable_timer_wakeup(1000LL * ms); // set timer but don't sleep now
	esp_sleep_enable_timer_wakeup(1000LL * ms);
    printf("config IO\n");

    //!< Keep power domain enabled in deep sleep, if it is needed by one of the wakeup options. Otherwise power it down.
    esp_sleep_pd_config(ESP_PD_DOMAIN_RTC_PERIPH, ESP_PD_OPTION_AUTO); 
    esp_sleep_pd_config(ESP_PD_DOMAIN_RTC_SLOW_MEM, ESP_PD_OPTION_ON); 

    printf("deep sleep\n");
    esp_deep_sleep_start();

	// Never reached
	return true;
}


//
//
bool mgos_deepsleep_now(int ms)
{
	if (ms < 0) return false;

//	LOG(LL_WARN, ("=== esp_deep_sleep(%d) ===", ms));

    printf("config Timer\n");
//    esp_deep_sleep_enable_timer_wakeup(1000LL * ms); // set timer but don't sleep now
	esp_sleep_enable_timer_wakeup(1000LL * ms);
    printf("config IO\n");
    esp_sleep_pd_config(ESP_PD_DOMAIN_RTC_PERIPH, ESP_PD_OPTION_AUTO); //!< Keep power domain enabled in deep sleep, if it is needed by one of the wakeup options. Otherwise power it down.

    printf("deep sleep\n");
    esp_deep_sleep_start();

//	esp_deep_sleep((uint64_t)(1000LL * ms));

	// Never reached
	return true;
}
