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
#include "mgos_http_fetch.h"

#include "mgos_json_streaming_parser.h"

#include "mgos_wug_client.h"

struct state {
	mgos_http_fetch_cb_t cb;		/* Callback function */
	void *args;						/* Callback args */
	int status;                     /* Request status */
	int64_t written;                /* Number of bytes written */
	FILE *fp;                       /* File to write to */
};


WUGConditions_t currentConds;

#if 0
//
//
static void http_cb(struct mg_connection *c, int ev, void *ev_data, void *ud)
{
	struct http_message *hm = (struct http_message *) ev_data;
	struct state *state = (struct state *) ud;

	switch (ev) {
		case MG_EV_CONNECT:
			state->status = *(int *) ev_data;
			break;

		case MG_EV_HTTP_CHUNK: {
			/*
			 * Write data to file.
		     */
//			size_t n = fwrite(hm->body.p, 1, hm->body.len, state->fp);
			size_t n =  hm->body.len;

			if (n != hm->body.len) {
				c->flags |= MG_F_CLOSE_IMMEDIATELY;
				state->status = 500;
			}
			state->written += n;
			c->flags |= MG_F_DELETE_CHUNK;
			break;
		}

		case MG_EV_HTTP_REPLY:
			/* Only when we successfully got full reply, set the status. */
			state->status = hm->resp_code;
			LOG(LL_INFO, ("Finished fetching status=%d", state->status));
			c->flags |= MG_F_CLOSE_IMMEDIATELY;
			break;

		case MG_EV_CLOSE:
			LOG(LL_INFO, ("status %d bytes %llu : cb=%p args=%p ud=%p", state->status, state->written, state->cb, state->args, ud));
			if (state->status == 200) {
				/* Report success only for HTTP 200 downloads */
				state->cb("ok", state->status, state->written, state->args);
			} else {
				state->cb("fail", state->status, state->written, state->args);
			}
			if (state->fp != NULL) fclose(state->fp);
			free(state);
			break;
	}
}

#else

int fetch_state = 0;

//
//
static void http_cb(struct mg_connection *c, int ev, void *ev_data, void *ud)
{
	struct http_message *hm = (struct http_message *) ev_data;
	struct state *state = (struct state *) ud;

	switch (ev) { 
		case MG_EV_CONNECT:
			state->status = *(int *) ev_data;
			fetch_state = 0;
			break;

		case MG_EV_HTTP_CHUNK: {
			/*
			 * Write data to file.
		     */
			size_t n=0;

			printf("chunk %d / %d\n", hm->body.len, (int)state->written);

			if (0 == fetch_state) {
				n = hm->body.len;
				char * buf = calloc(1, n);
				memcpy(buf, hm->body.p, n);
				streaming_parser_push_data(buf, n);
				free(buf);
			}

			if (n != hm->body.len) {
				c->flags |= MG_F_CLOSE_IMMEDIATELY;
				state->status = 500;
			}
			state->written += n;

			c->flags |= MG_F_DELETE_CHUNK;
			break;
		}

		case MG_EV_HTTP_REPLY:
			/* Only when we successfully got full reply, set the status. */
			state->status = hm->resp_code;
			fetch_state = 1;
			LOG(LL_INFO, ("Finished fetching status=%d", state->status));
			c->flags |= MG_F_CLOSE_IMMEDIATELY;
			break;


		case MG_EV_CLOSE:
			LOG(LL_INFO, ("status %d bytes %llu : cb=%p args=%p ud=%p", state->status, state->written, state->cb, state->args, ud));
			if (state->status == 200) {
				/* Report success only for HTTP 200 downloads */
				printf("OK: temp=%f pressure=%f wind=%f @ %d\n", currentConds.currentTemp, currentConds.pressure, currentConds.windSpeed, currentConds.windDir);

				state->cb("ok", state->status, state->written, state->args);
			} else {
				state->cb("fail", state->status, state->written, state->args);
			}
			if (state->fp != NULL) fclose(state->fp);
			free(state);
			break;
	}
}


#endif


//
// Get: ffi('int mgos_http_fetch         (char *, char *, void(*)(char *,       int,            int,       userdata), userdata)')
// typedef void (*mgos_http_fetch_cb_t)                          (char *result, int error_code, int bytes, void *ud);
//
int mgos_http_fetch(char *url, char *path, mgos_http_fetch_cb_t cb, void *args)
{
	struct state *state = NULL;
	FILE *fp = NULL;
	int rv = -1;
//	if (url == NULL || path == NULL) {
	if (url == NULL) {
		LOG(LL_WARN, ("=== url needed ==="));
		goto done;
	}

//	if (path != NULL && (fp = fopen(path, "w")) == NULL) {
//		LOG(LL_WARN, ("=== cannot open %s ===", path));
//		goto done;
//	}

	if ((state = calloc(1, sizeof(*state))) == NULL) {
		LOG(LL_WARN, ("=== cannot alloc %d ===", sizeof(*state)));
		goto done;
 	}

	state->fp = fp;
	state->written = 0;
	state->cb = cb;
	state->args = args;

	wug_client_init( &currentConds );

	LOG(LL_INFO, ("Start fetching %s to %s", url, path ? path : ""));
	if (!mg_connect_http(mgos_get_mgr(), http_cb, state, url, NULL, NULL)) {
		LOG(LL_WARN, ("=== malformed url %s ===", url));
		free(state);
		state = NULL;
		goto done;
	}

	rv = 0;

done:
	LOG(LL_INFO, ("Started '%s' -> '%s' ..", url ? url : "", path ? path : ""));
//	if (url) free(url);
//	if (path) free(path);
	if (NULL == state) cb("fail", 500, 0, args);
	return rv;
}


//
//
bool mgos_http_fetch_init(void) {	
	return true;
}
