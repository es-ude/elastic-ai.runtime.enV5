#ifndef ENV5_USB_PROTOCOL_TYPEDEFS_HEADER
#define ENV5_USB_PROTOCOL_TYPEDEFS_HEADER

#include <stddef.h>
#include <stdint.h>

typedef enum usbProtocolErrorCodes {
    USB_PROTOCOL_OKAY = 0x00,
    USB_PROTOCOL_ERROR_NOT_INITIALIZED,
    USB_PROTOCOL_ERROR_INVALID_ID,
    USB_PROTOCOL_ERROR_NULL_POINTER,
    USB_PROTOCOL_ERROR_HANDLE_NOT_SET,
    USB_PROTOCOL_ERROR_CHECKSUM_FAILED,
} usbProtocolErrorCodes_t;
/*!
 * @brief function handling data input
 *
 * @param[out] byteRead buffer to store received byte
 *
 * @returns error code
 * @retval 0 no error occurred
 */
typedef usbProtocolErrorCodes_t (*usbProtocolReadData)(uint8_t *byteRead);

/*!
 * @brief function handling data output
 *
 * @param[in] bytesToSend buffer storing the data to send
 * @param[in] numberOfBytesToSend length of the data buffer in Bytes
 *
 * @returns error code
 * @retval 0 no error occurred
 */
typedef usbProtocolErrorCodes_t (*usbProtocolSendData)(uint8_t *bytesToSend,
                                                       size_t numberOfBytesToSend);

/*!
 * @brief function providing the prototype for new command handles
 *
 * @param[in] data pointer to the buffer holding the function payload;
 * @param[in] length size of @p data in Bytes
 */
typedef void (*usbProtocolCommandHandle)(const uint8_t *data, size_t length);

typedef void *usbProtocolReceiveBuffer;

#endif // ENV5_USB_PROTOCOL_TYPEDEFS_HEADER
