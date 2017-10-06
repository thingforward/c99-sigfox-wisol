#include <Arduino.h>
#include <SoftwareSerial.h>
#include "wisol_sigfox.h"

// Software Serial to be used to communicate with SigFox module
// Change RX/TX pins according to your setup.
SoftwareSerial  wisol_serial(D1,D2);

#ifdef __cplusplus
extern "C" {
#endif

/** initialize software serial with proper timeouts */
void wisol_sigfox__serial_init() {
  wisol_serial.begin(9600);
  wisol_serial.setTimeout(100);
}

/** adapter function for synchronous communication with module */
void wisol_sigfox__serial_sync(const char *cmd, char *p_response_buf, int sz_response_buf, int timeout) {
  wisol_serial.setTimeout(timeout);
  wisol_serial.print(cmd); wisol_serial.print("\n");
  String s = wisol_serial.readStringUntil('\n');
  strncpy(p_response_buf, s.c_str(), sz_response_buf);
  int i = s.length()-1;
  while(i > 0 && (p_response_buf[i] == '\n' || p_response_buf[i] == '\r')) {
    p_response_buf[i--] = 0;
  }
}

#ifdef __cplusplus
}
#endif


void setup() {
  Serial.begin(9600);
  delay(1000);

  // attach a button to D7
  pinMode(D7, INPUT);

  // initialize serial communcaiton
  wisol_sigfox__serial_init();

  //
  if (wisol_sigfox__ready()) {
    Serial.println("Sigfox module is ready.");

    char buf[32];
    wisol_sigfox__get_id(buf);
    Serial.print("ID="); Serial.println(buf);

    wisol_sigfox__get_pac(buf);
    Serial.print("PAC="); Serial.println(buf);

    wisol_sigfox__get_firmware_version(buf, sizeof(buf));
    Serial.print("FirmwareVersion="); Serial.println(buf);

    wisol_sigfox__get_firmware_vcs_version(buf, sizeof(buf));
    Serial.print("FirmwareVCSVersion="); Serial.println(buf);

    wisol_sigfox__get_library_version(buf, sizeof(buf));
    Serial.print("LibraryVersion="); Serial.println(buf);

    //wisol_sigfox__set_power_level(9);
    //Serial.print("Output Power [dBm] "); Serial.println(wisol_sigfox__get_power_level());

  } else {
    Serial.println("Sigfox module is NOT ready.");
  }

}

char  buf[16];
const char  *format = "SF-%i";
int   counter = 0;

void loop() {
  delay(100);
  if( digitalRead(D7)) {
    Serial.println("Sending message ...");
    counter++;

    int l = snprintf(buf, sizeof(buf), format, counter);

    if (wisol_sigfox__send_frame((const uint8_t*)buf, l, false)) {
      Serial.println("sent.");
    }
  }
}
