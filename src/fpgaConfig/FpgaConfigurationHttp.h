

#ifndef ENV5_FPGACONFIGURATIONHTTP_H
#define ENV5_FPGACONFIGURATIONHTTP_H
#include "FpgaConfiguration.h"
#include "HTTP.h"
#include <stdint.h>
#include "FileIterator.h"
void setCommunication(HttpResponse_t *(*getDataFun)(uint32_t));
configErrorCode_t configure(uint32_t startAddress, uint32_t sizeOfConfiguration);
void writeConfigurationBlock(uint32_t address, dataBlock_t* dataBlock);
HttpResponse_t *dataReceive(uint32_t address);
#endif // ENV5_FPGACONFIGURATIONHTTP_H
