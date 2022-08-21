#ifndef ENV5_NETWORK_HEADER
#define ENV5_NETWORK_HEADER

#include <stdbool.h>
#include <stdint.h>

typedef struct {
    char *ssid;
    char *password;
} NetworkCredentials_t;

enum {
    NETWORK_NO_ERROR = 0x01,
    NETWORK_ESTABLISH_CONNECTION_FAILED = 0x02,
    NETWORK_ESP_CHIP_FAILED = 0x03,
    NETWORK_WIFI_ALREADY_CONNECTED = 0x04
};
typedef uint8_t network_errorCode;

/*! \brief tries to connect to network until successful
 *
 * Tries to connect to the network until successful. Is successful if there is already a connection.
 *
 * \param credentials network credentials (SSID, password)
 */
network_errorCode network_TryToConnectToNetworkUntilSuccessful(NetworkCredentials_t credentials);

/*! \brief tries to connect to network
 *
 * Tries to connect to the host broker. Is successful if there is already a connection.
 *
 * \param credentials network credentials (SSID, password)
 * \return true if connection successful or already connected, otherwise false
 */
network_errorCode network_ConnectToNetwork(NetworkCredentials_t credentials);

void network_DisconnectFromNetwork(void);

void network_checkConnection(void);

#endif /* ENV5_NETWORK_HEADER */
