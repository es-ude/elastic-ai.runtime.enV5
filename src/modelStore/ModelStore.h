#ifndef MODEL_STORE_H
#define MODEL_STORE_H

#include "Posting.h"
#include <stdbool.h>

typedef enum ModelStoreStatus {
    MS_OTHER = 1,
    MS_ILLEGAL_INPUT = 2,
    MS_MODEL_DATA_NOT_FOUND = 3,
    MS_MODEL_URI_NOT_FOUND = 4,
    // other error codes below 256 are reserved for errors coming from the model store
    MS_SUCCESS = 256,
    MS_TIMEOUT,
    MS_DOWNLOAD_ERROR
  } ModelStoreStatus;

void responseCallback(posting_t posting);

void ModelStore_connect(char *topic);
void ModelStore_disconnect();

ModelStoreStatus ModelStore_getModel(const char *modelName, char **model);
ModelStoreStatus ModelStore_searchModel(const char *problem_graph, char **modelUri);

#endif