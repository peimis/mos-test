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
}


// -----------------------------------------------------------------------
//
void WugJsonListener::key(String keystring)
{

	currentKey = String(keystring);

	if (currentKey == "current_observation") {
		inCurrentObservation = true;
		inHourlyForecast = false;
		inForecast = false;
		LOG(LL_INFO, ("Starting '%s'", currentKey.c_str()));
	}
	else if (currentKey == "hourly_forecast") {
		inCurrentObservation = false;
		inHourlyForecast = true;
		inForecast = false;
		hourly->currentIndex = -1;
		LOG(LL_INFO, ("Starting '%s'", currentKey.c_str()));

	}
	else if (currentKey == "forecast") {
		inCurrentObservation = false;
		inHourlyForecast = false;
		inForecast = true;
		LOG(LL_INFO, ("Starting '%s'", currentKey.c_str()));
	}

//	printf("'%s' : ", c_str);
}

// -----------------------------------------------------------------------
//
void WugJsonListener::hourlyForecastValue(String value)
{
	if (currentParent == "FCTTIME") {
		if (currentKey == "epoch") {
			hourly->currentIndex++;
			if (hourly->currentIndex > 23) {
				hourly->currentIndex = 0;
				LOG(LL_WARN, ("Current index overflow"));
			}
			hourly->data[ hourly->currentIndex ].time = value.toInt();
		}
	}
	else if (currentParent == "temp") {
		if (isMetric && currentKey == "metric") {
			hourly->data[ hourly->currentIndex ].temp = value.toInt();
		}
		else if (!isMetric && currentKey == "english") {
			hourly->data[ hourly->currentIndex ].temp = value.toInt();
		}
	}
	else if (currentParent == "dewpoint") {
		if (isMetric && currentKey == "metric") {
			hourly->data[ hourly->currentIndex ].dewPoint = value.toInt();
		}
		else if (!isMetric && currentKey == "english") {
			hourly->data[ hourly->currentIndex ].dewPoint = value.toInt();
		}
	}
	else if (currentParent == "wspd") {
		if (isMetric && currentKey == "metric") {
			hourly->data[ hourly->currentIndex ].windSpeed = value.toInt();
		}
		else if (!isMetric && currentKey == "english") {
			hourly->data[ hourly->currentIndex ].windSpeed = value.toInt();
		}
	}
	else if (currentParent == "wdir") {
		if (currentKey == "degrees") {
			hourly->data[ hourly->currentIndex ].windDir = value.toInt();
		}
	}
	else if (currentParent == "windchill") {
		if (isMetric && currentKey == "metric") {
			hourly->data[ hourly->currentIndex ].windchill = value.toInt();
		}
		else if (!isMetric && currentKey == "english") {
			hourly->data[ hourly->currentIndex ].windchill = value.toInt();
		}
	}
	else if (currentParent == "feelslike") {
		if (isMetric && currentKey == "metric") {
			hourly->data[ hourly->currentIndex ].feelslike = value.toInt();
		}
		else if (!isMetric && currentKey == "english") {
			hourly->data[ hourly->currentIndex ].feelslike = value.toInt();
		}
	}
	else if (currentParent == "qpf") {
		if (isMetric && currentKey == "metric") {
			hourly->data[ hourly->currentIndex ].qpf = value.toFloat();
		}
		else if (!isMetric && currentKey == "english") {
			hourly->data[ hourly->currentIndex ].qpf = value.toFloat();
		}
	}
	else if (currentParent == "snow") {
		if (isMetric && currentKey == "metric") {
			hourly->data[ hourly->currentIndex ].snow = value.toFloat();
		}
		else if (!isMetric && currentKey == "english") {
			hourly->data[ hourly->currentIndex ].snow = value.toFloat();
		}
	}
	else if (currentParent == "mslp") {
		if (isMetric && currentKey == "metric") {
			hourly->data[ hourly->currentIndex ].pressure = value.toFloat();
		}
		else if (!isMetric && currentKey == "english") {
			hourly->data[ hourly->currentIndex ].pressure = value.toFloat();
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
void WugJsonListener::value(String value)
{
	if (inCurrentObservation) currentObsevationValue(value);
	else if (inHourlyForecast) hourlyForecastValue(value);
	else if (inForecast) forecastValue(value);
	else {
		LOG(LL_INFO, ("No use for value '%s' : '%s'", currentKey ? currentKey.c_str() : "", value.c_str()));
	}
}


// -----------------------------------------------------------------------
//
void WugJsonListener::startObject()
{
	struct parent_key *parent = (struct parent_key *)calloc(1, sizeof(struct parent_key));
	parent->name = (char *)calloc(1, 1+strlen(currentKey.c_str()));
	strcpy(parent->name, currentKey.c_str());
	currentParent = currentKey;

	SLIST_INSERT_HEAD(&crumbs.breadcrumbs, parent, entries);
	crumbs.count++;

	SLIST_FOREACH(parent, &crumbs.breadcrumbs, entries) {
		printf("'%s' -> ", parent->name ? parent->name : "-" );
	}
//	printf("'%s' { ", currentKey.c_str() );
}


// -----------------------------------------------------------------------
//
void WugJsonListener::endObject()
{
	struct parent_key *parent;

    if( !SLIST_EMPTY(&crumbs.breadcrumbs)) {
		crumbs.count--;
		parent = SLIST_FIRST(&crumbs.breadcrumbs);
		if (parent->name) {
			LOG(LL_INFO, ("Remove '%s'", parent->name));
			free(parent->name);
			}
		SLIST_REMOVE_HEAD(&crumbs.breadcrumbs, entries);
		if (parent) {
			free(parent);
		}
		if (!SLIST_EMPTY(&crumbs.breadcrumbs)) {
			parent = SLIST_FIRST(&crumbs.breadcrumbs);
			printf("next head is '%s'\n", parent->name ? parent->name : "?" );
		}
		currentParent = "";
	} else  {
		printf("List empty\n");
		currentParent = "";
	}
}

// -----------------------------------------------------------------------
//
void WugJsonListener::startArray() {
	inArray = true;
//	printf(" [ ");
}

void WugJsonListener::endArray() {
	inArray = false;
//	printf(" ] \n");
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
