#ifndef ENV5_NETWORK_HEADER
#define ENV5_NETWORK_HEADER

#include <stdbool.h>
#include <stdint.h>

/*! \b INFORMATION
 * \n
 * special characters like `,`, `"`,`\` needs to be escaped with an `\`.
 * Don't forget to escape the `\` in C Strings!
 * \n
 * if the password is "ajk"jkl,23" the string needs to be "ajk\\\"jkl\\\,23"
 */
struct networkCredentials {
    char *ssid;
    char *password;
};
typedef struct networkCredentials networkCredentials_t;

enum {
    NETWORK_NO_ERROR = 0x01,
    NETWORK_ESTABLISH_CONNECTION_FAILED = 0x02,
    NETWORK_ESP_CHIP_FAILED = 0x03,
    NETWORK_WIFI_ALREADY_CONNECTED = 0x04
};
typedef uint8_t networkErrorCode_t;

/*! \brief tries to connect to network until successful
 *
 * Tries to connect to the network until successful. Is successful if there is already a connection.
 *
 * \param credentials network credentials (SSID, password)
 */
networkErrorCode_t networkTryToConnectToNetworkUntilSuccessful();

/*! \brief tries to connect to network
 *
 * Tries to connect to the host broker. Is successful if there is already a connection.
 *
 * \param credentials network credentials (SSID, password)
 * \return true if connection successful or already connected, otherwise false
 */
networkErrorCode_t networkConnectToNetwork(networkCredentials_t credentials);

void networkDisconnectFromNetwork(void);

networkErrorCode_t networkCheckConnection(void);

#endif /* ENV5_NETWORK_HEADER */
