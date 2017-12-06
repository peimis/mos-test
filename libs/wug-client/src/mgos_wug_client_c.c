#include <stdbool.h>
#include "mgos.h"

#include "mgos_wug_client.h"
#include "mgos_json_streaming_parser.h"


// ------------------------------------------------------------------------------
//

static int fetch_state = 0;

struct http_get_state {
	mgos_wug_data_cb_t cb;			/* Callback function */
	WUGConditions_t *currentConditions;
	WUGHourly_t 	*hourlyForecast;
	void *args;						/* Callback args */
	int status;                     /* Request status */
	int64_t written;                /* Number of bytes written */
};


// ------------------------------------------------------------------------------
//
static void http_cb(struct mg_connection *c, int ev, void *ev_data, void *ud);


// ------------------------------------------------------------------------------
//
char *wug_get_url(void)
{
	int i, len=0;
	const char * const url_strings[] = {
		"http://api.wunderground.com/api",
		mgos_sys_config_get_wug_apikey(),
		mgos_sys_config_get_wug_features(),
		mgos_sys_config_get_wug_options(),
		"q",
		mgos_sys_config_get_wug_location(),
	};

	const int strings = (sizeof(url_strings)/sizeof(*url_strings));
	printf("Paste %d items\n", strings);
	for (i=0; i<strings; i++) {
		if (url_strings[i]) {
			int l = strlen(url_strings[i]);
			printf("string '%s'\n", url_strings[i]);
			len += l;
			printf("len %d -> %d\n", l, len);
		}
	}
	len += 6+strings; // make some room for slashes and '.json'

	char *url = (char*)calloc(1, 1+len );

	char *ptr = url;
	for (i=0; i<strings; i++) {
		if (url_strings[i]) {
			int l = strlen(url_strings[i]);
			printf("len %d -> %d\n", l, len);
			memcpy(ptr, url_strings[i], l);
			ptr += l;
			*ptr++ = '/';
		}
	}
	ptr--;
	strcpy(ptr, ".json");

	// http_url = 'http://api.wunderground.com/api/' + apikey + '/' + features + '/' + options + 'q/' + location + '.json';

	printf("URL = '%s' len=%d %d\n", url, strlen(url), len);

	return url;
}


// ====================================================================================
// ffi:ed C functions
//
// ------------------------------------------------------------------------------
//
static void http_cb(struct mg_connection *c, int ev, void *ev_data, void *args)
{
	struct http_message *hm = (struct http_message *) ev_data;
	struct http_get_state *state = (struct http_get_state *) args;

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

//			printf("chunk %d / %d\n", hm->body.len, (int)state->written);

			if (0 == fetch_state) {
				n = hm->body.len;
				char * buf = (char *)calloc(1, n);
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
			LOG(LL_INFO, ("status %d bytes %llu : cb=%p args=%p ud=%p", state->status, state->written, state->cb, state->args, args));
			if (state->status == 200) {
				int i;
				/* Report success only for HTTP 200 downloads */
				char *resp = (char *)calloc(1, 100);
				sprintf(resp, "{\"temp\":%.2f, \"pressure\":%.2f \"wind\":%.2f \"winddir\":%d}", state->currentConditions->currentTemp, 
					state->currentConditions->pressure, state->currentConditions->windSpeed, state->currentConditions->windDir);
				printf("OK : '%s'\n", resp);

				for (i=0; i<24; i++) {
					WUGHourlyItem_t *item = &state->hourlyForecast->data[i];

					time_t now = item->time; // TZ=GMT+1
					struct tm* tm_info = gmtime(&now);

					printf("%d.%d @ %d : %d C %f kPa, %d m/s @ %d\n", tm_info->tm_mday, 1+tm_info->tm_mon, tm_info->tm_hour, item->temp, item->pressure, item->windSpeed, item->windDir);
				}

				state->cb(state->status, resp, state->args);
				free(resp);
			} else {
				state->cb(state->status, NULL, state->args);
			}

			free(state);
			break;
	}
}


// ------------------------------------------------------------------------------
//
// typedef int (*mgos_wug_data_cb_t)(int status, void *result_struct, void *args);
//
int mgos_wug_get_conditions(int data, mgos_wug_data_cb_t cb, void *args)
{
	struct http_get_state *state = NULL;
	int rv = -1;

	char *url = wug_get_url();
	(void)data;

	if (url == NULL) {
		LOG(LL_WARN, ("=== url needed ==="));
		goto done;
	}

	if ((state = (struct http_get_state *)calloc(1, sizeof(*state))) == NULL) {
		LOG(LL_WARN, ("=== cannot alloc %d ===", sizeof(*state)));
		goto done;
	}

	state->written = 0;
	state->cb = cb;
	state->args = args;

	wug_client_init();
	state->currentConditions = wug_client_get_conditions_data();
	state->hourlyForecast = wug_client_get_hourly_forecast_data();

	LOG(LL_INFO, ("Start fetching from '%s'", url ));
	if (!mg_connect_http(mgos_get_mgr(), http_cb, state, url, NULL, NULL)) {
		LOG(LL_WARN, ("=== malformed url %s ===", url));
		free(state);
		state = NULL;
		goto done;
	}

	rv = 0;

done:
	LOG(LL_INFO, ("Started to GET '%s' ..", url ? url : ""));
	if (url) free(url);
	if (NULL == state) cb(500, NULL, args);

	return rv;
}


// ------------------------------------------------------------------------------
//
bool mgos_wug_client_init(void)
{
	return true;
}
