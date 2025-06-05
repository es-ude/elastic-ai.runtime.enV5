#ifndef ENV5_RUNTIME_EXPERIMENT_PROTOCOL_H
#define ENV5_RUNTIME_EXPERIMENT_PROTOCOL_H

enum {
    EXP_PROTOCOL_ACK = 0x01,
    EXP_PROTOCOL_NACK = 0x00,
    EXP_PROTOCOL_OPEN_STREAM = 0x02,
    EXP_PROTOCOL_CLOSE_STREAM = 0x03,
    EXP_PROTOCOL_DATAGRAM = 0x04,
};



typedef struct ExperimentEndpoint ExperimentEndpoint;

typedef struct ExperimentData ExperimentData;
typedef struct DataTransmission DataTransmission;

struct ExperimentData {
    int length;
    void *data;
};

typedef void (*ExperimentDataHandlerFn)(ExperimentData);

typedef struct ExperimentDataHandler ExperimentDataHandler;

struct ExperimentDataHandler {
    const char *name;
    ExperimentDataHandlerFn run;
};



const ExperimentEndpoint *
ExperimentEndpoint_create(DataTransmission *transmission,
                          const ExperimentDataHandler *handlers,
                          int num_handlers,
                          int max_package_size);

void
ExperimentEndpoint_destroy(const ExperimentEndpoint *self);

void
ExperimentEndpoint_process(const ExperimentEndpoint *self);


#endif //ENV5_RUNTIME_EXPERIMENT_PROTOCOL_H
