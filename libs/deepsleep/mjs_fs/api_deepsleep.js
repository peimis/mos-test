let DeepSleep = {
  // ## **`DeepSleep.enter(ms)`**
  // Put the ESP32 to deep sleep mode. `ms` is a deep sleep period given as milliseconds.
  // If `ms` is 0 device is set to sleep until external interrupt / ULP wakes cpu.
  //
  // Return: true - success, false - failure.
  //
  // Example:
  // ```javascript
  // DeepSleep.enter(130);
  // ```
  // Note:
  // Remember to set the deep sleep wakeup stubs.
  EnableGpioWakeup: ffi('bool mgos_deepsleep_enable_gpio_wakeup(int)'),
  SleepMs: ffi('bool mgos_deepsleep_ms(int)'),
  SetWakeupMode: ffi('void mgos_deepsleep_set_wakeup_mode(int)'),
  SleepNow: ffi('bool mgos_deepsleep_now(int)'),
  Count: ffi('int mgos_deepsleep_count()'),
};
