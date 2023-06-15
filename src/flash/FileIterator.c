#define SOURCE_FILE "FILEITERATOR"
#include "FileIterator.h"
#include "Common.h"
#include "FreeRtosTaskWrapper.h"
#include <string.h>
#include <stdlib.h>


file_t* initFile(uint32_t address, dataBlock_t* dataBlock , uint32_t chunk_size) {
    
    file_t* file= malloc(sizeof (file_t));
    file->address=address;
    file->data=dataBlock->data;
    file->bytes_left=dataBlock->size;
    file->next=&next;
    file->isEOF=&isEOF;
    file->freeFile=&freeFile;
    file->chunk_size=chunk_size;
    
    return file;
}

configurationBlock_t* next(file_t* file) {
    configurationBlock_t* configuration= malloc(sizeof(configurationBlock_t));
    configuration->dataBlock= malloc(sizeof(dataBlock_t));
    PRINT("bytes left %u, chunk size %u, address %u",file->bytes_left, file->chunk_size, file->address)
  
    configuration->dataBlock->data=(file->bytes_left >= file->chunk_size) ? malloc(file->chunk_size) :malloc(file->bytes_left);
    configuration->address = file->address;
    configuration->dataBlock->size = (file->bytes_left >= file->chunk_size) ? file->chunk_size :
                                                                             file->bytes_left;
    
    memcpy(configuration->dataBlock->data, file->data, configuration->dataBlock->size);
    file->bytes_left -= configuration->dataBlock->size;
    file->data = (file->data + configuration->dataBlock->size);
    file->address += configuration->dataBlock->size;
    return configuration;
}
bool isEOF(file_t* file) {
    PRINT("HELLOO? ")
    uint32_t bytes_left= file->bytes_left;
    if (bytes_left == 0) {
        PRINT("EOF")
        return true;
    }
    PRINT(" No EOF")
    return false;
}

void freeFile(file_t* file){
    free(file);
    file=NULL;
    
}

void freeConfBlock(configurationBlock_t* configurationBlock) {
    free(configurationBlock->dataBlock->data);
    free(configurationBlock->dataBlock);
    free(configurationBlock);
    configurationBlock=NULL;
}
// malloc
