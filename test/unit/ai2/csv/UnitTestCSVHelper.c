#include "CSVHelper.h"
#include "unity.h"
#include <string.h>
#include <stdlib.h>

#define CSV_INPUT_PATH "../../../../../../test/unit/ai2/csv/input.csv"
#define CSV_OUTPUT_PATH "../../../../../../test/unit/ai2/csv/output.csv"


void testCSVReadRowsByBufferSize() {
    char *filePath = CSV_INPUT_PATH;

    size_t numberOfRows = 1;

    csvData_t csvData;

    char row[MAX_ROW_SIZE];
    char *rows[1];
    rows[0] = row;
    size_t entriesInRow[1];

    setCSVData(&csvData, rows, numberOfRows, entriesInRow);

    csvReadRowsByBufferSize(filePath, &csvData);

    TEST_ASSERT_EQUAL_STRING("1, 2, 3, 4, 5, 6", csvData.rows[0]);
    TEST_ASSERT_EQUAL_size_t(6, csvData.entriesInRow[0]);
}

void testCSVParseBufferAsFloat() {
    char *filePath = CSV_INPUT_PATH;

    size_t numberOfRows = 1;

    csvData_t csvData;

    char row[MAX_ROW_SIZE];
    char *rows[numberOfRows];
    rows[0] = row;
    size_t entriesInRow[1];

    setCSVData(&csvData, rows, numberOfRows, entriesInRow);

    csvReadRowsByBufferSize(filePath, &csvData);

    float *rowsAsFloat[numberOfRows];
    float rowFloat[6];
    rowsAsFloat[0] = rowFloat;

    csvParseBufferAsFloat(&csvData, rowsAsFloat);
    float expected[] = {1, 2, 3, 4, 5, 6};

    TEST_ASSERT_EQUAL_FLOAT_ARRAY(expected, rowsAsFloat[0], 6);
}

void testCSVWriteRowsByBufferSize() {
    char *filePath = CSV_OUTPUT_PATH;

    size_t numberOfRows = 1;

    csvData_t csvData;

    char row[] = {"1, 2, 3, 4, 5, 6"};
    char *rows[1];
    rows[0] = row;
    size_t entriesInRow[] = {6};

    setCSVData(&csvData, rows, numberOfRows, entriesInRow);

    csvWriteRowsByBufferSize(filePath, &csvData);

    char actual[MAX_ROW_SIZE];

    FILE *fp;
    fp = fopen(filePath, "r");
    fgets(actual, MAX_ROW_SIZE, fp);
    fclose(fp);

    TEST_ASSERT_EQUAL_STRING("1, 2, 3, 4, 5, 6", actual);
}


void setUp() {}
void tearDown() {}

int main() {
    UNITY_BEGIN();
    RUN_TEST(testCSVReadRowsByBufferSize);
    RUN_TEST(testCSVParseBufferAsFloat);
    RUN_TEST(testCSVWriteRowsByBufferSize);
    return UNITY_END();
}
