#ifndef ENV5_AT_COMMANDS
#define ENV5_AT_COMMANDS

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
#define AT_DISABLE_ECHO "ATE0"
#define AT_DISABLE_ECHO_LENGTH 5
#define AT_DISABLE_ECHO_RESPONSE "OK"

/* Soft Reset (restart module)
 *
 * Command:
 *   AT+RST
 *
 * Expected Response:
 *   OK
 */
#define AT_RESTART "AT+RST"
#define AT_RESTART_LENGTH 7
#define AT_RESTART_RESPONSE "OK"

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
#define AT_DISABLE_MULTI_CONNECT "AT+CIPMUX=0"
#define AT_DISABLE_MULTI_CONNECT_LENGTH 12
#define AT_DISABLE_MULTI_CONNECT_RESPONSE "OK"

/* Connect to network
 *
 * Command:
 *   AT+CWJAP=SSID,PWD,BSSID,PCI_EN,RECON_INTERVAL,LISTEN_INTERVAL,SCAN_MODE,TIMEOUT,PMF
 *
 *   If no arguments were passed> use settings from last successful connection
 *
 * Fields are:
 * - SSID -> Network getID
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
#define AT_CONNECT_TO_NETWORK "AT+CWJAP=\"%s\",\"%s\",,1,5,3,0,120"
#define AT_CONNECT_TO_NETWORK_LENGTH 28
#define AT_CONNECT_TO_NETWORK_RESPONSE "WIFI CONNECTED"

/* Check status of current network connection
 *
 * Command:
 *   AT+CWJAP?
 *
 * Expected Response:
 *   +CWJAP:SSID,BSSID,CHANNEL,RSSI,PCI_EN,RECONN_INTERVAL,LISTEN_INTERVAL,SCAN_MODE,PMF
 *   OK
 */
#define AT_CHECK_CONNECTION "AT+CWJAP?"
#define AT_CHECK_CONNECTION_LENGTH 10
#define AT_CHECK_CONNCETION_RESPONSE "OK"

/* Disconnect from network
 *
 * Command:
 *   AT+CWQAP
 *
 * Expected Response:
 *   OK
 */
#define AT_DISCONNECT "AT+CWQAP"
#define AT_DISCONNECT_LENGTH 9
#define AT_DISCONNECT_RESPONSE "OK"

/* endregion */

/* region MQTT */

/* Connect to MQTT broker
 *
 * Command:
 *   AT+MQTTCONN=LINK_ID,BROKER_IP,BROKER_PORT,RECONNECT
 *
 * Fields are:
 * - LINK_ID -> always 0
 * - BROKER_IP -> Domain/IP of the MQTT broker, max. 128 Bytes
 * - BROKER_PORT -> Port of the MQTT Broker, max. 65535
 * - RECONNECT -> 0=no automatic reconnect, 1=automatic reconnect
 *
 * Expected Response:
 *   OK
 */
#define AT_MQTT_CONNECT_TO_BROKER "AT+MQTTCONN=0,\"%s\",%s,0"
#define AT_MQTT_CONNECT_TO_BROKER_LENGTH 20
#define AT_MQTT_CONNECT_TO_BROKER_RESPONSE "OK"

/* Disconnect from MQTT Broker and relase resources
 *
 * Command:
 *   AT+MQTTCLEAN=LINK_ID
 *
 * Fields are:
 * - LINK_ID -> always 0
 *
 * Expected Response:
 *   OK
 *
 */
#define AT_MQTT_DISCONNECT_FROM_BROKER "AT+MQTTCLEAN=0"
#define AT_MQTT_DISCONNECT_FROM_BROKER_LENGTH 15
#define AT_MQTT_DISCONNECT_FROM_BROKER_RESPONSE "OK"

/* Set Connection Configuration
 *
 * Command:
 *   AT+MQTTCONNCFG=LINK_ID,KEEPALIVE,DISABLE_CLEAN_SESSION,"LWT_TOPIC","LWT_MSG",LWT_QOS,LWT_RETAIN
 *
 * Fields are:
 * - LINK_ID -> always 0
 * - KEEPALIVE -> timeout for MQTT ping, seconds between 0 and 7200, default is 0
 * - DISABLE_CLEAN_SESSION -> 0=Resume old session on reconnect, 1=start new session on reconnect
 * - LWT_TOPIC -> Disconnect topic
 * - LWT_MSG -> Disconnect message
 * - LWT_QOS -> Disconnect message Quality of service,
 *              0=try delivery (default), 1=message delivered, 2=message delivered exactly one time
 * - LWT_RETAIN -> Keep LWT message as last good message
 *                 1=keep message, 0=disable keep message
 *
 * Expected Response:
 *   OK
 */
#define AT_MQTT_CONNECTION_CONFIGURATION "AT+MQTTCONNCFG=0,60,0,\"%s\",\"%s\",1,1"
#define AT_MQTT_CONNECTION_CONFIGURATION_LENGTH 32
#define AT_MQTT_CONNECTION_CONFIGURATION_RESPONSE "OK"

/* Set User configuration
 *
 * Command:
 *   AT+MQTTUSERCFG=LINK_ID,SCHEME,CLIENT_ID,USERNAME,PASSWORD,CERT_ID,CA_ID,PATH
 *
 * Field are:
 * - LINK_ID -> always 0
 * - SCHEME -> 1=TCP, 2=TLS (no certificate), 3=TLS (server certificate),
 *             4=TLS (client certificate), 5=TLS (server and client certificate)
 *             6=WebSocket+1, 7=WebSocket+2, 8=WebSocket+3, 9=WebSocket+4, 10=WebSocket+5
 * - CLIENT_ID -> MQTT client, max 256 Bytes
 * - USERNAME -> Login username for Broker, max 64 Bytes
 * - PASSWORD -> Login password for Broker, max 64 Bytes
 * - CERT_ID -> Certificate ID, always 0
 * - CA_ID -> Certificate authority getID, always 0
 * - PATH -> Path to resource, max 32 Bytes
 *
 * Expected Response:
 *   OK
 */
#define AT_MQTT_USER_CONFIGURATION "AT+MQTTUSERCFG=0,1,\"%s\",\"%s\",\"%s\",0,0,\"\""
#define AT_MQTT_USER_CONFIGURATION_LENGTH 35
#define AT_MQTT_USER_CONFIGURATION_RESPONSE "OK"

/* Subscribe to Topic
 *
 * Command:
 *   AT+MQTTSUB=LINK_ID,TOPIC,QOS
 *
 * Fields are:
 * - LINK_ID -> always 0
 * - TOPIC -> Topic to subscribe
 * - QOS -> Quality of service,
 *          0=try delivery, 1=message delivered, 2=message delivered exactly one time
 *
 * Expected Response:
 *   OK
 *  or:
 *   ALREADY SUBSCRIBED
 *
 */
#define AT_MQTT_SUBSCRIBE_TOPIC "AT+MQTTSUB=0,\"%s\",0"
#define AT_MQTT_SUBSCRIBE_TOPIC_LENGTH 18
#define AT_MQTT_SUBSCRIBE_TOPIC_RESPONSE "OK"

/* Unsubscribe from Topic
 *
 * Command:
 *   AT+MQTTUNSUB=LINK_ID,TOPIC
 *
 * Field are:
 * - LINK_ID -> always 0
 * - TOPIC -> Topic to unsubscribe from
 *
 * Expected Response:
 *   OK
 */
#define AT_MQTT_UNSUBSCRIBE_TOPIC "AT+MQTTUNSUB=0,\"%s\""
#define AT_MQTT_UNSUBSCRIBE_TOPIC_LENGTH 18
#define AT_MQTT_UNSUBSCRIBE_TOPIC_RESPONSE "OK"

/* Publish data to broker
 *
 * Command:
 *   AT+MQTTPUB=LINK_ID,TOPIC,DATA,QOS,RETAIN
 *
 * Fields are:
 * - LINK_ID -> always 0
 * - TOPIC -> topic under which the data is published
 * - DATA -> data to publish
 * - QOS -> Quality of service,
 *          0=try delivery, 1=message delivered, 2=message delivered exactly one time
 * - RETAIN -> Retain last message of topic, 0=disable, 1=enable
 *
 * Expected Response:
 *   OK
 */
#define AT_MQTT_PUBLISH "AT+MQTTPUB=0,\"%s\",\"%s\",0,%s"
#define AT_MQTT_PUBLISH_LENGTH 23
#define AT_MQTT_PUBLISH_RESPONSE "OK"

/* Publish data if command would be longer than 256 bytes to broker
 *
 * Command:
 *   AT+MQTTPUBRAW=<LinkID>,<"topic">,<length>,<qos>,<retain>
 *
 * Fields are:
 * - LINK_ID -> always 0
 * - TOPIC -> topic under which the data is published
 * - LENGTH -> Length of MQTT Message
 * - QOS -> Quality of service,
 *          0=try delivery, 1=message delivered, 2=message delivered exactly one time
 * - RETAIN -> Retain last message of topic, 0=disable, 1=enable
 *
 * Expected Response:
 *   OK
 *   >
 */
#define AT_MQTT_PUBLISH_LONG "AT+MQTTPUBRAW=0,\"%s\",%lu,0,0"
#define AT_MQTT_PUBLISH_LONG_LENGTH 24
#define AT_MQTT_PUBLISH_LONG_RESPONSE "OK"

/* Publish data if command would be longer than 256 bytes to broker
 *
 * Command:
 *  AT+HTTPCLIENT=<opt>,<content-type>,<"url">,[<"host">],[<"path">],<transport_type>[,<"data">][,<"http_req_header">][,<"http_req_header">][...]
 *
 * Fields are:
 * - OPT -> Method of Request
 * - CONTENT-TYPE -> Data type (1: json)
 * - URL -> data to publish
 * - HOST ->
 * - PATH ->
 * - TRANSPORT_TYPE -> Client transport type (Default 1, TCP)
 * - DATA -> Data if POST requests
 * - HTTP_REQ_HEADER -> Request Header
 *
 * Expected Response:
 *   +HTTPCLIENT:<size>,<data>
 *
 *   OK
 */
#define AT_HTTP_GET "AT+HTTPCLIENT=2,0,\"%s\",,,1"
#define AT_HTTP_GET_LENGTH 23
#define AT_HTTP_GET_RESPONSE "OK"
/* endregion */

#endif /* ENV5_AT_COMMANDS */
