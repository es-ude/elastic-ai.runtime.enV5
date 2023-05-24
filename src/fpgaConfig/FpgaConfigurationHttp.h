#ifndef ENV5_FPGACONFIGURATIONHTTP_H
#define ENV5_FPGACONFIGURATIONHTTP_H

#include "FpgaConfiguration.h"
#include "HTTP.h"
#include <stdint.h>

void setCommunication(HttpResponse_t *(*getDataFun)(uint32_t));
configErrorCode_t configure(uint32_t startAddress, uint32_t sizeOfConfiguration);

HttpResponse_t *dataReceive(uint32_t address);

#endif // ENV5_FPGACONFIGURATIONHTTP_H
