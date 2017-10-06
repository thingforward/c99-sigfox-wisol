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
#ifndef __WISOL_SIGFOX_H
#define __WISOL_SIGFOX_H

#include <stdint.h>
#include <stdio.h>
#include <stdbool.h>

#ifdef __cplusplus
extern "C" {
#endif

/**
 * External function for serial communication with device.
 * Sends a single command (AT...) to device, waits for a response up to given timeout
 * and places the result in given response buffer.
 *
 * !! Needs to be implemented by user, to handle serial i/o between module and board !!
 *
 * @param cmd                 command to send to module
 * @param p_response_buf      buffer for response
 * @param sz_response_buf     size of response buffer
 * @param timeout             timeout in [msec] to wait for response from module
 */
extern void wisol_sigfox__serial_sync(const char *cmd, char *p_response_buf, int sz_response_buf, int timeout);

/**
 * Sends dummy/check command to module and checks for "OK" response.
 * @return true if module responded with "OK" to "AT" dummy command
 */
bool wisol_sigfox__ready();

/**
 * Copies module's ID into given buffer.
 * @param p_id_buf  Buffer to receive id
 * @warning buffer must hold at least 9 bytes.
 */
void wisol_sigfox__get_id(char *p_id_buf);

/**
 * copies module's PAC into given buffer.
 * @param p_pac_buf buffer to receive pac
 * @warning Buffer must hod at least 17 bytes.
 */
void wisol_sigfox__get_pac(char *p_pac_buf);

/**
 * sends a single frame with given payload and size (max. 12 bytes). May optionally request a downlink frame
 * @param p_payload_buf   payload to send
 * @param payload_size    size of payload, max. 12 bytes
 * @param download_flag   if true, the download request flag is transmitted as well
 */
bool wisol_sigfox__send_frame(const uint8_t *p_payload_buf, size_t payload_size, bool downlink_flag);

/**
 * returns the TX frequency
 * @return frequency
 */
float wisol_sigfox__get_tx_freq();

/**
 * returns RX frequency
 * @return frequency
 */
float wisol_sigfox__get_rx_freq();

/**
 * retrieves firmware VCS version and copies to buffer
 * @param p_buf   buffer to receive result
 * @param sz_buf  max size of buffer
 ^*/
void wisol_sigfox__get_firmware_vcs_version(char *p_buf, size_t sz_buf);

/**
 * retrieves firmware version and copies to buffer
 * @param p_buf   buffer to receive result
 * @param sz_buf  max size of buffer
 */
void wisol_sigfox__get_firmware_version(char *p_buf, size_t sz_buf);

/**
 * retrieves sigfox library version and copies to buffer
 * @param p_buf   buffer to receive result
 * @param sz_buf  max size of buffer
 */
void wisol_sigfox__get_library_version(char *p_buf, size_t sz_buf);

/** power mode types, see wisol documentation */
typedef enum wisol_sigfox_power_mode_e {
  WISOL_SIGFOX__POWER_MODE_RESET = 0,
  WISOL_SIGFOX__POWER_MODE_SLEEP = 1,
  WISOL_SIGFOX__POWER_MODE_DEEPSLEEP = 2
} wisol_sigfox_power_mode_t;

/**
 * Sets the power mode
 * @param mode  power mode to be set.
 */
void wisol_sigfox__set_power_mode( wisol_sigfox_power_mode_t mode);

/**
 * Gets the output power level
 * @return output power level in dBm (0..14)
 */
uint8_t wisol_sigfox__get_power_level();

/**
 * Sets the output power level
 * @param power_level output power level in dBm (0..14)
 */
void wisol_sigfox__set_power_level(uint8_t power_level);


#ifdef __cplusplus
}
#endif

#endif    /* __LPWAN_SIGFOX_H */
