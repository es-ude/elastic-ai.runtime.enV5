#ifndef ENV5_HARDWARE_CONTROLLER_HEADER
#define ENV5_HARDWARE_CONTROLLER_HEADER

#include <stdbool.h>

void env5HwInit();

void env5HwLedsInit();
void env5HwLedsAllOff();
void env5HwLedsAllOn();

void env5HwFpgaInit();
void env5HwFpgaPowersOn();
void env5HwFpgaPowersOff();
void env5HwFpgaReset(bool enable);

#endif // ENV5_HARDWARE_CONTROLLER_HEADER
