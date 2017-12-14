//
//
#ifndef MOS_LIBS_MGOS_WUG_CLIENT_H_
#define MOS_LIBS_MGOS_WUG_CLIENT_H_

typedef struct WUGConditions {
	float		currentTemp;
	float		windSpeed;
	int			windDir;
	float		humidity;
	float		pressure;
	float		dewPoint;
	float		precipitationToday;
	float		precipitation1hr;
	float		feelslike;
	int			UV;
	uint32_t	observationTime;
	uint32_t	localTime;
	char		*weatherIcon;
	char		*weatherIconURL;
	char		*weatherText;
} WUGConditions_t;


typedef struct WUGForecast {
	int		temp;
	int		dewPoint;
	int		windSpeed;
} WUGForecast_t;

typedef struct WUGHourlyItem {
	int		temp;
	int		dewPoint;
	int		windSpeed;
	int		windDir;
	int		windchill;
	int		sky;
	int		humidity;
	int		fctcode;
	int		UV;
	float	qpf;
	float   snow;
	float   pressure;
	float   feelslike;
	uint32_t	time;
	char    *hourlyIcon;
	char    *hourlyCondition;
} WUGHourlyItem_t;


typedef struct WUGHourly {
	WUGHourlyItem_t	data[36];
	uint32_t		forecastTime;
} WUGHourly_t;


// breadcrumbs for the current key
//
struct parent_key
{
	char *name;
	SLIST_ENTRY(parent_key) entries;
};


struct parent_key_breadcrumbs
{
	int		count;
	int		hasArray;
	SLIST_HEAD(breadcrumbs, parent_key) breadcrumbs;
};




/* FFI-able signature of the function that receives response to a request. */
typedef int (*mgos_wug_data_cb_t)(int status, char *result, void *args);

#ifdef __cplusplus
extern "C" {
#endif

void wug_client_init(void);
WUGConditions_t * wug_client_get_conditions_data(void);
WUGHourly_t * wug_client_get_hourly_forecast_data(void);

int mgos_wug_get_conditions(int data, mgos_wug_data_cb_t cb, void *args);

#ifdef __cplusplus
}
#endif

#endif // MOS_LIBS_MGOS_WUG_CLIENT_H_
