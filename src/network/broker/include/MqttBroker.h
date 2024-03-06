#ifndef ENV5_MQTT_BROKER_HEADER
#define ENV5_MQTT_BROKER_HEADER

#include <stdbool.h>
#include <stdint.h>

#include "Posting.h"

#define MAX_SUBSCRIBER 100

struct mqttHost {
    char *ip;
    char *port;
    char *userID;
    char *password;
};
typedef struct mqttHost mqttBrokerHost_t;

typedef enum MQTTExceptions {
    MQTT_ESP_CHIP_FAILED = 0x01,
    MQTT_WIFI_FAILED = 0x02,
    MQTT_CONNECTION_FAILED = 0x03,
    MQTT_ESP_WRONG_ANSWER = 0x04,
    MQTT_ALREADY_CONNECTED = 0x05,
    MQTT_COULD_NOT_DISCONNECT_BROKER = 0x06
} MQTTExceptions_t;

/*!
 * @brief tries to connect to Broker until successful
 *
 * Set the broker domain, client ID and then tries to connect to the host broker until successful.
 * If there is already any connection, then it is also successful.
 *
 * @param mqttHost contains ip and port of MQTT host
 * @param brokerDomain domain of broker, added before every message
 * @param clientID return value of `getDomain` for this client,
 *                 used to Identify to the Broker and added after the Domain in every message
 *
 * @throws MQTT_ESP_CHIP_FAILED if we can't communicate with esp chip
 * @throws MQTT_WIFI_FAILED if wifi is not connected
 * @throws MQTT_ALREADY_CONNECTED if already connected to mqtt broker
 */
void mqttBrokerConnectToBrokerUntilSuccessful(char *brokerDomain, char *clientID);

/*!
 * @brief tries to connect to Broker
 *
 * Set the broker domain, client ID and then tries to connect to the host broker.
 * If there is already any connection, then it is also successful
 *
 * @param mqttHost contains ip and port of MQTT host
 * @param brokerDomain domain of broker, added before every message
 * @param clientID return value of `getDomain` for this client,
 *                 used to Identify to the Broker and added after the Domain in every message
 *
 * @throws MQTT_ESP_CHIP_FAILED if we can't communicate with esp chip
 * @throws MQTT_WIFI_FAILED if wifi is not connected
 * @throws MQTT_ALREADY_CONNECTED if already connected to mqtt broker
 * @throws MQTT_ESP_WRONG_ANSWER if response from esp chip is invalid
 * @throws MQTT_CONNECTION_FAILED if establishing a connection failed
 */
void mqttBrokerConnectToBroker(mqttBrokerHost_t credentials, char *brokerDomain, char *clientID);

void publishLong(posting_t posting);

/*!
 * @brief disconnect from MQTT broker
 * @param force[bool] if set disconnect comment will force disconnect
 */
void mqttBrokerDisconnect(bool force);

/*!
 * @brief called by uart when MQTT message is received
 * @param response the MQTT message received
 */
void mqttBrokerReceive(char *response);

/*!
 * @brief publish online status
 * @param availableMeasurements topic IDs of the available measurements
 */
void publishAliveStatusMessage(char *availableMeasurements);

#endif /* ENV5_MQTT_BROKER_HEADER */
