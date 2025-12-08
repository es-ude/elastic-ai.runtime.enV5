#include "CSVHelper.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

void setCSVData(csvData_t *csvData, char **rows, size_t numberOfRows, size_t *entriesInRow) {
    csvData->rows = rows;
    csvData->entriesInRow = entriesInRow;
    csvData->numberOfRows = numberOfRows;
}

static size_t countCsvEntries(const char *row) {
    size_t count = 1;
    for (size_t i = 0; row[i] != '\0'; i++) {
        if (row[i] == ',') {
            count++;
        }
    }
    return count;
}


void csvReadRowsByBufferSize(char *filePath, csvData_t *csvData) {
    FILE *fp;
    fp = fopen(filePath, "r");
    if (fp == NULL) {
        printf("Error: Could not open file %s\n", filePath);
        return;
    }

    for (size_t i = 0; i < csvData->numberOfRows; i++) {
        fgets(csvData->rows[i], MAX_ROW_SIZE, fp);
        csvData->entriesInRow[i] = countCsvEntries(csvData->rows[i]);
    }

    fclose(fp);
}

void csvParseBufferAsFloat(csvData_t *csvData, float **output) {
    for (size_t i = 0; i < csvData->numberOfRows; i++) {
        char *token = strtok(csvData->rows[i], ",");
        size_t index = 0;

        while (token != NULL && index < csvData->entriesInRow[i]) {
            output[i][index] = strtof(token, NULL);
            index++;
            token = strtok(NULL, ",");
        }
    }
}

void csvWriteRowsByBufferSize(char *filePath, csvData_t *csvData) {
    FILE *fp = fopen(filePath, "w");
    if (fp == NULL) {
        printf("Error: Could not open file %s\n", filePath);
        return;
    }

    for (size_t i = 0; i < csvData->numberOfRows; i++) {
        fputs(csvData->rows[i], fp);
    }

    fclose(fp);
}
