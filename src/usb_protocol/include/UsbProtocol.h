#ifndef ENV5_USB_PROTOCOL_HEADER
#define ENV5_USB_PROTOCOL_HEADER

#include <stddef.h>
#include <stdint.h>

typedef enum usbProtocolErrorCodes {
    USB_PROTOCOL_OKAY = 0x00,
    USB_PROTOCOL_ERROR_INVALID_ID,
    USB_PROTOCOL_ERROR_NULL_POINTER,
    USB_PROTOCOL_ERROR_CHECKSUM_FAILED,
} usbProtocolErrorCodes_t;

/*!
 * @brief function providing the prototype for new command handles
 *
 * @param[in] data pointer to the buffer holding the function payload;
 * @param[in] length size of @p data in Bytes
 */
typedef void (*usbProtocolCommandHandle)(uint8_t *data, size_t length);

/*!
 * @brief function handling data input
 *
 * @param[out] byteRead buffer to store received byte
 *
 * @returns error code
 * @retval 0 no error occurred
 */
typedef usbProtocolErrorCodes_t (*usbProtocolReadCommand)(uint8_t *byteRead);

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

typedef struct usbProtocolReceivedCommand {
    uint8_t command;
    uint8_t *payload;
    size_t payloadLength;
} usbProtocolReceivedCommand_t;

/*!
 * @brief initializes the usb protocol handler
 *
 * @param[in] readFunction function providing input for the protocol handler
 * @param[in] sendFunction function allowing the handler to send a response
 *
 * @throws USB_PROTOCOL_ERROR_NULL_POINTER a function provided is not defined
 */
void usbProtocolInit(usbProtocolReadCommand readFunction, usbProtocolSendData sendFunction);

/*!
 * @brief register a new command
 *
 * @param[in]identifier identifier in range 128-255
 * @param[in]command pointer to function handling the command
 *
 * @throws USB_PROTOCOL_ERROR_INVALID_ID @p identifier out of range
 * @throws USB_PROTOCOL_ERROR_NULL_POINTER command is a null pointer
 */
void usbProtocolAddCommand(size_t identifier, usbProtocolCommandHandle command);

/*!
 * @brief function waiting for data (command + payload + checksum) (BLOCKING)
 *
 * @returns pointer to buffer holding the received command and payload
 *
 * @throws USB_PROTOCOL_ERROR_CHECKSUM_FAILED checksum mismatch
 */
usbProtocolReceivedCommand_t *usbProtocolWaitForCommand(void);

/*!
 * @brief function handling the received command
 *
 * @param[in] commandToHandle command alongside data to handle!
 */
void usbProtocolHandleCommand(usbProtocolReceivedCommand_t *commandToHandle);

#endif // ENV5_USB_PROTOCOL_HEADER
