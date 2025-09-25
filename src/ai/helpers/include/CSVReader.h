#ifndef CSVREADER_H
#define CSVREADER_H
#include <stddef.h>

#define CSV_INPUT_PATH "../../../../../../src/ai/csv_files/input.csv"
#define CSV_LABEL_PATH "../../../../../../src/ai/csv_files/label.csv"
#define CSV_SETTINGS_PATH "../../../../../../src/ai/csv_files/settings.csv"

#define MAX_ROW_SIZE 1000

typedef struct csvData {
    char **rows;
    size_t numberOfRows;
    size_t bufferSize;
}csvData_t;

char *csvReadRow(char *filePath);

char** csvReadEntireFile(char path[]);

csvData_t *csvReadBuffered(char *filePath, size_t requiredRowsPerRead);

float *csvParseRowAsFloat(char *row, size_t entriesPerRow);

#endif //CSVREADER_H
