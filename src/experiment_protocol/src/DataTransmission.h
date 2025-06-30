#ifndef ENV5_RUNTIME_DATATRANSMISSION_H
#define ENV5_RUNTIME_DATATRANSMISSION_H

typedef struct DataTransmission DataTransmission;

enum {
    DATA_TRANSMISSION_STARTED = 0x01,
    DATA_TRANSMISSION_SEND_ERROR = 0x08,
    DATA_TRANSMISSION_RECEIVE_ERROR = 0x10
};

struct DataTransmission {
    void (*start)(DataTransmission *);
    void (*send)(DataTransmission *, const void *, int);
    void (*receive)(DataTransmission *, void *, int);
    void (*end)(DataTransmission *);
    int (*getStatus)(DataTransmission *);
    void (*destroy)(DataTransmission **);
};

static inline void
DataTransmission_start(DataTransmission *self) {
    self->start(self);
}

static inline const void
DataTransmission_send(DataTransmission *self, const void *buffer, int length) {
    return self->send(self, buffer, length);
}

static inline void
DataTransmission_receive(DataTransmission* self, void * buffer, int length) {
    return self->receive(self, buffer, length);
}

static inline void
DataTransmission_end(DataTransmission *self) {
    self->end(self);
}

static inline int
DataTransmission_getStatus(DataTransmission *self) {
    return self->getStatus(self);
}

static inline void
DataTransmission_destroy(DataTransmission **self) {
    (*self)->destroy(self);
}

#endif //ENV5_RUNTIME_DATATRANSMISSION_H
