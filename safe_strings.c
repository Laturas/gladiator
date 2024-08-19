#define EXTRA_SAFE
typedef struct {
    unsigned int length;
    char* raw_string;
} _String;

#define string _String*
#define clear(str) str = free_string(str)

string free_string(string str) {
    free(str->raw_string);
    free(str);
    return NULL;
}
string new(char* str) {
    int ln = strlen(str);
    char* buffer = calloc(ln, sizeof(char));
    string new_str = malloc(sizeof(string));
    new_str->length = ln;
    new_str->raw_string = buffer;
    return new_str;
}

void print(string input_string) {
    #ifdef EXTRA_SAFE
    if (!input_string) {printf("[NULL STRING]"); return;}
    #endif
    printf("%.*s", input_string->length, input_string->raw_string);
}
void println(string input_string) {
    #ifdef EXTRA_SAFE
    if (!input_string) {printf("[NULL STRING]\n"); return;}
    #endif
    printf("%.*s\n", input_string->length, input_string->raw_string);
}