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


typedef struct WUGHourlyItem {
	int		Temp;
	int		dewPoint;
	float	windSpeed;
	int		windDir;
	int		windchill;
	int		sky;
	int		humidity;
	int		fctcode;
	int		UV;
	float	qpf;
	float   snow;
	float   feelslike;
	uint32_t	forecastTime;
	char    *hourlyIcon;
	char    *hourlyCondition;
} WUGHourlyItem_t;


typedef struct WUGHourly {
	WUGHourlyItem_t	hourlydata[24];
	uint32_t		forecastTime;
	int				currentIndex;
} WUGHourly_t;

/* FFI-able signature of the function that receives response to a request. */
typedef int (*mgos_wug_data_cb_t)(int status, char *result, void *args);

#ifdef __cplusplus
extern "C" {
#endif

void wug_client_init(WUGConditions_t *c);
void wug_client_set_conditions(WUGConditions_t *c);
int mgos_wug_get_conditions(int data, mgos_wug_data_cb_t cb, void *args);

#ifdef __cplusplus
}
#endif

#endif // MOS_LIBS_MGOS_WUG_CLIENT_H_
