#include <string.h>
#include "arenas.h"

#define EXTRA_SAFE
typedef struct String {
    unsigned int length;
    char* raw_string;
} _String;

#define string _String*

/**
 * Utility function
 * Creates an empty string and stores it onto the given arena.
 */
string newstr(arena store_location) {
    _String newstr = {0, NULL};
    string struc = apush(store_location, newstr);
    return struc;
}

/**
 * Returns 1 if the two input strings are the same size and are equivalent in characters at every location.
 */
int strs_are_equal(string str_1, string str_2) {
    if (str_1->length != str_2->length) {return 0;}
    for (int i = 0; i < str_1->length; i++) {if (str_1->raw_string[i] != str_2->raw_string[i]) {return 0;}}
    return 1;
}

/**
 * Returns 1 if the two input strings are the same size and are equivalent in characters at every location.
 * Assumes that the two strings are the same length already.
 */
int str_chrstr_equal(string str_1, char* str_2) {
    for (int i = 0; i < str_1->length; i++) {if (str_1->raw_string[i] != str_2[i]) {return 0;}}
    return 1;
}

void print(string input_string) {
    #ifdef EXTRA_SAFE
    if (!input_string) {printf("[NULL]"); return;}
    #endif
    printf("%.*s", input_string->length, input_string->raw_string); fflush(stdout);
}
void println(string input_string) {
    #ifdef EXTRA_SAFE
    if (!input_string) {printf("[NULL]\n"); return;}
    #endif
    printf("%.*s\n", input_string->length, input_string->raw_string); fflush(stdout);
}