/**
 *
 * This file is part of c99-wisol-sigfox
 *
 *   c99-wisol-sigfox is free software: you can redistribute it and/or modify
 *   it under the terms of the GNU Lesser General Public License as published by
 *   the Free Software Foundation, either version 3 of the License, or
 *   (at your option) any later version.
 *
 *   c99-wisol-sigfox is distributed in the hope that it will be useful,
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *   GNU General Public License for more details.
 *
 *   You should have received a copy of the GNU Lesser General Public License
 *   along withc99-wisol-sigfox.  If not, see <http://www.gnu.org/licenses/>.
 */

#include "wisol_sigfox.h"
#include <string.h>
#include <stdlib.h>

#ifdef __cplusplus
extern "C" {
#endif

/**
 * Command array
 */
const char *wisol_sigfox__commands[] = {
  "AT",                       // Dummy command / check status
  "AT$SB=%s",                 // Send Bit (and downlink flag)
  "AT$SF=%s%s",               // Send Frame (and bit)
  "AT$SO",                    // send out-of-band message
  "AT$TR?",                   // Get the transmit repeat
  "AT$TR=%u",                 // set the transmit repeat
  "ATS%u?",                   // get register
  "ATS%u=%s",                 // set register
  "AT$IF=%s",                 // set TX frequency
  "AT$IF?",                   // get TX frequency
  "AT$DR=%s",                 // set RX frequency
  "AT$DR?",                   // get RX frequency
  "AT$CW=%u,%c,%u",           // run emission test
  "AT$CB=%u,%c",              // test mode TX constant byte
  "AT$T?",                    // get temperature
  "AT$V?",                    // get voltages
  "AT$I=%u",                  // get chip information
  "AT$P=%u",                  // set power module
  "AT$WR",                    // save config
  "AT:P%u?",                  // get gpio pin
  "AT:P%u=?",                 // get gpio pin range
  "AT:P%u=%s",                // set gpio pin mode
  "AT:ADC P%u%s"              //
  "AT:SPI%c=%s",
  "AT:CLK=%s,%s",
  "AT:CLK=OFF",
  "AT:CLK?",
  "AT:DAC=%s",
  "AT:DAC=off",
  "AT:DAC?",
  "ATSTM=%s,%s"               // Activate test mode
  "AT$SE"                     // test mode
  "AT$SL=%s"                  // send local loop Frame
  "AT$RL"                     // receive local loop Frame
};

/**
 * Commands as indexes into command array above.
 * Commands defined here are implemented, the rest is not (yet)
 */
typedef enum {
  CMD_STATUS = 0,
  CMD_SEND_FRAME = 2,
  CMD_GET_REGISTER = 6,
  CMD_SET_REGISTER = 7,
  CMD_GET_TX_FREQ = 9,
  CMD_GET_RX_FREQ = 11,
  CMD_INFORMATION = 16,
  CMD_SET_POWER_MODE = 17
} wisol_sigfox__commmands_e;

// size of scratch buffer to construct commands
#define DEFAULT_BUF_SZ  32

// default timeout when communicating with module
#define WISOL_SIGFOX_DEFAULT_TIMEOUT  100

bool wisol_sigfox__ready() {
  char buf[DEFAULT_BUF_SZ];
  wisol_sigfox__serial_sync(wisol_sigfox__commands[CMD_STATUS], buf, sizeof(buf), WISOL_SIGFOX_DEFAULT_TIMEOUT);
  return ( strncmp("OK", buf, sizeof(buf)) == 0);
}

void wisol_sigfox__get_id(char *p_id_buf) {
  memset(p_id_buf, 0, 9);
  char buf[DEFAULT_BUF_SZ];
  snprintf(buf, sizeof(buf), wisol_sigfox__commands[CMD_INFORMATION], 10);
  wisol_sigfox__serial_sync(buf, p_id_buf, 8, WISOL_SIGFOX_DEFAULT_TIMEOUT);
}

void wisol_sigfox__get_pac(char *p_pac_buf) {
  memset(p_pac_buf, 0, 17);
  char buf[DEFAULT_BUF_SZ];
  snprintf(buf, sizeof(buf), wisol_sigfox__commands[CMD_INFORMATION], 11);
  wisol_sigfox__serial_sync(buf, p_pac_buf, 16, WISOL_SIGFOX_DEFAULT_TIMEOUT);
}

float wisol_sigfox__freq_str_to_fl(const char *buf) {
  char b1[4], b2[7];
  memset(b1,0,sizeof(b1));
  strncpy(b1,buf,3);
  memset(b2,0,sizeof(b2));
  strncpy(b2,(buf+3),6);

  return (float)atoi(b1)+((float)atoi(b2))/1000000.0f;
}

float wisol_sigfox__get_tx_freq() {
  char buf[DEFAULT_BUF_SZ];
  char buf_res[DEFAULT_BUF_SZ];
  snprintf(buf, sizeof(buf), "%s", wisol_sigfox__commands[CMD_GET_TX_FREQ]);
  wisol_sigfox__serial_sync(buf, buf_res, sizeof(buf_res), WISOL_SIGFOX_DEFAULT_TIMEOUT);

  return wisol_sigfox__freq_str_to_fl(buf_res);
}

float wisol_sigfox__get_rx_freq() {
  char buf[DEFAULT_BUF_SZ];
  char buf_res[DEFAULT_BUF_SZ];
  snprintf(buf, sizeof(buf), "%s", wisol_sigfox__commands[CMD_GET_RX_FREQ]);
  wisol_sigfox__serial_sync(buf, buf_res, sizeof(buf_res), WISOL_SIGFOX_DEFAULT_TIMEOUT);

  return wisol_sigfox__freq_str_to_fl(buf_res);
}

bool wisol_sigfox__send_frame(const uint8_t *p_payload_buf, size_t payload_size, bool downlink_flag) {
  char buf[24+DEFAULT_BUF_SZ];
  char payload_buf[25];

  if (payload_size > 12) {
    return false;
  }

  // convert payload to hex, 12 bytes max, formatted to hex plus \0 = 25 bytes max.
  char *p_buf = payload_buf;
  for ( int i = 0; i < payload_size; i++) {
    p_buf += sprintf(p_buf, "%02X", p_payload_buf[i]);
  }

  size_t s = snprintf(
    buf,sizeof(buf),
    wisol_sigfox__commands[CMD_SEND_FRAME],
    payload_buf, (downlink_flag?",1":"")
  );

  char buf_res[DEFAULT_BUF_SZ];

  // send with larger timeout, to wait for module to respond
  wisol_sigfox__serial_sync(buf, buf_res, sizeof(buf_res), 20000);

  return ( strncmp(buf_res, "OK", sizeof(buf_res)) == 0);
}

void wisol_sigfox__get_firmware_version(char *p_buf, size_t sz_buf) {
  char buf[DEFAULT_BUF_SZ];
  char buf_res1[DEFAULT_BUF_SZ];
  char buf_res2[DEFAULT_BUF_SZ];

  snprintf(buf, sizeof(buf), wisol_sigfox__commands[CMD_INFORMATION], 4);
  wisol_sigfox__serial_sync(buf, buf_res1, sizeof(buf_res1), WISOL_SIGFOX_DEFAULT_TIMEOUT);
  snprintf(buf, sizeof(buf), wisol_sigfox__commands[CMD_INFORMATION], 5);
  wisol_sigfox__serial_sync(buf, buf_res2, sizeof(buf_res2), WISOL_SIGFOX_DEFAULT_TIMEOUT);

  snprintf(p_buf, sz_buf, "%s.%s", buf_res1, buf_res2);
}

void wisol_sigfox__get_firmware_vcs_version(char *p_buf, size_t sz_buf) {
  char buf[DEFAULT_BUF_SZ];
  snprintf(buf, sizeof(buf), wisol_sigfox__commands[CMD_INFORMATION], 8);
  wisol_sigfox__serial_sync(buf, p_buf, sz_buf, WISOL_SIGFOX_DEFAULT_TIMEOUT);
}

void wisol_sigfox__get_library_version(char *p_buf, size_t sz_buf) {
  char buf[DEFAULT_BUF_SZ];
  snprintf(buf, sizeof(buf), wisol_sigfox__commands[CMD_INFORMATION], 9);
  wisol_sigfox__serial_sync(buf, p_buf, sz_buf, WISOL_SIGFOX_DEFAULT_TIMEOUT);
}

void wisol_sigfox__set_power_mode(wisol_sigfox_power_mode_t mode) {
  char buf[DEFAULT_BUF_SZ];
  char buf_res[DEFAULT_BUF_SZ];
  snprintf(buf, sizeof(buf), wisol_sigfox__commands[CMD_SET_POWER_MODE], (uint8_t)mode);
  wisol_sigfox__serial_sync(buf, buf_res, sizeof(buf_res), WISOL_SIGFOX_DEFAULT_TIMEOUT);
}

uint8_t wisol_sigfox__get_power_level() {
  char buf[DEFAULT_BUF_SZ];
  char buf_res[DEFAULT_BUF_SZ];
  memset(buf_res,0,sizeof(buf_res));
  snprintf(buf, sizeof(buf), wisol_sigfox__commands[CMD_GET_REGISTER], 302);
  wisol_sigfox__serial_sync(buf, buf_res, sizeof(buf_res), WISOL_SIGFOX_DEFAULT_TIMEOUT);

  return (uint8_t)atoi(buf_res);
}

void wisol_sigfox__set_power_level(uint8_t power_level) {
  char buf[DEFAULT_BUF_SZ];
  char buf_res[DEFAULT_BUF_SZ];
  char buf_pl[DEFAULT_BUF_SZ];
  memset(buf_res,0,sizeof(buf_res));
  memset(buf_pl,0,sizeof(buf_pl));
  snprintf(buf_pl, sizeof(buf_pl), "%u", power_level);
  snprintf(buf, sizeof(buf), wisol_sigfox__commands[CMD_SET_REGISTER], 302, buf_pl);
  wisol_sigfox__serial_sync(buf, buf_res, sizeof(buf_res), WISOL_SIGFOX_DEFAULT_TIMEOUT);
}

#ifdef __cplusplus
}
#endif
