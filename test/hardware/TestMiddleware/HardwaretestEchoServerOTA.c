#define SOURCE_FILE "HWTEST-MIDDLEWARE"

#include "Common.h"
#include "Esp.h"
#include "Flash.h"
#include "FpgaConfigurationHandler.h"
#include "FreeRtosTaskWrapper.h"
#include "MqttBroker.h"
#include "Network.h"
#include "NetworkConfiguration.h"
#include "Protocol.h"
#include "echo_server.h"
#include "enV5HwController.h"
#include "middleware.h"
#include "pico/bootrom.h"

#include <hardware/spi.h>
#include <pico/stdlib.h>

#include <stdbool.h>
#include <stdio.h>
#include <string.h>

typedef struct downloadRequest {
    char *url;
    size_t fileSizeInBytes;
    size_t startAddress;
} downloadRequest_t;
downloadRequest_t *downloadRequest = NULL;

bool flashing = false;

spi_t spiConfiguration = {
   .spi = spi0, .baudrate = 5000000, .misoPin = 0, .mosiPin = 3, .sckPin = 2};
uint8_t csPin = 1;

uint32_t configStartAddress = 0x00000000;

static void initHardware() {
   // initialize the serial output
   stdio_init_all();
   while ((!stdio_usb_connected())) {
       // wait for serial connection
   }

   // connect to Wi-Fi network
   espInit();
   networkTryToConnectToNetworkUntilSuccessful(networkCredentials);
   mqttBrokerConnectToBrokerUntilSuccessful(mqttHost, "eip://uni-due.de/es", "enV5");
   
   // enable QXI interface to the FPGA
   middlewareInit();

   // initialize the Flash and FPGA
   flashInit(&spiConfiguration, csPin);
   env5HwInit();
   fpgaConfigurationHandlerInitialize();
   env5HwFpgaPowersOff();
}

_Noreturn static void runTest() {
   freeRtosTaskWrapperTaskSleep(7500);
   env5HwLedsAllOn();
   int8_t counter = 0;
   while (true) {

       if (flashing) {
           freeRtosTaskWrapperTaskSleep(500);
           continue;
       }
       
       uint8_t return_val = (uint8_t)echo_server_echo(counter);

       PRINT("%i, %i", return_val, counter)
       
       if (return_val == counter + 1) {
           env5HwLedsAllOff();
           sleep_ms(500);
       }

       env5HwLedsAllOn();
       sleep_ms(500);

       counter++;
   }
}

void receiveDownloadBinRequest(posting_t posting) {
   PRINT("RECEIVED FLASH REQUEST")
   // get download request
   char *urlStart = strstr(posting.data, "URL:") + 4;
   char *urlEnd = strstr(urlStart, ";") - 1;
   size_t urlLength = urlEnd - urlStart + 1;
   char *url = malloc(urlLength);
   memcpy(url, urlStart, urlLength);
   url[urlLength - 1] = '\0';
   char *sizeStart = strstr(posting.data, "SIZE:") + 5;
   char *endSize = strstr(sizeStart, ";") - 1;
   size_t length = strtol(sizeStart, &endSize, 10);
   
   char *positionStart = strstr(posting.data, "POSITION:") + 9;
   char *positionEnd = strstr(positionStart, ";") - 1;
   size_t position = strtol(positionStart, &positionEnd, 10);

   downloadRequest = malloc(sizeof(downloadRequest_t));
   downloadRequest->url = url;
   downloadRequest->fileSizeInBytes = length;
   downloadRequest->startAddress = position;
}

_Noreturn void fpgaTask(void) {
   /* What this function does:
     *   - add listener for download start command (MQTT)
     *      uart handle should only set flag -> download handled at task
     *   - download data from server and stored to flash
     *   - add listener for FPGA flashing command
     *   - trigger flash of FPGA
     *      handled in UART interrupt
    */
   
   freeRtosTaskWrapperTaskSleep(5000);
   protocolSubscribeForCommand("FLASH", (subscriber_t){.deliver = receiveDownloadBinRequest});

   PRINT("FPGA Ready ...")

   while (1) {
       if (downloadRequest == NULL) {
           freeRtosTaskWrapperTaskSleep(1000);
           continue;
       }
       flashing = true;
       
       PRINT("Starting to download bitfile...")
       
       env5HwFpgaPowersOff();

       PRINT_DEBUG("Download: position in flash: %i, address: %s, size: %i",
                   downloadRequest->startAddress, downloadRequest->url,
                   downloadRequest->fileSizeInBytes)

       fpgaConfigurationHandlerError_t configError =
           fpgaConfigurationHandlerDownloadConfigurationViaHttp(downloadRequest->url,
                                                                downloadRequest->fileSizeInBytes,
                                                                downloadRequest->startAddress);

       // clean artifacts
       free(downloadRequest->url);
       free(downloadRequest);
       downloadRequest = NULL;
       PRINT("Download finished!")

       if (configError != FPGA_RECONFIG_NO_ERROR) {
           protocolPublishCommandResponse("FLASH", false);
           PRINT("ERASE ERROR")
       } else {
           freeRtosTaskWrapperTaskSleep(10);
           env5HwFpgaPowersOn();
           PRINT("FPGA reconfigured")
           protocolPublishCommandResponse("FLASH", true);
       }
       flashing = false;
   }
}

int main() {
   initHardware();
   
   freeRtosTaskWrapperRegisterTask(fpgaTask, "fpgaTask", 0, FREERTOS_CORE_0);
   freeRtosTaskWrapperRegisterTask(runTest, "runTest", 0, FREERTOS_CORE_1);
   
   freeRtosTaskWrapperStartScheduler();
}
