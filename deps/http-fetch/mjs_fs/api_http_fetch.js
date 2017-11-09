let HttpFetch = {
  // ## **`HttpFetch.get(url, file)`**
  // Put the ESP32 to deep sleep mode. `ms` is a deep sleep period given as milliseconds.
  // If `ms` is 0 device is set to sleep until external interrupt / ULP wakes cpu.
  //
  // Return: true - success, false - failure.
  //
  // Example:
  // ```javascript
  // HttpFetch.Get('http://my.url/file.ext, 'localfile.txt');
  // ```
  // Note:
  // Remember to set the deep sleep wakeup stubs.
  Get: ffi('int mgos_http_fetch(char *, char *, void(*)(char *, int, int, userdata), userdata)'),
};
