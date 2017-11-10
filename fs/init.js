// Load Mongoose OS API
load('api_config.js');
load('api_timer.js');
load('api_gpio.js');
load('api_deepsleep.js');
load('api_http.js');
load('api_http_fetch.js');
// load('api_pwm.js');
// load('api_mqtt.js');

load('api_arduino_bme280.js');

//LED Status Lights
let RED_LED = Cfg.get('app_gpio.led_r');
let GREEN_LED = Cfg.get('app_gpio.led_g');
let BLUE_LED = Cfg.get('app_gpio.led_b');
let deepsleep = Cfg.get('app.time_deepsleep');
let awake = Cfg.get('app.time_awake');

let bme_cs = Cfg.get('app_gpio.bme280_cs');
let button = Cfg.get('app_gpio.button');
let load_en = Cfg.get('app_gpio.load_en');
let count = 0;
let apikey = 'your-apikey-here';


//let features = 'conditions/forecast';
//let features = 'forecast';
let features = 'conditions';
//let features = 'hourly';

//let options = 'lang:FI/';
let options = '';
let location = 'Finland/Nousiainen';
let http_url = 'http://api.wunderground.com/api/' + apikey + '/' + features + '/' + options + 'q/' + location + '.json';

print('leds green:', GREEN_LED, ' red:', RED_LED, ' BME280 CS:', bme_cs);

// Initialize Adafruit_BME280 library
// 23, 19, 18 mosi, miso, sck
let bme = Adafruit_BME280.create(bme_cs, 23, 19, 18);
//let bme = Adafruit_BME280.create(bme_cs, -1, -1, -1);
let bme_present = 0;

GPIO.write(GREEN_LED, 1);
GPIO.set_mode(GREEN_LED, GPIO.MODE_OUTPUT);
GPIO.write(RED_LED, 1);
GPIO.set_mode(RED_LED, GPIO.MODE_OUTPUT);

GPIO.write(load_en, 1);
GPIO.set_mode(load_en, GPIO.MODE_OUTPUT);

// Publish to MQTT topic on a button press. Button is wired to GPIO pin 0
GPIO.set_button_handler(0, GPIO.PULL_UP, GPIO.INT_EDGE_NEG, 200, function() {
	let url = 'http://icons.wxug.com/i/c/k/cloudy.gif';

	print('Make a fetch query: ', url);

	HttpFetch.Get(url, 'cloudy.gif', function(status, code, bytes) {
		print("stat:", status, "code:", code, "bytes", bytes);
	}, null );
}, null);


// Publish to MQTT topic on a button press. Button is wired to GPIO pin 0
GPIO.set_button_handler(17, GPIO.PULL_UP, GPIO.INT_EDGE_NEG, 200, function() {
	print('Make HTTP query: ', http_url);

	HTTP.query({
	  url: http_url,
	  // Optional - headers
//		  headers: { 'X-Foo': 'bar' },
	  // Optional. If set, JSON-encoded and POST-ed
//		  data: {foo: 1, bar: 'baz'},
	  success: function(body, full_http_msg) { 
		print('Got response:');
		let weather = JSON.parse(body);
		print('Response:', JSON.stringify(weather));
	  },
	  error: function(err) { print(err); },  // Optional
	});

}, null);


Timer.set(4800, 1, function() {
	GPIO.write(RED_LED, 0);

	Timer.set(40, 0, function() {
		GPIO.write(RED_LED, 1);

		Timer.set(300, 0, function() {
			GPIO.write(RED_LED, 0);

			Timer.set(40, 0, function() {
				GPIO.write(RED_LED, 1);
			}, null);
		}, null);
	}, null);
}, null);

// Primary loop run every 1 second
//
Timer.set(1750, 1, function() {
	let wakeups = DeepSleep.Count();
	count = count + 1;
	print('* wakeups:', wakeups, " count:", count);

	GPIO.write(GREEN_LED, 0);

	Timer.set(50, 0, function() {
		GPIO.write(GREEN_LED, 1);
	}, null);

	if (bme_present === 1) {
		print('T:', bme.readTemperature(), '*C  H:', bme.readHumidity(), '%RH  P:', bme.readPressure(), 'hPa');
	} else {
		if (bme.begin( -1 ) === 0) {
			print('Cant find a sensor');
		} else {
			print("bme ok");
			bme_present = 1;
		}
	}

	if (count >= awake) {
		print('Sleep now ', deepsleep);
		GPIO.write(load_en, 0);
		DeepSleep.SleepMs(deepsleep);
	}

}, null);

