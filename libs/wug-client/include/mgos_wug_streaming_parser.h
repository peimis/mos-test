#pragma once

#ifndef MOS_LIBS_MGOS_WUG_JSON_STREAMING_PARSER_H_
#define MOS_LIBS_MGOS_WUG_JSON_STREAMING_PARSER_H_

#include "JsonListener.h"
#include "mgos_wug_client.h"

#define maxKeyLen 31

enum decState_t {
	decoderStateInit = 0,
	decoderStateObservation,
	decoderStateForecast,
	decoderStateHourlyForecast,
};


class WugJsonListener: public JsonListener
{
	private:
		WUGConditions_t *conditions = nullptr;
		WUGForecast_t *forecast = nullptr;
		WUGHourly_t *hourly = nullptr;

		char *	currentKey = nullptr;
		char *	currentParent = nullptr;
		char *	currentArray = nullptr;
		int		currentArrayIndex = 0;
		int		currentArrayLevel = 0;
		boolean	inArray = false;

		//
		enum decState_t	decoderState = decoderStateInit;

		boolean isMetric = true;
		void insertParent(void);
		void replaceParent(void);

	public:
		WugJsonListener(boolean isMetric);

		WUGConditions_t *getConditions();
		WUGForecast_t 	*getForecast();
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