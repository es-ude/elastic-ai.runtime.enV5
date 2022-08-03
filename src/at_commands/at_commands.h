#ifndef SENSORBOARD_ESP_AT_COMMANDS
#define SENSORBOARD_ESP_AT_COMMANDS

#include <stddef.h>

/* region NETWORK */

/* Disable echo of AT command
 *
 * Command:
 *   ATE0
 *
 * Expected Response:
 *   OK
 */
extern char AT_DISABLE_ECHO[];
extern size_t AT_DISABLE_ECHO_LENGTH;
extern char AT_DISABLE_ECHO_RESPONSE[];

/* Soft Reset (restart module)
 *
 * Command:
 *   AT+RST
 *
 * Expected Response:
 *   OK
 */
extern char AT_RESTART[];
extern size_t AT_RESTART_LENGTH;
extern char AT_RESTART_RESPONSE[];

/* Disable multiple connections
 *
 * Command:
 *   AT+CIPMUX=MODE
 *
 * Can only be used when disconnected
 *
 * Fields are:
 * - MODE -> 0=disable, 1=enable
 *
 * Expected Response:
 *   OK
 */
extern char AT_DISABLE_MULTI_CONNECT[];
extern size_t AT_DISABLE_MULTI_CONNECT_LENGTH;
extern char AT_DISABLE_MULTI_CONNECT_RESPONSE[];

/* Connect to network
 *
 * Command:
 *   AT+CWJAP=SSID,PWD,BSSID,PCI_EN,RECON_INTERVAL,LISTEN_INTERVAL,SCAN_MODE,TIMEOUT,PMF
 *
 *   If no arguments were passed => use settings from last successful connection
 *
 * Fields are:
 * - SSID -> Network ID
 * - PWD -> Network Password
 * - BSSID -> Access Point MAC address
 * - PCI_EN -> 0=enable OPEN and WEP, 1=Disable OPEN and WEP
 * - RECON_INTERVAL -> time until reconnect, seconds between 1 and 7200, default is 1, 0=Disable
 * - LISTEN_INTERVAL -> time to listen to AP beacon, seconds between 1 and 100, default is 3
 * - SCAN_MODE -> 0=Fast scan - interrupt scan after AP found, 1=all channel scan
 * - TIMEOUT -> time until connection trial is timeout, seconds between 3 and 600, default is 15
 * - PMF -> Frame Protection, 0=Disable, 1=Optional, 3=Required
 *
 * Expected Response:
 *   WIFI CONNECTED
 *   WIFI GOT IP
 *   OK
 */
extern char AT_CONNECT_TO_NETWORK[];
extern size_t AT_CONNECT_TO_NETWORK_LENGTH;
extern char AT_CONNECT_TO_NETWORK_RESPONSE[];

/* Disconnect from network
 *
 * Command:
 *   AT+CWQAP
 *
 * Expected Response:
 *   OK
 */
extern char AT_DISCONNECT[];
extern size_t AT_DISCONNECT_LENGTH;
extern char AT_DISCONNECT_RESPONSE[];

/* endregion */

/* region MQTT */

/* endregion */

#endif /* SENSORBOARD_ESP_AT_COMMANDS */
