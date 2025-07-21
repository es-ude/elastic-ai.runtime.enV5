#ifndef ENV5_USB_PROTOCOL_HEADER
#define ENV5_USB_PROTOCOL_HEADER

#include <stddef.h>

#include "Flash.h"

/* region TYPEDEFS */

typedef enum usbProtocolErrorCodes {
    USB_PROTOCOL_OKAY = 0x00,
    USB_PROTOCOL_ERROR_NOT_INITIALIZED,
    USB_PROTOCOL_ERROR_INVALID_ID,
    USB_PROTOCOL_ERROR_NULL_POINTER,
    USB_PROTOCOL_ERROR_HANDLE_NOT_SET,
    USB_PROTOCOL_ERROR_HANDLE_EXECUTION_FAILED,
    USB_PROTOCOL_ERROR_CHECKSUM_FAILED,
    USB_PROTOCOL_ERROR_READ_FAILED,
} usbProtocolErrorCodes_t;

/*!
 * @brief function handling data input
 *
 * @param[out] bytes buffer to store received bytes
 * @param[in] numberOfBytes size of the byte buffer
 *
 * @returns error code
 * @retval 0 no error occurred
 */
typedef usbProtocolErrorCodes_t (*usbProtocolReadData)(uint8_t *bytes, size_t numberOfBytes);

/*!
 * @brief function handling data output
 *
 * @param[in] bytes buffer storing the data to send
 * @param[in] numberOfBytes length of the data buffer in Bytes
 *
 * @returns error code
 * @retval 0 no error occurred
 */
typedef usbProtocolErrorCodes_t (*usbProtocolSendData)(uint8_t *bytes, size_t numberOfBytes);

typedef void *usbProtocolReceiveBuffer;

/* endregion TYPEDEFS */

/*!
 * @brief initializes the usb protocol handler
 *
 * @code
 * void usbProtocolInit(
 *      usbProtocolReadCommand readFunction,
 *      usbProtocolSendData sendFunction
 * );
 * @endcode
 *
 * @param[in] readFunction function providing input for the protocol handler
 * @param[in] sendFunction function allowing the handler to send a response
 * @param[in] flashConfiguration pointer to configuration for flash
 * @param[in] defaultCommandCreator function adding commands on init
 *
 * @throws USB_PROTOCOL_ERROR_NULL_POINTER a function provided is not defined
 */
void usbProtocolInit(
    usbProtocolReadData readFunction,
    usbProtocolSendData sendFunction,
    flashConfiguration_t *flashConfiguration,
    void *defaultCommandCreator);

/*!
 * @brief function waiting for data (command + payload + checksum) (BLOCKING)
 *
 * @returns pointer to buffer holding the received command and payload
 *
 * @throws USB_PROTOCOL_ERROR_CHECKSUM_FAILED checksum mismatch
 */
usbProtocolReceiveBuffer usbProtocolWaitForCommand(void);

/*!
 * @brief function handling the received command
 *
 * @param[in] buffer buffer with received data to be handled
 *
 * @throws USB_PROTOCOL_ERROR_NULL_POINTER if command id doesn't have a registered function
 * associated
 * @throws USB_PROTOCOL_ERROR_INVALID_ID if command id is not valid
 */
void usbProtocolHandleCommand(usbProtocolReceiveBuffer buffer);

#endif // ENV5_USB_PROTOCOL_HEADER
