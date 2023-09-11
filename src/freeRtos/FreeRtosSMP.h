#ifndef ENV5_FREERTOSSMP_H
#define ENV5_FREERTOSSMP_H

#include "FreeRTOS.h"
#include "task.h"

void vApplicationMallocFailedHook( void );
void vApplicationIdleHook( void );
void vApplicationStackOverflowHook( TaskHandle_t pxTask, char *pcTaskName );
void vApplicationTickHook( void );

#endif // ENV5_FREERTOSSMP_H
