#include <stdio.h>
#include "StdIODataTransmission.h"
#include <stdint.h>
#include <malloc.h>
#include "DataTransmission.h"

typedef struct StdIODataTransmission {
    DataTransmission self;
    uint8_t status;
} StdIODataTransmission;

static StdIODataTransmission *
cast(DataTransmission *self) {
    return (StdIODataTransmission *)self;
}

static void start(DataTransmission *self) {
    StdIODataTransmission *this = cast(self);
    this->status |= DATA_TRANSMISSION_STARTED;
}

static void end(DataTransmission *self) {
    StdIODataTransmission *this = cast(self);
    this->status &= ~DATA_TRANSMISSION_STARTED;
}

static void
receive(DataTransmission *self, void *buffer, int length) {
    fread(buffer, 1, length, stdin);
}

static void
send(DataTransmission *self, const void * restrict buffer, int length) {
    fwrite(buffer, 1, length, stdout);
}

static int
getStatus(DataTransmission *self) {
    return cast(self)->status;
}

static void
destroy(DataTransmission **self) {
    free(*self);
    *self = NULL;
}

DataTransmission *
IDataTransmission_createStdIODataTransmission(void) {
    static DataTransmission fns = {
            .end = end,
            .receive = receive,
            .send = send,
            .start = start,
            .getStatus = getStatus,
            .destroy = destroy
    };
    DataTransmission *self = malloc(sizeof(StdIODataTransmission));
    *cast(self) = (StdIODataTransmission){.status=0x00,
                                          .self=fns};
    return self;
}