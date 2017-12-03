#include "mgos_json_streaming_parser.h"
#include "mgos_wug_streaming_parser.h"
#include "mgos_wug_client.h"

#include "mgos.h"

WugJsonListener * wug_listener = new WugJsonListener(true);

struct http_get_state {
	mgos_wug_data_cb_t cb;			/* Callback function */
	WUGConditions_t *conds;
	void *args;						/* Callback args */
	int status;                     /* Request status */
	int64_t written;                /* Number of bytes written */
};



static void http_cb(struct mg_connection *c, int ev, void *ev_data, void *ud);


// -----------------------------------------------------------------------
//
WugJsonListener::WugJsonListener(boolean _isMetric) {
	isMetric = _isMetric;

	hourly = (WUGHourly_t *)calloc(1, sizeof(WUGHourly_t));
	if (nullptr == hourly) {
		LOG(LL_INFO, ("Cannot allocate hourly data"));
	}
}


// -----------------------------------------------------------------------
//
void WugJsonListener::key(String keystring) {
//	const char *c_str = keystring.c_str();

	currentKey = String(keystring);

	if (currentKey == "current_observation") {
		inCurrentObservation = true;
		inHourlyForecast = false;
		inForecast = false;
	}
	else if (currentKey == "hourly_forecast") {
		inCurrentObservation = false;
		inHourlyForecast = true;
		inForecast = false;
	}
	else if (currentKey == "forecast") {
		inCurrentObservation = false;
		inHourlyForecast = false;
		inForecast = true;
	}

//	printf("'%s' : ", c_str);
}

// -----------------------------------------------------------------------
//
void WugJsonListener::hourlyForecastValue(String value)
{
	if (currentParent == "FCTTIME") {
		if (currentKey == "epoch") {
		}
	}
	else if (currentParent == "temp") {
		
	}
	else if (currentParent == "dewpoint") {
		
	}
	else if (currentParent == "wspd") {
		
	}
	else if (currentParent == "wdir") {
		
	}
	else if (currentParent == "windchill") {
		
	}
	else if (currentParent == "feelslike") {
		
	}
	else if (currentParent == "qpf") {
		
	}
	else if (currentParent == "snow") {
		
	}
	else if (currentParent == "mslp") {
		
	}
	else if (currentParent == "wdir") {
		
	}
	else if (currentParent == "wdir") {
		
	}
}

// -----------------------------------------------------------------------
//
void WugJsonListener::forecastValue(String value)
{
}

// -----------------------------------------------------------------------
//
void WugJsonListener::currentObsevationValue(String value)
{


	// -----------------------------------------------
	// Current temperature
	if (currentKey == "temp_c" && isMetric) {
	    conditions->currentTemp = value.toFloat();
	}

	if (currentKey == "temp_f" && !isMetric) {
	    conditions->currentTemp = value.toFloat();
	}


	// -----------------------------------------------
	// Current windspeed
	if (currentKey == "wind_kph" && isMetric) {
	    conditions->windSpeed = value.toFloat();
	}

	if (currentKey == "wind_mph" && !isMetric) {
	    conditions->windSpeed = value.toFloat();
	}

	// -----------------------------------------------
	// Current wind direction 0..359 deg
	if (currentKey == "wind_degrees") {
	    conditions->windDir = value.toInt();
	}

	// -----------------------------------------------
	// Current humidity
	if (currentKey == "relative_humidity") {
	    conditions->humidity = value.toInt();
	}

	// -----------------------------------------------
	// Current pressure
	if (currentKey == "pressure_mb" && isMetric) {
	    conditions->pressure = value.toFloat();
	}

	if (currentKey == "pressure_in" && !isMetric) {
	    conditions->pressure = value.toFloat();
	}

	// -----------------------------------------------
	// Current dew point
	if (currentKey == "dewpoint_c" && isMetric) {
	    conditions->dewPoint = value.toFloat();
	}

	if (currentKey == "dewpoint_f" && !isMetric) {
	    conditions->dewPoint = value.toFloat();
	}

	// -----------------------------------------------
	// Current precipitation
	if (currentKey == "precip_1hr_metric" && isMetric) {
	    conditions->precipitation1hr = value.toFloat();
	}

	if (currentKey == "precip_1hr_in" && !isMetric) {
	    conditions->precipitation1hr = value.toFloat();
	}

	// -----------------------------------------------
	// Precipitation today
	if (currentKey == "precip_today_metric" && isMetric) {
	    conditions->precipitationToday = value.toFloat();
	}

	if (currentKey == "precip_today_in" && !isMetric) {
	    conditions->precipitationToday = value.toFloat();
	}

	// -----------------------------------------------
	// Feels like
	if (currentKey == "feelslike_c" && isMetric) {
	    conditions->feelslike = value.toFloat();
	}

	if (currentKey == "feelslike_f" && !isMetric) {
	    conditions->feelslike = value.toFloat();
	}


	// -----------------------------------------------
	// UV Index
	if (currentKey == "UV") {
	    conditions->UV = value.toInt();
	}

	// -----------------------------------------------
	// Observation Time
	if (currentKey == "observation_epoch") {
	    conditions->observationTime = value.toInt();
	}

	// -----------------------------------------------
	// Local time when data is fetched
	if (currentKey == "local_epoch") {
	    conditions->localTime = value.toInt();
	}

	// -----------------------------------------------
	// Icon URL & name
	if (currentKey == "icon_url") {
		if (conditions->weatherIconURL) {
			free(conditions->weatherIconURL);
		}
	    conditions->weatherIconURL = (char *)calloc(1, 1+strlen( value.c_str() ));
	    memcpy(conditions->weatherIconURL, value.c_str(), strlen( value.c_str() ));
	}

	if (currentKey == "icon") {
		if (conditions->weatherIcon) {
			free(conditions->weatherIcon);
		}
	    conditions->weatherIcon = (char *)calloc(1, 1+strlen( value.c_str() ));
	    memcpy(conditions->weatherIcon, value.c_str(), strlen( value.c_str() ));
	}

	// -----------------------------------------------
	// Local time when data is fetched
	if (currentKey == "weather") {
		if (conditions->weatherText) {
			free(conditions->weatherText);
		}
	    conditions->weatherText = (char *)calloc(1, 1+strlen( value.c_str() ));
	    memcpy(conditions->weatherText, value.c_str(), strlen( value.c_str() ));
	}
}




// -----------------------------------------------------------------------
//
void WugJsonListener::value(String value) {
//	const char *c_str = value.c_str();
//	printf("'%s',\n", c_str);

	if (inCurrentObservation) currentObsevationValue(value);
	else if (inHourlyForecast) hourlyForecastValue(value);
	else if (inForecast) forecastValue(value);
	else {
		LOG(LL_INFO, ("No use for value '%s'", value.c_str()));
	}
}


// -----------------------------------------------------------------------
//
void WugJsonListener::startObject() {
	currentParent = currentKey;
	printf("'%s' { ", currentKey.c_str() );
}

void WugJsonListener::endObject() {
	printf(" }\n");

	currentParent = "";
}

void WugJsonListener::startArray() {
	inArray = true;
	printf(" [ ");
}


void WugJsonListener::endArray() {
	inArray = false;
	printf(" ] \n");
}


// -----------------------------------------------------------------------
//
void WugJsonListener::startDocument() {
	printf("\n === start document.\n");
}

void WugJsonListener::endDocument() {
	printf("\n === end document.\n");
}

// -----------------------------------------------------------------------
//
void WugJsonListener::whitespace(char c) {
	printf(" (whitespace) ");
}


// -----------------------------------------------------------------------

float WugJsonListener::getCurrentTemp(void)
{
	return conditions->currentTemp;
}

float WugJsonListener::getCurrentPressure(void)
{
	return conditions->pressure;
}


float WugJsonListener::getCurrentWindSpeed(void)
{
	return conditions->windSpeed;
}

int WugJsonListener::getCurrentWindDir(void)
{
	return conditions->windDir;
}


void WugJsonListener::setConditions(WUGConditions_t * _conditions)
{
	conditions = _conditions;
}


// ====================================================================================
// ffi:ed C functions
//


//
//
void wug_client_set_conditions(WUGConditions_t *conditions)
{
	wug_listener->setConditions(conditions);
}


//
//
void wug_client_init(WUGConditions_t *conditions)
{
	wug_listener->setConditions(conditions);
	streaming_parser_set_listener(wug_listener);
}


//
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

	if((state->conds = (WUGConditions_t *)calloc(1, sizeof(WUGConditions_t))) == NULL )
	{
		LOG(LL_WARN, ("=== cannot alloc %d ===", sizeof(WUGConditions_t)));
		goto done;		
	}

	state->written = 0;
	state->cb = cb;
	state->args = args;

	wug_client_init( state->conds );

	LOG(LL_INFO, ("Start fetching from '%s'", url ));
	if (!mg_connect_http(mgos_get_mgr(), http_cb, state, url, NULL, NULL)) {
		LOG(LL_WARN, ("=== malformed url %s ===", url));
		free(state);
		state = NULL;
		free(state->conds);
		goto done;
	}

	rv = 0;

done:
	LOG(LL_INFO, ("Started to GET '%s' ..", url ? url : ""));
	if (url) free(url);
	if (NULL == state) cb(500, NULL, args);

	return rv;
}


static int fetch_state = 0;

//
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

			printf("chunk %d / %d\n", hm->body.len, (int)state->written);

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
				/* Report success only for HTTP 200 downloads */
				char *resp = (char *)calloc(1, 100);
				sprintf(resp, "{\"temp\":%.2f, \"pressure\":%.2f \"wind\":%.2f \"winddir\":%d}", state->conds->currentTemp, 
					state->conds->pressure, state->conds->windSpeed, state->conds->windDir);
				printf("OK : '%s'\n", resp);
				state->cb(state->status, resp, state->args);
				free(resp);
			} else {
				state->cb(state->status, NULL, state->args);
			}

			free(state->conds);
			free(state);
			break;
	}
}
