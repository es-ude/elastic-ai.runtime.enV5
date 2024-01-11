#ifndef ENV5_GPIO_HEADER
#define ENV5_GPIO_HEADER

#include <stdbool.h>
#include <stdint.h>

typedef enum gpioErrorCode {
    GPIO_NO_ERROR = 0x00,
    GPIO_GENERAL_ERROR = 0x01,
    GPIO_INIT_ERROR = 0x10,
} gpioErrorCode_t;

typedef uint8_t gpioPin_t;

typedef enum gpioPinFunction {
    GPIO_FUNCTION_XIP = 0,
    GPIO_FUNCTION_SPI = 1,
    GPIO_FUNCTION_UART = 2,
    GPIO_FUNCTION_I2C = 3,
    GPIO_FUNCTION_PWM = 4,
    GPIO_FUNCTION_SIO = 5,
    GPIO_FUNCTION_PIO0 = 6,
    GPIO_FUNCTION_PIO1 = 7,
    GPIO_FUNCTION_GPCK = 8,
    GPIO_FUNCTION_USB = 9,
    GPIO_FUNCTION_NULL = 0x1f,
} gpioPinFunction_t;

typedef enum gpioPinMode {
    GPIO_PIN_HIGH = 1,
    GPIO_PIN_LOW = 0,
} gpioPinMode_t;

typedef enum gpioDirection {
    GPIO_INPUT = 0,
    GPIO_OUTPUT = 1,
} gpioDirection_t;

/*!
 * @brief Initialize GPIO for input or output.
 *        Has to be called before the GPIO can be used!
 * @param pin GPIO
 * @param direction input or output
 * @return 0x00 if no error occurred
 */
gpioErrorCode_t gpioInitPin(gpioPin_t pin, gpioDirection_t direction);

/*!
 * @brief Disable GPIO
 * @param pin GPIO
 * @return 0x00 if no error occurred
 */
gpioErrorCode_t gpioDisablePin(gpioPin_t pin);

/*!
 * @brief Set the Function of the GPIO
 * @param pin GPIO
 * @param function Function to USE (I2C, SPI, ...)
 * @return 0x00 if no error occurred
 */
gpioErrorCode_t gpioSetPinFunction(gpioPin_t pin, gpioPinFunction_t function);

/*!
 * @brief Enable pull-up resistor of GPIO
 * @param pin GPIO
 * @return 0x00 if no error occurred
 */
gpioErrorCode_t gpioEnablePullUp(gpioPin_t pin);

/*!
 * @brief Set GPIO output to HIGH/LOW
 * @param pin GPIO
 * @param mode 1=HIGH / 0=LOW
 */
void gpioSetPin(gpioPin_t pin, gpioPinMode_t mode);

/*!
 * @brief Get GPIO input value
 * @param pin GPIO
 * @return 0 if LOW / else High
 */
bool gpioGetPin(gpioPin_t pin);

#endif /* ENV5_GPIO_HEADER */
