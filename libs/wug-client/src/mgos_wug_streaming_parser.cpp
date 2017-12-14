#include "mgos.h"

#include "mgos_json_streaming_parser.h"
#include "mgos_wug_streaming_parser.h"
#include "mgos_wug_client.h"


WugJsonListener * wug_listener = new WugJsonListener(true);

static struct parent_key_breadcrumbs crumbs;


// -----------------------------------------------------------------------
//
WugJsonListener::WugJsonListener(boolean _isMetric) {
	isMetric = _isMetric;

	hourly = (WUGHourly_t *)calloc(1, sizeof(WUGHourly_t));
	if (nullptr == hourly) {
		LOG(LL_INFO, ("Cannot allocate hourly data"));
	}
	conditions = (WUGConditions_t *)calloc(1, sizeof(WUGConditions_t));
	if (nullptr == conditions) {
		LOG(LL_INFO, ("Cannot allocate conditions data"));
	}
	forecast = (WUGForecast_t *)calloc(1, sizeof(WUGForecast_t));
	if (nullptr == forecast) {
		LOG(LL_INFO, ("Cannot allocate forecast data"));
	}

	currentKey = (char *)calloc(1, maxKeyLen+1);
	currentParent = (char *)calloc(1, maxKeyLen+1);
	currentArray = (char *)calloc(1, maxKeyLen+1);
}


// -----------------------------------------------------------------------
//
void WugJsonListener::hourlyForecastValue(String value)
{
	if (!strcmp(currentParent, "FCTTIME")) {
		if (!strcmp(currentKey, "epoch")) {
			hourly->data[ currentArrayIndex ].time = value.toInt();
			LOG(LL_INFO, ("New epoch time @ %d = %u", currentArrayIndex, hourly->data[ currentArrayIndex ].time));
		}
	}
	else if (!strcmp(currentParent, "temp")) {
		if (isMetric && !strcmp(currentKey, "metric")) {
			hourly->data[ currentArrayIndex ].temp = value.toInt();
		}
		else if (!isMetric && !strcmp(currentKey, "english")) {
			hourly->data[ currentArrayIndex ].temp = value.toInt();
		}
	}
	else if (!strcmp(currentParent,  "dewpoint")) {
		if (isMetric && !strcmp(currentKey, "metric")) {
			hourly->data[ currentArrayIndex ].dewPoint = value.toInt();
		}
		else if (!isMetric && !strcmp(currentKey, "english")) {
			hourly->data[ currentArrayIndex ].dewPoint = value.toInt();
		}
	}
	else if (!strcmp(currentParent,  "wspd")) {
		if (isMetric && !strcmp(currentKey, "metric")) {
			hourly->data[ currentArrayIndex ].windSpeed = value.toInt();
		}
		else if (!isMetric && !strcmp(currentKey, "english")) {
			hourly->data[ currentArrayIndex ].windSpeed = value.toInt();
		}
	}
	else if (!strcmp(currentParent, "wdir")) {
		if (!strcmp(currentKey, "degrees")) {
			hourly->data[ currentArrayIndex ].windDir = value.toInt();
		}
	}
	else if (!strcmp(currentParent, "windchill")) {
		if (isMetric && !strcmp(currentKey, "metric")) {
			hourly->data[ currentArrayIndex ].windchill = value.toInt();
		}
		else if (!isMetric && !strcmp(currentKey, "english")) {
			hourly->data[ currentArrayIndex ].windchill = value.toInt();
		}
	}
	else if (!strcmp(currentParent, "feelslike")) {
		if (isMetric && !strcmp(currentKey, "metric")) {
			hourly->data[ currentArrayIndex ].feelslike = value.toInt();
		}
		else if (!isMetric && !strcmp(currentKey, "english")) {
			hourly->data[ currentArrayIndex ].feelslike = value.toInt();
		}
	}
	else if (!strcmp(currentParent, "qpf")) {
		if (isMetric && !strcmp(currentKey, "metric")) {
			hourly->data[ currentArrayIndex ].qpf = value.toFloat();
		}
		else if (!isMetric && !strcmp(currentKey, "english")) {
			hourly->data[ currentArrayIndex ].qpf = value.toFloat();
		}
	}
	else if (!strcmp(currentParent, "snow")) {
		if (isMetric && !strcmp(currentKey, "metric")) {
			hourly->data[ currentArrayIndex ].snow = value.toFloat();
		}
		else if (!isMetric && !strcmp(currentKey, "english")) {
			hourly->data[ currentArrayIndex ].snow = value.toFloat();
		}
	}
	else if (!strcmp(currentParent, "mslp")) {
		if (isMetric && !strcmp(currentKey, "metric")) {
			hourly->data[ currentArrayIndex ].pressure = value.toFloat();
		}
		else if (!isMetric && !strcmp(currentKey, "english")) {
			hourly->data[ currentArrayIndex ].pressure = value.toFloat();
		}
	}
	else if (!strncmp(currentParent, "hourly_forecast", strlen("hourly_forecast"))) {
		if (!strcmp(currentKey, "icon")) {
			printf("got icon '%s' for index %d\n", value.c_str(), currentArrayIndex);

			if (hourly->data[ currentArrayIndex ].hourlyIcon) {
				free(hourly->data[ currentArrayIndex ].hourlyIcon);
			}
			hourly->data[ currentArrayIndex ].hourlyIcon = (char *)calloc(1, 1+strlen( value.c_str() ));
			strcpy(hourly->data[ currentArrayIndex ].hourlyIcon, value.c_str() );
//			memcpy(hourly->data[ currentArrayIndex ].hourlyIcon, value.c_str(), strlen( value.c_str() ));
		}
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
	if (!strcmp(currentKey, "temp_c") && isMetric) {
	    conditions->currentTemp = value.toFloat();
	}

	if (!strcmp(currentKey, "temp_f") && !isMetric) {
	    conditions->currentTemp = value.toFloat();
	}


	// -----------------------------------------------
	// Current windspeed
	if (!strcmp(currentKey, "wind_kph") && isMetric) {
	    conditions->windSpeed = value.toFloat();
	}

	if (!strcmp(currentKey, "wind_mph") && !isMetric) {
	    conditions->windSpeed = value.toFloat();
	}

	// -----------------------------------------------
	// Current wind direction 0..359 deg
	if (!strcmp(currentKey, "wind_degrees")) {
	    conditions->windDir = value.toInt();
	}

	// -----------------------------------------------
	// Current humidity
	if (!strcmp(currentKey, "relative_humidity")) {
	    conditions->humidity = value.toInt();
	}

	// -----------------------------------------------
	// Current pressure
	if (!strcmp(currentKey, "pressure_mb") && isMetric) {
	    conditions->pressure = value.toFloat();
	}

	if (!strcmp(currentKey, "pressure_in") && !isMetric) {
	    conditions->pressure = value.toFloat();
	}

	// -----------------------------------------------
	// Current dew point
	if (!strcmp(currentKey, "dewpoint_c") && isMetric) {
	    conditions->dewPoint = value.toFloat();
	}

	if (!strcmp(currentKey, "dewpoint_f") && !isMetric) {
	    conditions->dewPoint = value.toFloat();
	}

	// -----------------------------------------------
	// Current precipitation
	if (!strcmp(currentKey, "precip_1hr_metric") && isMetric) {
	    conditions->precipitation1hr = value.toFloat();
	}

	if (!strcmp(currentKey, "precip_1hr_in") && !isMetric) {
	    conditions->precipitation1hr = value.toFloat();
	}

	// -----------------------------------------------
	// Precipitation today
	if (!strcmp(currentKey, "precip_today_metric") && isMetric) {
	    conditions->precipitationToday = value.toFloat();
	}

	if (!strcmp(currentKey, "precip_today_in") && !isMetric) {
	    conditions->precipitationToday = value.toFloat();
	}

	// -----------------------------------------------
	// Feels like
	if (!strcmp(currentKey, "feelslike_c") && isMetric) {
	    conditions->feelslike = value.toFloat();
	}

	if (!strcmp(currentKey, "feelslike_f") && !isMetric) {
	    conditions->feelslike = value.toFloat();
	}


	// -----------------------------------------------
	// UV Index
	if (!strcmp(currentKey, "UV")) {
	    conditions->UV = value.toInt();
	}

	// -----------------------------------------------
	// Observation Time
	if (!strcmp(currentKey, "observation_epoch")) {
	    conditions->observationTime = value.toInt();
	}

	// -----------------------------------------------
	// Local time when data is fetched
	if (!strcmp(currentKey, "local_epoch")) {
	    conditions->localTime = value.toInt();
	}

	// -----------------------------------------------
	// Icon URL & name
	if (!strcmp(currentKey, "icon_url")) {
		if (conditions->weatherIconURL) {
			free(conditions->weatherIconURL);
		}
	    conditions->weatherIconURL = (char *)calloc(1, 1+strlen( value.c_str() ));
	    memcpy(conditions->weatherIconURL, value.c_str(), strlen( value.c_str() ));
	}

	if (!strcmp(currentKey, "icon")) {
		if (conditions->weatherIcon) {
			free(conditions->weatherIcon);
		}
	    conditions->weatherIcon = (char *)calloc(1, 1+strlen( value.c_str() ));
	    memcpy(conditions->weatherIcon, value.c_str(), strlen( value.c_str() ));
	}

	// -----------------------------------------------
	// Local time when data is fetched
	if (!strcmp(currentKey, "weather")) {
		if (conditions->weatherText) {
			free(conditions->weatherText);
		}
	    conditions->weatherText = (char *)calloc(1, 1+strlen( value.c_str() ));
	    memcpy(conditions->weatherText, value.c_str(), strlen( value.c_str() ));
	}
}


// -----------------------------------------------------------------------
//
void WugJsonListener::key(String keyString)
{
	enum decState_t newState = decoderState;

	strncpy(currentKey, keyString.c_str(), maxKeyLen);

	if (strlen(keyString.c_str()) >= maxKeyLen) {
		LOG(LL_WARN, ("too long key '%s'", keyString.c_str()));
	}
//	LOG(LL_INFO, ("'%s'", currentKey));

	if (!strcmp(currentKey, "current_observation")) {
		newState = decoderStateObservation;
	}
	else if (!strcmp(currentKey, "hourly_forecast")) {
		newState = decoderStateHourlyForecast;
		currentArrayIndex = 0;
	}
	else if (!strcmp(currentKey, "forecast")) {
		newState = decoderStateForecast;
	}

	if (newState != decoderState) {
		decoderState = newState;
		LOG(LL_WARN, ("** Got key '%s' state=%d", currentKey, decoderState));
	}
}


// -----------------------------------------------------------------------
//
void WugJsonListener::value(String value)
{
	switch (decoderState)
	{
		case decoderStateObservation:
			currentObsevationValue(value);
		break;

		case decoderStateForecast:
			forecastValue(value);
		break;

		case decoderStateHourlyForecast:
			hourlyForecastValue(value);
		break;

		default:
			LOG(LL_WARN, ("Got key '%s' = '%s' in state %d", strlen(currentKey) ? currentKey : "", value.c_str(), decoderState));
		break;
	}
}


void WugJsonListener::insertParent()
{
	struct parent_key *parent = (struct parent_key *)calloc(1, sizeof(struct parent_key));
	parent->name = (char *)calloc(1, 1+strlen(currentKey));
	strcpy(parent->name, currentKey);
	strcpy(currentParent, currentKey);

	SLIST_INSERT_HEAD(&crumbs.breadcrumbs, parent, entries);
	crumbs.count++;
//	printf("Objects : ");
//	SLIST_FOREACH(parent, &crumbs.breadcrumbs, entries) {
//		printf("'%s' -> ", parent->name ? parent->name : "??" );
//	}
//	printf("\n");

}


void WugJsonListener::replaceParent()
{
	struct parent_key *parent;

    if( !SLIST_EMPTY(&crumbs.breadcrumbs)) {
		crumbs.count--;
		parent = SLIST_FIRST(&crumbs.breadcrumbs);
		if (parent->name) {
//			LOG(LL_INFO, ("Drop '%s'", parent->name));
			free(parent->name);
			parent->name = NULL;
			}
		SLIST_REMOVE_HEAD(&crumbs.breadcrumbs, entries);
		if (parent) {
			free(parent);
			parent = NULL;
		}
		if (!SLIST_EMPTY(&crumbs.breadcrumbs)) {
			parent = SLIST_FIRST(&crumbs.breadcrumbs);
//			LOG(LL_INFO, ("prev parent: '%s'", parent->name ? parent->name : "??"));
			strcpy(currentParent, parent->name);
		}
	} else  {
		LOG(LL_WARN, ("List empty"));
	}
}

// -----------------------------------------------------------------------
//
void WugJsonListener::startObject()
{
//	LOG(LL_INFO, ("prev key='%s' parent='%s' arr='%s' index=%d\n", currentKey, currentParent, currentArray, currentArrayIndex));

	if (inArray) {
		if (0 == currentArrayLevel) {
			strncpy(currentKey, currentArray, maxKeyLen);
			sprintf(&currentKey[strlen(currentArray)], "[%d]", currentArrayIndex);
//			LOG(LL_INFO, ("start index %d in array '%s' -> '%s'", currentArrayIndex, currentArray, currentKey));
		}
		currentArrayLevel++;
	}

	insertParent();
}


// -----------------------------------------------------------------------
//
void WugJsonListener::endObject()
{
	if (inArray) {
		currentArrayLevel--;
	}
	if (0 == currentArrayLevel) {
//		LOG(LL_INFO, ("end index %d in array '%s'", currentArrayIndex, currentArray));
		currentArrayIndex++;
		if (currentArrayIndex > (sizeof(hourly->data) / sizeof(hourly->data[0])) ) {
			LOG(LL_WARN, ("currentArrayIndex overflow @ %d", currentArrayIndex));
			currentArrayIndex = (sizeof(hourly->data) / sizeof(hourly->data[0]));
		}
	}

	replaceParent();
}


// -----------------------------------------------------------------------
//
void WugJsonListener::startArray() {
	if (inArray) {
		LOG(LL_ERROR, ("Nested arrays not implemented"));
	}
	inArray = true;
	currentArrayIndex = 0;
	currentArrayLevel = 0;

	strcpy(currentArray, currentKey);
	LOG(LL_INFO, (" == '%s'", currentArray));

	insertParent();
}


void WugJsonListener::endArray() {
	inArray = false;
	LOG(LL_INFO, (" == '%s'", currentArray));
	replaceParent();
	currentArray[0] = '\0';
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
//
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

WUGConditions_t * WugJsonListener::getConditions()
{
	return conditions;
}

WUGHourly_t *WugJsonListener::getHourlyForecast()
{
	return hourly;
}


// -----------------------------------------------------------------------
//
WUGConditions_t * wug_client_get_conditions_data(void)
{
	return wug_listener->getConditions();
}

WUGHourly_t * wug_client_get_hourly_forecast_data(void)
{
	return wug_listener->getHourlyForecast();
}



// -----------------------------------------------------------------------
//
void wug_client_init(void)
{
	streaming_parser_set_listener(wug_listener);
}
