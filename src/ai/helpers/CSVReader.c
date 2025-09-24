#include "CSVReader.h"
#include<stdio.h>
#include <stdlib.h>
#include<string.h>

char *csvReadRow(char *filePath) {
    FILE *file = fopen(filePath, "r");
    if (!file) return NULL;
    char *row = calloc(MAX_ROW_SIZE, sizeof(char));
    fgets(row, MAX_ROW_SIZE, file);
    fclose(file);
    return row;
}

char **csvReadEntireFile(char *filePath) {
    FILE *fp;
    char row[MAX_ROW_SIZE];
    size_t rowCounter = 0;

    fp = fopen(filePath, "r");

    if (fp == NULL) {
        printf("Error: Could not open file %s\n", filePath);
        return NULL;
    }

    while (fgets(row, MAX_ROW_SIZE, fp) != NULL) {
        rowCounter += 1;
    }

    fclose(fp);

    char **csvData = malloc(rowCounter * sizeof(char *));
    fp = fopen(filePath, "r");

    for (size_t i = 0; i < rowCounter; i++) {
        fgets(row, MAX_ROW_SIZE, fp);
        csvData[i] = malloc(strlen(row) + 1);
        strcpy(csvData[i], row);
    }

    fclose(fp);

    return csvData;
}

csvData_t *csvReadBuffered(char *filePath, size_t requiredRows) {
    return NULL;
}

float *parseRowAsFloat(char *row, size_t entriesPerRow) {
    float *output = calloc(entriesPerRow, sizeof(float));
    size_t index = 0;

    char *token = strtok(row, ",");
    while (token != NULL && index < entriesPerRow) {
        output[index] = strtof(token, NULL);
        index++;
        token = strtok(NULL, ",");
    }

    return output;
}

