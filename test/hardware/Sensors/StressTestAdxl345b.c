#define SOURCE_FILE "ADXL345-StressTest"

#include "Adxl345b.h"
#include "Common.h"
#include "Esp.h"
#include "FreeRtosTaskWrapper.h"
#include "HardwaretestHelper.h"
#include "Protocol.h"
#include "enV5HwController.h"

// pico-sdk headers
#include "hardware/i2c.h"
#include "pico/bootrom.h"
#include "pico/stdlib.h"
#include "hardware/watchdog.h"

#define G_VALUE_BATCH_SECONDS 1
#define STESSTEST_INTENSITY 0

int fibonacci(int n);
int factorial(int n);
int ackermann(int m, int n);
void t_test_only_adxl();
void t_stressTest_adxlWithFreeRTOS();
static int veryHighFibonacci = 14930352;
void init_adxl();



void init_adxl(){
    // wait for user console to connect
    while ((!stdio_usb_connected( ))) { }
    sleep_ms( 500);
    
    /* initialize ADXL345B sensor */
    PRINT( "START INIT" );
    adxl345bErrorCode_t errorCode;
    while (1) {
      errorCode = adxl345bInit(i2c1, ADXL345B_I2C_ALTERNATE_ADDRESS);
      if (errorCode == ADXL345B_NO_ERROR) {
        PRINT( "Initialised ADXL345B." );
        break;
      }
      PRINT( "Initialise ADXL345B failed; adxl345b_ERROR: %02X", errorCode );
      sleep_ms( 500 );
    }
    adxl345bWriteConfigurationToSensor(ADXL345B_REGISTER_BW_RATE, ADXL345B_BW_RATE_400);
    adxl345bChangeMeasurementRange(ADXL345B_16G_RANGE);
    
  }
int fibonacci(int n) {
    if (n <= 1)
      return n;
    else
      return fibonacci(n-1) + fibonacci(n-2);
  }

int factorial(int n) {
    if (n == 0)
      return 1;
    else
      return n * factorial(n-1);
  }

int ackermann(int m, int n) {
    if (m == 0)
      return n + 1;
    else if (m > 0 && n == 0)
      return ackermann(m - 1, 1);
    else
      return ackermann(m - 1, ackermann(m, n - 1));
  }

_Noreturn void easyTask(void);
_Noreturn void mediocreTask(void);
_Noreturn void hardTask(void);
_Noreturn void adxlTask(void);



_Noreturn void easyTask(void) {
    uint8_t counter = 0;
    while(true){
      counter++;
      freeRtosTaskWrapperTaskSleep( 10 );
    }
  };

_Noreturn void mediocreTask(void) {
    PRINT("START MEDIOCRE TASK");
    int result;
    int counterN = 1;
    while (true) {
      result = factorial( counterN );
      if (counterN % 5 != 0) {
        counterN++;
      } else {
        freeRtosTaskWrapperTaskSleep( 10 );
        counterN++;
        PRINT( "Factorial: %d\n", result );
        if (counterN >= 100) { counterN = 0; }
      }
      
    }
  };


_Noreturn void hardTask(void) {
    int result;
    int counterN = 1;
    while (true) {
      result = fibonacci( counterN );
      PRINT( "fibonacci: %d\n", result );
      if (result >= veryHighFibonacci) { counterN = 0; }
      if (counterN % 10 != 0) {
        counterN++;
      } else {
        freeRtosTaskWrapperTaskSleep( 10);
        counterN++;
      }
    }
  };

void publishTestData() {
    protocolPublishData("testPub", "testData00");
 }

void _Noreturn mqttTask(void) {
    PRINT("=== STARTING TEST ===");

    connectToNetwork();
    connectToMQTT();

    uint64_t messageCounter = 0;
    while (1) {
      publishTestData();
      messageCounter++;
      freeRtosTaskWrapperTaskSleep(1000);
    }
  }


_Noreturn void adxlTask(void){
    uint32_t interval = G_VALUE_BATCH_SECONDS * 1000000;
    PRINT("STARTING...");
    
    while (true) {
      
      int count = 0;
      uint32_t startTime = time_us_32( );
      while (startTime + interval >= time_us_32()) {
        float xAxis, yAxis, zAxis;
        adxl345bReadMeasurements(&xAxis, &yAxis, &zAxis);
        PRINT("X: %f, Y: %f, Z: %f", xAxis,yAxis, zAxis);
        count++;
        
//        //scheduler NEEDS a sleep if tasks on same core. minimal sleep is 10ms
//        if(count >= 3200){
//          freeRtosTaskWrapperTaskSleep(10);
//        }
      }
      
      PRINT("Got %i samples.", count);
    }
  }
  
  
_Noreturn void enterBootModeTask(void) {
    while (true) {
      if (getchar_timeout_us(10) == 'r' || !stdio_usb_connected()) {
        // enter boot mode if 'r' was received
        reset_usb_boot(0, 0);
      }
      
      // watchdog update needs to be performed frequent, otherwise the device will crash
      watchdog_update();
      freeRtosTaskWrapperTaskSleep(1000);
    }
  }

  
void t_test_only_adxl(){
    adxlTask();
  }

void t_stressTest_adxlWithFreeRTOS(){
    freeRtosTaskWrapperRegisterTask(adxlTask, "adxlTask", 1, FREERTOS_CORE_0);
}

void t_stressTest_easy(){
    freeRtosTaskWrapperRegisterTask(adxlTask, "adxlTask", 1, FREERTOS_CORE_0);
    freeRtosTaskWrapperRegisterTask(easyTask, "easyTask", 1,
                                    FREERTOS_CORE_1);
}


void t_stressTest_mediocre(){
    freeRtosTaskWrapperRegisterTask(adxlTask, "adxlTask", 1, FREERTOS_CORE_0);
    freeRtosTaskWrapperRegisterTask(mediocreTask, "mediocreTask", 1, FREERTOS_CORE_1);
}


void t_stressTest_hard(){
    freeRtosTaskWrapperRegisterTask(adxlTask, "adxlTask", 1, FREERTOS_CORE_0);
    freeRtosTaskWrapperRegisterTask(hardTask, "hardTask", 1, FREERTOS_CORE_1);
}

void t_stressTest_withMQTT(){
    freeRtosTaskWrapperRegisterTask(adxlTask, "adxlTask", 1, FREERTOS_CORE_0);
    freeRtosTaskWrapperRegisterTask(mqttTask, "mqttTask", 1, FREERTOS_CORE_1);
}

int main(void) {
    // Did we crash last time -> reboot into boot rom mode
    if (watchdog_enable_caused_reboot()) {
      reset_usb_boot(0, 0);
    }
    
    env5HwInit();
    
    // init stdio and esp
    stdio_init_all();
    while ((!stdio_usb_connected())) {}
    
    //das hier passiert vorher noch im Ballchallenge-Repo. Erkenne keinen Zusammenhang.
    //espInit();
    // initialize WiFi and MQTT broker
    //networkTryToConnectToNetworkUntilSuccessful(networkCredentials);
    //mqttBrokerConnectToBrokerUntilSuccessful(mqttHost, "eip://uni-due.de/es", "enV5");
    
    init_adxl();
//#if STRESSTEST_INTENSITY == 0
//#elif
//#endif
    if (STESSTEST_INTENSITY == 0) {
      t_test_only_adxl();
    }
    else {
      if (STESSTEST_INTENSITY == 1) {
        t_stressTest_adxlWithFreeRTOS( );
      } else if (STESSTEST_INTENSITY == 2) {
        t_stressTest_easy( );
      } else if (STESSTEST_INTENSITY == 3) {
        t_stressTest_mediocre( );
      } else if (STESSTEST_INTENSITY == 4) {
        t_stressTest_hard( );
      } else if (STESSTEST_INTENSITY == 5){
        t_stressTest_withMQTT();
      }
      freeRtosTaskWrapperStartScheduler( );
    }
}

  