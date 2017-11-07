// Load Mongoose OS API
load('api_timer.js');
load('api_gpio.js');
load('api_pwm.js');
// load('api_mqtt.js');


//LED Status Lights
let RED_LED = 2;
let BLUE_LED = 2;

let PWM_duty = 0.1;
let direction = 0.05;

GPIO.set_mode(RED_LED, GPIO.MODE_OUTPUT);

// Primary loop run every 1 second looking for a door state change
Timer.set(50, 1, function() {
  PWM.set(RED_LED, 500, PWM_duty);
  PWM_duty += direction;

  if (PWM_duty >= 0.99) {
    direction = -0.05;
    print("Down");

  } else if (PWM_duty <= 0.05) {
    direction = 0.05;
    print("Up");

  }

}, null);
