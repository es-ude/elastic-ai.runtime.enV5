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

enum {
    MQTT_NO_ERROR = 0x00,
    MQTT_ESP_CHIP_FAILED = 0x01,
    MQTT_WIFI_FAILED = 0x02,
    MQTT_CONNECTION_FAILED = 0x03,
    MQTT_ESP_WRONG_ANSWER = 0x04,
    MQTT_ALREADY_CONNECTED = 0x05
};
typedef uint8_t mqttBrokerErrorCode_t;

/*! \brief tries to connect to Broker until successful
 *
 * Set the broker domain, client Id and then tries to connect to the host broker until successful.
 * Is successful if there is already any connection.
 *
 * \param mqttHost contains ip and port of MQTT host
 * \param brokerDomain domain of broker, added before every message
 * \param clientID getDomain of this client, used to Identify to the Broker and added
 * after the Domain in every message
 */
mqttBrokerErrorCode_t mqttBrokerConnectToBrokerUntilSuccessful(mqttBrokerHost_t mqttHost,
                                                               char *brokerDomain, char *clientID);

/*! \brief tries to connect to Broker
 *
 * Set the broker domain, client Id and then tries to connect to the host broker. Is successful if
 * there is already any connection.
 *
 * \param mqttHost contains ip and port of MQTT host
 * \param brokerDomain domain of broker, added before every message
 * \param clientID getID of this client, used to Identify to the Broker and added after the Domain
 * in every message \return true if connection successful or already connected, otherwise false
 */
mqttBrokerErrorCode_t mqttBrokerConnectToBroker(mqttBrokerHost_t credentials, char *brokerDomain,
                                                char *clientID);

void publishLong(posting_t posting);

/*! \brief disconnect from MQTT broker
 *
 * @param force[bool] if set disconnect comment will force disconnect
 */
void mqttBrokerDisconnect(bool force);

/*! \brief called by uart when MQTT message is received
 *
 * \param response the MQTT message received
 */
void mqttBrokerReceive(char *response);

/*! \brief Published Online Status */
void publishAliveStatusMessage(char *measurements);

#endif /* ENV5_MQTT_BROKER_HEADER */
