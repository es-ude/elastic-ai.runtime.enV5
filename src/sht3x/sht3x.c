#define SOURCE_FILE "src/sht3x/sht3x.c"

#include "sht3x_public.h"
#include "sht3x_internal.h"
#include "i2c.h"
#include "pico/time.h"
#include "common.h"

/* region VARIABLES */

static sht3x_i2c_sensorConfiguration sensorConfiguration; /*!< i2c configuration for the sensor */

/* endregion */

/* region HEADER FUNCTION IMPLEMENTATIONS */

sht3x_errorCode sht3x_init ( i2c_inst_t * i2cHost )
    {
        /* save i2c access for later usage */
        sensorConfiguration.i2c_host          = i2cHost;
        sensorConfiguration.i2c_slave_address = 0x44;
        
        I2C_Init ( sensorConfiguration.i2c_host, ( 100 * 1000 ), 0, 1 );
        
        /* sleep to make sure the sensor is fully initialized */
        sleep_ms ( 2 );
        
        
        /* check if SHT3X is on Bus by requesting serial number without processing */
        uint8_t sizeOfCommandBuffer = 2;
        uint8_t commandBuffer[sizeOfCommandBuffer];
        commandBuffer[ 0 ] = ( SHT3X_CMD_READ_STATUS >> 8 );
        commandBuffer[ 1 ] = ( SHT3X_CMD_READ_STATUS & 0xFF );
        
        /* if i2c returns error -> sensor not available on bus */
        I2C_ErrorCode errorCode = I2C_WriteCommand ( commandBuffer, sizeOfCommandBuffer, sensorConfiguration.i2c_slave_address, sensorConfiguration.i2c_host );
        if ( errorCode != I2C_NO_ERROR )
            {
                return SHT3X_INIT_ERROR;
            }
        
        return SHT3X_NO_ERROR;
    }

sht3x_errorCode sht3x_readSerialNumber ( uint32_t * serialNumber )
    {
        uint8_t sizeOfRequestBuffer = 6;
        uint8_t requestBuffer[sizeOfRequestBuffer];
        
        sht3x_errorCode errorCode = sendRequestToSensor ( SHT3X_CMD_READ_SERIALNUMBER );
        if ( errorCode != SHT3X_NO_ERROR )
            {
                return errorCode;
            }
        
        errorCode = receiveDataFromSensor ( requestBuffer, sizeOfRequestBuffer );
        if ( errorCode != SHT3X_NO_ERROR )
            {
                return errorCode;
            }
        
        errorCode = performChecksumCheck ( requestBuffer, sizeOfRequestBuffer );
        if ( errorCode != SHT3X_NO_ERROR )
            {
                return errorCode;
            }
        
        * serialNumber = ( ( uint32_t ) ( requestBuffer[ 0 ] ) << 24 ) | ( ( uint32_t ) ( requestBuffer[ 1 ] ) << 16 ) | ( ( uint32_t ) ( requestBuffer[ 3 ] )
                << 8 ) | ( uint32_t ) ( requestBuffer[ 4 ] );
        
        return SHT3X_NO_ERROR;
    }

sht3x_errorCode sht3x_readStatusRegister ( sht3x_statusRegister * statusRegister )
    {
        uint8_t sizeOfResponseBuffer = 3;
        uint8_t responseBuffer[sizeOfResponseBuffer];
        
        sht3x_errorCode sht3XErrorCode = sendRequestToSensor ( SHT3X_CMD_READ_STATUS );
        if ( sht3XErrorCode != SHT3X_NO_ERROR )
            {
                return sht3XErrorCode;
            }
        
        sht3XErrorCode = receiveDataFromSensor ( responseBuffer, sizeOfResponseBuffer );
        if ( sht3XErrorCode != SHT3X_NO_ERROR )
            {
                return sht3XErrorCode;
            }
        
        sht3XErrorCode = performChecksumCheck ( responseBuffer, sizeOfResponseBuffer );
        if ( sht3XErrorCode != SHT3X_NO_ERROR )
            {
                return sht3XErrorCode;
            }
        
        statusRegister->config = ( ( uint16_t ) ( responseBuffer[ 0 ] ) << 8 ) | ( uint16_t ) ( responseBuffer[ 1 ] );
        
        return SHT3X_NO_ERROR;
    }

sht3x_errorCode sht3x_getTemperature ( float * temperature )
    {
        uint8_t sizeOfResponseBuffer = 3;
        uint8_t responseBuffer[sizeOfResponseBuffer];
        
        sht3x_errorCode errorCode = sendRequestToSensor ( SHT3X_CMD_MEASURE_CLOCKSTRETCH_LOW );
        if ( errorCode != SHT3X_NO_ERROR )
            {
                return errorCode;
            }
        
        errorCode = receiveDataFromSensor ( responseBuffer, sizeOfResponseBuffer );
        if ( errorCode != SHT3X_NO_ERROR )
            {
                return errorCode;
            }
        
        errorCode = performChecksumCheck ( responseBuffer, sizeOfResponseBuffer );
        if ( errorCode != SHT3X_NO_ERROR )
            {
                return errorCode;
            }
        
        uint16_t rawTemperature = ( ( uint16_t ) ( responseBuffer[ 0 ] ) << 8 ) | ( uint16_t ) ( responseBuffer[ 1 ] );
        * temperature = calculateTemperature ( rawTemperature );
        
        return SHT3X_NO_ERROR;
    }

sht3x_errorCode sht3x_getHumidity ( float * humidity )
    {
        uint8_t sizeOfResponseBuffer = 6;
        uint8_t responseBuffer[sizeOfResponseBuffer];
        
        sht3x_errorCode errorCode = sendRequestToSensor ( SHT3X_CMD_MEASURE_CLOCKSTRETCH_LOW );
        if ( errorCode != SHT3X_NO_ERROR )
            {
                return errorCode;
            }
        
        errorCode = receiveDataFromSensor ( responseBuffer, sizeOfResponseBuffer );
        if ( errorCode != SHT3X_NO_ERROR )
            {
                return errorCode;
            }
        
        errorCode = performChecksumCheck ( responseBuffer, sizeOfResponseBuffer );
        if ( errorCode != SHT3X_NO_ERROR )
            {
                return errorCode;
            }
        
        uint16_t rawHumidity = ( ( uint16_t ) ( responseBuffer[ 3 ] ) << 8 ) | ( uint16_t ) ( responseBuffer[ 4 ] );
        * humidity = calculateHumidity ( rawHumidity );
        
        return SHT3X_NO_ERROR;
    }

sht3x_errorCode sht3x_getTemperatureAndHumidity ( float * temperature, float * humidity )
    {
        uint8_t sizeOfResponseBuffer = 6;
        uint8_t responseBuffer[sizeOfResponseBuffer];
        
        sht3x_errorCode errorCode = sendRequestToSensor ( SHT3X_CMD_MEASURE_CLOCKSTRETCH_LOW );
        if ( errorCode != SHT3X_NO_ERROR )
            {
                return errorCode;
            }
        
        errorCode = receiveDataFromSensor ( responseBuffer, sizeOfResponseBuffer );
        if ( errorCode != SHT3X_NO_ERROR )
            {
                return errorCode;
            }
        
        errorCode = performChecksumCheck ( responseBuffer, sizeOfResponseBuffer );
        if ( errorCode != SHT3X_NO_ERROR )
            {
                return errorCode;
            }
        
        uint16_t rawTemperature = ( ( uint16_t ) ( responseBuffer[ 0 ] ) << 8 ) | ( uint16_t ) ( responseBuffer[ 1 ] );
        * temperature = calculateTemperature ( rawTemperature );
        
        uint16_t rawHumidity = ( ( uint16_t ) ( responseBuffer[ 3 ] ) << 8 ) | ( uint16_t ) ( responseBuffer[ 4 ] );
        * humidity = calculateHumidity ( rawHumidity );
        
        return SHT3X_NO_ERROR;
    }

sht3x_errorCode sht3x_readMeasurementBuffer ( float * temperature, float * humidity )
    {
        uint8_t sizeOfResponseBuffer = 6;
        uint8_t responseBuffer[sizeOfResponseBuffer];
        
        sht3x_errorCode errorCode = sendRequestToSensor ( SHT3X_CMD_FETCH_DATA );
        if ( errorCode != SHT3X_NO_ERROR )
            {
                return errorCode;
            }
        
        errorCode = receiveDataFromSensor ( responseBuffer, sizeOfResponseBuffer );
        if ( errorCode != SHT3X_NO_ERROR )
            {
                return errorCode;
            }
        
        errorCode = performChecksumCheck ( responseBuffer, sizeOfResponseBuffer );
        if ( errorCode != SHT3X_NO_ERROR )
            {
                return errorCode;
            }
        
        uint16_t rawTemperature = ( ( uint16_t ) ( responseBuffer[ 0 ] ) << 8 ) | ( uint16_t ) ( responseBuffer[ 1 ] );
        * temperature = calculateTemperature ( rawTemperature );
        
        uint16_t rawHumidity = ( ( uint16_t ) ( responseBuffer[ 3 ] ) << 8 ) | ( uint16_t ) ( responseBuffer[ 4 ] );
        * humidity = calculateHumidity ( rawHumidity );
        
        return SHT3X_NO_ERROR;
    }

sht3x_errorCode sht3x_enableHeater ( void )
    {
        return sendRequestToSensor ( SHT3X_CMD_HEATER_ENABLE );
    }

sht3x_errorCode sht3x_disableHeater ( void )
    {
        return sendRequestToSensor ( SHT3X_CMD_HEATER_DISABLE );
    }

sht3x_errorCode sht3x_softReset ( void )
    {
        return sendRequestToSensor ( SHT3X_CMD_SOFT_RESET );
    }

/* endregion */

/* region STATIC FUNCTION IMPLEMENTATIONS */

static sht3x_errorCode sendRequestToSensor ( sht3x_command command )
    {
        uint8_t sizeOfCommandBuffer = 2;
        uint8_t commandBuffer[sizeOfCommandBuffer];
        commandBuffer[ 0 ] = ( command >> 8 );
        commandBuffer[ 1 ] = ( command & 0xFF );
        
        PRINT_DEBUG( "requesting data from sensor" )
        sht3x_errorCode errorCode = I2C_WriteCommand ( commandBuffer, sizeOfCommandBuffer, sensorConfiguration.i2c_slave_address, sensorConfiguration.i2c_host
                                                     );
        
        if ( errorCode == I2C_NO_ERROR )
            {
                return SHT3X_NO_ERROR;
            }
        
        PRINT_DEBUG( "sending request failed, error was %02X", errorCode )
        return SHT3X_SEND_COMMAND_ERROR;
    }

static sht3x_errorCode receiveDataFromSensor ( uint8_t * responseBuffer, uint8_t sizeOfResponseBuffer )
    {
        PRINT_DEBUG( "receiving data from sensor" )
        sht3x_errorCode errorCode = I2C_ReadData ( responseBuffer, sizeOfResponseBuffer, sensorConfiguration.i2c_slave_address, sensorConfiguration.i2c_host );
        
        if ( errorCode == I2C_NO_ERROR )
            {
                return SHT3X_NO_ERROR;
            }
        
        PRINT_DEBUG( "receiving data failed, error was %02X", errorCode )
        return SHT3X_RECEIVE_DATA_ERROR;
    }

static sht3x_errorCode performChecksumCheck ( const uint8_t * responseBuffer, uint8_t sizeOfResponseBuffer )
    {
        PRINT_DEBUG( "performing checksum check" )
        uint8_t numberOfTriplets = sizeOfResponseBuffer / 3;
        uint8_t group            = 0;
        
        while ( group < numberOfTriplets )
            {
                uint8_t startIndex          = group * 3;
                uint8_t calculatedChecksum  = calculateChecksum ( & ( responseBuffer[ startIndex ] ) );
                uint8_t transmittedChecksum = responseBuffer[ startIndex + 2 ];
                
                if ( transmittedChecksum != calculatedChecksum )
                    {
                        PRINT_DEBUG( "checksum failed, input: %i, calculated: %i", transmittedChecksum, calculatedChecksum )
                        return SHT3X_CHECKSUM_ERROR;
                    }
                
                group ++;
            }
        
        PRINT_DEBUG( "checksum passed" )
        return SHT3X_NO_ERROR;
    }

static uint8_t calculateChecksum ( const uint8_t * dataBuffer )
    {
        uint8_t checksum = 0xFF;
        
        for ( uint8_t byteCounter = 0 ; byteCounter < 2 ; byteCounter ++ )
            {
                checksum ^= ( dataBuffer[ byteCounter ] );
                for ( uint8_t bitMask = 8 ; bitMask > 0 ; -- bitMask )
                    {
                        if ( checksum & 0x80 )
                            {
                                checksum = ( checksum << 1 ) ^ CRC8_POLYNOMIAL;
                            }
                        else
                            {
                                checksum = ( checksum << 1 );
                            }
                    }
            }
        
        return checksum;
    }

static float calculateTemperature ( uint16_t rawValue )
    {
        float temperature = ( 175.0f * ( ( float ) rawValue / DENOMINATOR ) ) - 45.0f;
        PRINT_DEBUG( "calculate the temperature, temperature=%f", temperature )
        return temperature;
    }

static float calculateHumidity ( uint16_t rawValue )
    {
        float humidity = 100.0f * ( ( float ) rawValue / DENOMINATOR );
        PRINT_DEBUG( "calculating humidity, humidity=%f", humidity )
        return humidity;
    }

/* endregion */

