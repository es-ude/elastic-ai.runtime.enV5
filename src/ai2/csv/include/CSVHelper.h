#ifndef CSVHELPER_H
#define CSVHELPER_H

#define MAX_ROW_SIZE 1000
#include <stddef.h>

typedef struct csvData {
    char **rows;
    size_t numberOfRows;
    size_t *entriesInRow;
}csvData_t;

void setCSVData(csvData_t *csvData, char **rows, size_t numberOfRows, size_t *entriesInRow);

void csvReadRowsByBufferSize(char *filePath, csvData_t *csvData);

void csvParseBufferAsFloat(csvData_t *csvData, float **output);

void csvWriteRowsByBufferSize(char *filePath, csvData_t *csvData);

#endif //CSVHELPER_H
