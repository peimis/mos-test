/*
 * Copyright (c) 2014-2017 Cesanta Software Limited
 * All rights reserved
 */

/*
 * Deepsleep API.
 */

#ifndef CS_MOS_LIBS_DEEPSLEEP_SRC_MGOS_DEEPSLEEP_H_
#define CS_MOS_LIBS_DEEPSLEEP_SRC_MGOS_DEEPSLEEP_H_

#include <stdbool.h>

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

/*
 *
 *  Put the ESP32 to deep sleep mode.
 *
 *  Params:
 *  ms:		Duration of deep_sleep in ms:
 *    		0 sets device to sleep until external interrupt / ulp wakes cpu.
 *  Return:
 *  true - SUCCESS, false - FAIL.
 *
 *  Note:
 *  Sleep.
 */
bool mgos_deepsleep_now(int ms);
bool mgos_deepsleep_enable_gpio_wakeup(int gpio);
bool mgos_deepsleep_ms(int ms);


int mgos_deepsleep_count(void);
bool mgos_deepsleep_init(void);

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* CS_MOS_LIBS_DEEPSLEEP_SRC_MGOS_DEEPSLEEP_H_ */