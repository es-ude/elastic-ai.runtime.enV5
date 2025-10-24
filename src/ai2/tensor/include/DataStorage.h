//
// Created by Leo Buron on 20.10.25.
//

#ifndef ENV5_RUNTIME_DATASTORAGE_H
#define ENV5_RUNTIME_DATASTORAGE_H
#include <stddef.h>
#include <stdint.h>

typedef struct Entry {
    uint8_t* dataPTR; // PTR for starting address in uint8_t array
    size_t numberOfElements;
} dataEntry_t;

typedef struct DataStorage {
    uint8_t *data;
    size_t size;
    dataEntry_t *entries;
    size_t numberOfEntries;
}dataStorage_t;


uint8_t* getDataFromStorage(dataStorage_t storage, void* dataPTR);
dataEntry_t* addDataToStorage(dataStorage_t storage, void* dataPTR, size_t numberOfElements);


#endif // ENV5_RUNTIME_DATASTORAGE_H
