#include <stdint.h>

#include "FpgaConfigDatahandler.h"

uint8_t readValueNum = 0;
uint8_t dataSent[FLASH_PAGE_SIZE * 4];
uint8_t fpgaConfigHandlerNumWrittenBlock=0;
void fpgaConfigHandlerSetAddress(uint32_t addr) {
    address = addr;
}
void fpgaConfigHandlerSetConfigSize(uint32_t conf_size) {
    config_size = conf_size;
}
uint16_t fpgaConfigHandlerReceiveData(uint8_t *block, uint16_t bufferLength) {
    readValueNum = 0;
    for (uint16_t i = 0; i < bufferLength; i++) {
        block[i] = i;
    }
    return 0;
}
void fpgaConfigHandlerReadValue(uint32_t *destination) {
    if (readValueNum == 0) {
        *destination = address;
    } else if (readValueNum == 1) {
        *destination = config_size;
    }
    readValueNum++;
}
void fpgaConfigHandlerSendAck(){

}
void fpgaConfigHandlerSendDataAck(uint32_t *data){

}

void fpgaConfigHandlerSendData(uint8_t *data, uint16_t length){
    for (uint16_t i = 0; i < length; i++) {
        dataSent[i+fpgaConfigHandlerNumWrittenBlock*FLASH_PAGE_SIZE] = data[i];
    }
    fpgaConfigHandlerNumWrittenBlock++;


}
