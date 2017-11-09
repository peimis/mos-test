// Load Mongoose OS API
load('api_config.js');
load('api_timer.js');
load('api_gpio.js');
load('api_deepsleep.js');
load('api_http.js');
load('api_http_fetch.js');
// load('api_pwm.js');
// load('api_mqtt.js');


//LED Status Lights
let RED_LED = Cfg.get('app_gpio.led_r');
let GREEN_LED = Cfg.get('app_gpio.led_g');
let BLUE_LED = Cfg.get('app_gpio.led_b');
let deepsleep = Cfg.get('app.time_deepsleep');
let awake = Cfg.get('app.time_awake');
let count = 0;

let button = 0;

let apikey = 'your-apikey-here';

//let features = 'conditions/forecast';
//let features = 'forecast';
let features = 'conditions';
//let features = 'hourly';

//let options = 'lang:FI/';
let options = '';
let location = 'Finland/Nousiainen';
let http_url = 'http://api.wunderground.com/api/' + apikey + '/' + features + '/' + options + 'q/' + location + '.json';

print('led pin:', GREEN_LED);

GPIO.write(GREEN_LED, 0);
GPIO.set_mode(GREEN_LED, GPIO.MODE_OUTPUT);


// Publish to MQTT topic on a button press. Button is wired to GPIO pin 0
GPIO.set_button_handler(0, GPIO.PULL_UP, GPIO.INT_EDGE_NEG, 200, function() {
	let url = 'http://icons.wxug.com/i/c/k/cloudy.gif';

	print('Make a fetch query: ', url);

	HttpFetch.Get(url, 'cloudy.gif', function(status, code, bytes) {
		print("stat:", status, "code:", code, "bytes", bytes);
	}, null );
}, null);



// Publish to MQTT topic on a button press. Button is wired to GPIO pin 0
GPIO.set_button_handler(15, GPIO.PULL_UP, GPIO.INT_EDGE_NEG, 200, function() {
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



// Primary loop run every 1 second
//
Timer.set(1000, 1, function() {
	let wakeups = DeepSleep.Count();
	count = count + 1;
	print('* wakeups:', wakeups, " count:", count);

	GPIO.write(GREEN_LED, 1);

	Timer.set(50, 0, function() {
		GPIO.write(GREEN_LED, 0);
	}, null);

	if (count >= awake) {
		print('Sleep now ', deepsleep);
		DeepSleep.SleepMs(deepsleep);
	}

}, null);
