#ifndef INCLUDES
#include "ALL_INCLUDES"
#endif

typedef enum ERROR_TYPE {
    DEFAULT_ERROR,
    PARSING_ERROR,
    ASSEMBLING_ERROR,
} ERROR_TYPE;

void print_line_column(u32 start, const string const file_buff) {
    u32 line = 1; u32 column = 1;
    for (u32 i = 0; i < start; i++) {
        if (file_buff->raw_string[i] == '\n') { column = 1; line++; }
        else {column++;}
    }
    printf("line %d column %d", line, column);
}

void error_on(u32 start, const string const file_text, ERROR_TYPE error) {
    switch (error) {
        case DEFAULT_ERROR: printf("Error on: "); break;
        case PARSING_ERROR: printf("Parsing error on: "); break;
        case ASSEMBLING_ERROR: printf("Assembling error on: "); break;
    }
    print_line_column(start, file_text); printf("\n");
}