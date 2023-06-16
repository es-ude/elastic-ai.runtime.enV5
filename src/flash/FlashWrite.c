#include "Flash.h"
#include "FlashInternal.h"
#include "Common.h"

flashWriteError_t flashWrite(uint32_t address, dataBlock_t* dataBlock) {
//    PRINT("flash write, address %u and datablock size %u", configuration->address,
//          configuration->dataBlock->size)
    file_t* file = initFile( address, dataBlock, FLASH_PAGE_SIZE);
    PRINT("file content: bytes left %u, address %u chunk size %u", file->bytes_left,
          file->address, file->chunk_size);

 
    while (!(file->isEOF(file))) {
        PRINT("First chunk")
        configurationBlock_t* chunk = file->next(file);
        flashWriteError_t writeError = flashWritePage(chunk->address,
                                                      chunk->dataBlock->data,
                                                      chunk->dataBlock->size);
        if (writeError == FLASH_WRITE_ERROR) {
            PRINT("Flash write error")
            freeConfBlock(chunk);
            return writeError;
        }
        freeConfBlock(chunk);
    }
    file->freeFile(file);
    return FLASH_WRITE_SUCCEED;
}