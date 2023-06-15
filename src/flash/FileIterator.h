#ifndef ENV5_FILEITERATOR_H
#define ENV5_FILEITERATOR_H

#include <stdint.h>
#include <stdbool.h>
typedef struct dataBlock {
    uint32_t size;
    uint8_t *data;
} dataBlock_t;
typedef struct configurationBlock {
    uint32_t address;
    dataBlock_t* dataBlock;
} configurationBlock_t;


typedef struct file {
    volatile uint32_t address;
    uint8_t *data;
    volatile uint32_t bytes_left;
    volatile uint32_t chunk_size;
    configurationBlock_t* (*next)(struct file *);
    bool (*isEOF)(struct  file * );
    void (*freeFile)(struct file*);
   
} file_t;


file_t* initFile(uint32_t address, dataBlock_t *configuration, uint32_t chunk_size);
configurationBlock_t* next(file_t* file);
void freeFile(file_t* file);
bool isEOF(file_t* file);
void freeConfBlock(configurationBlock_t* configurationBlock) ;
#endif // ENV5_FILEITERATOR_H
