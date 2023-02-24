

#ifndef ENV5_FPGACONFIGURATIONHTTP_H
#define ENV5_FPGACONFIGURATIONHTTP_H
#include <stdint.h>

void setCommunication(uint8_t* (*getDataFun)(uint32_t));
uint8_t* configure();

uint8_t* dataReceive(uint32_t address);
#endif // ENV5_FPGACONFIGURATIONHTTP_H
