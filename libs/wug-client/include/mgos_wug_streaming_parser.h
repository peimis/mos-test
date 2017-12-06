#pragma once

#ifndef MOS_LIBS_MGOS_WUG_JSON_STREAMING_PARSER_H_
#define MOS_LIBS_MGOS_WUG_JSON_STREAMING_PARSER_H_

#include "JsonListener.h"
#include "mgos_wug_client.h"

class WugJsonListener: public JsonListener
{
	private:
		WUGConditions_t *conditions = nullptr;
		WUGHourly_t *hourly = nullptr;

		String	currentKey;
		String	currentParent;
		String	currentState;

		//
		boolean	inCurrentObservation = false;
		boolean	inHourlyForecast = false;
		boolean	inForecast= false;

		boolean	inArray = false;
		boolean isMetric = true;
		
	public:
		WugJsonListener(boolean isMetric);

		WUGConditions_t *getConditions();
		WUGHourly_t     *getHourlyForecast();

		float getCurrentTemp();
		float getCurrentPressure();
		float getCurrentWindSpeed();
		int getCurrentWindDir();

		void hourlyForecastValue(String value);
		void forecastValue(String value);
		void currentObsevationValue(String value);


		virtual void whitespace(char c);
	  
		virtual void startDocument();

		virtual void key(String key);

		virtual void value(String value);

		virtual void endArray();

		virtual void endObject();

		virtual void endDocument();

		virtual void startArray();

		virtual void startObject();
};

#endif