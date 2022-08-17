#ifndef SENSOR_BOARD_MQTT_BROKER_H
#define SENSOR_BOARD_MQTT_BROKER_H

#include <stdbool.h>

#define MAX_SUBSCRIBER 100

typedef struct {
    char *ip;
    char *port;
} MQTTHost_t;

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
void mqtt_ConnectToBrokerUntilSuccessful(MQTTHost_t mqttHost, char *brokerDomain, char *clientID);

/*! \brief tries to connect to Broker
 *
 * Set the broker domain, client Id and then tries to connect to the host broker. Is successful if
 * there is already any connection.
 *
 * \param mqttHost contains ip and port of MQTT host
 * \param brokerDomain domain of broker, added before every message
 * \param clientID getID of this client, used to Identify to the Broker and added after the Domain in
 * every message
 * \return true if connection successful or already connected, otherwise false
 */
bool mqtt_ConnectToBroker(MQTTHost_t credentials, char *brokerDomain, char *clientID);

/*! \brief disconnect from MQTT broker
 *
 * @param force[bool] if set disconnect comment will force disconnect
 */
void mqtt_Disconnect(bool force);

/*! \brief called by uart when MQTT message is received
 *
 * \param response the MQTT message received
 */
void mqtt_Receive(char *response);

void mqtt_setBrokerDomain(char *ID);

void mqtt_SetClientId(char *clientId);

#endif // SENSOR_BOARD_MQTT_BROKER_H
