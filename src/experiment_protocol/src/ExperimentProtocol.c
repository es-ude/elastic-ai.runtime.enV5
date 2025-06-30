#include "ExperimentProtocol.h"
#include "DataTransmission.h"
#include <stdint.h>

struct ExperimentEndpoint {
    DataTransmission *transmission;
    int num_handlers;
    const ExperimentDataHandler *handlers;
    int max_package_size;
};

const ExperimentEndpoint *ExperimentEndpoint_create(DataTransmission *transmission,
                                                    const ExperimentDataHandler *handlers,
                                                    int num_handlers) {
    ExperimentEndpoint *self = malloc(sizeof(ExperimentEndpoint));
    *self = (ExperimentEndpoint){
        .transmission = transmission, .handlers = handlers, .num_handlers = num_handlers};
    return self;
}

void ExperimentEndpoint_process(const ExperimentEndpoint *self) {
    uint8_t buffer[self->max_package_size];
    DataTransmission_receive(self->transmission, buffer, self->max_package_size);
}
