#include "CSVReader.h"
#include "unity.h"
#include <string.h>
#include <stdlib.h>

void testCSVReadRow() {
    char filePath[] = CSV_INPUT_PATH;
    char *actual = csvReadRow(filePath);

    TEST_ASSERT_EQUAL_STRING("0, 1, 2, 3, 4, 5, 0, -1, -2, -3, -4, -5", actual);
}

void testCSVReadEntireFile() {

    char filePath[] = CSV_INPUT_PATH;
    char **csvData = csvReadEntireFile(filePath);

    char row[MAX_ROW_SIZE];
    FILE *fp;
    fp = fopen(filePath, "r");

    size_t index = 0;
    while (fgets(row, MAX_ROW_SIZE, fp) != NULL) {
        TEST_ASSERT_EQUAL_STRING(row, csvData[index]);
        index += 1;
    }
}

void testReadBufferedCSV() {
}


void setUp() {}
void tearDown() {}

int main() {
    UNITY_BEGIN();
    RUN_TEST(testCSVReadRow);
    RUN_TEST(testCSVReadEntireFile);
    RUN_TEST(testReadBufferedCSV);
    return UNITY_END();
}
