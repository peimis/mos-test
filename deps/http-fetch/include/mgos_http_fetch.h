/*
 * Copyright (c) 2014-2017 Cesanta Software Limited
 * All rights reserved
 */

/*
 * Deepsleep API.
 */

#ifndef CS_MOS_LIBS_HTTP_FETCH_SRC_MGOS_HTTP_FETCH_H_
#define CS_MOS_LIBS_HTTP_FETCH_SRC_MGOS_HTTP_FETCH_H_

#include <stdbool.h>

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

/* FFI-able signature of the function that receives response to a request. */
typedef void (*mgos_http_fetch_cb_t)(char *result, int error_code, int bytes, void *ud);

/*
 *
 *  Fetch a file from url.
 *
 *  Params:
 *  url:		Address of file to fetch.
 *  path:		Path of local file to write.
 *  cb:			Callback function.
 *  Return:
 *  true - SUCCESS, false - FAIL.
 *
 *  Note:
 *  -
 */
int mgos_http_fetch(char *url, char *path, mgos_http_fetch_cb_t cb, void *ud);

bool mgos_http_fetch_init(void);

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* CS_MOS_LIBS_HTTP_FETCH_SRC_MGOS_HTTP_FETCH_H_ */