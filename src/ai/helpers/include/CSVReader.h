#ifndef CSVREADER_H
#define CSVREADER_H
#include <stddef.h>

#define MAX_ROW_SIZE 20

typedef struct csvData {
    char **rows;
    size_t numberOfRows;
    size_t bufferSize;
}csvData_t;

char *csvReadRow(char *filePath);

char** csvReadEntireFile(char path[]);

csvData_t *csvReadBuffered(char *filePath, size_t requiredRowsPerRead);

float *parseRowAsFloat();

#endif //CSVREADER_H
