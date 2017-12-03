let Wug = {
  // ## **`Wug.getConditions(url, file)`**
  // Put the ESP32 to deep sleep mode. `ms` is a deep sleep period given as milliseconds.
  // If `ms` is 0 device is set to sleep until external interrupt / ULP wakes cpu.
  //
  // Return: true - success, false - failure.
  //
  // Example:
  // ```javascript
  // Wug.getConditions('http://my.url/file.ext, 'localfile.txt');
  // ```
  // Note:
  // Remember to set the deep sleep wakeup stubs.
  getConditions: ffi('int mgos_wug_get_conditions(int, void(*)(int, char *, userdata), userdata)'),
};
