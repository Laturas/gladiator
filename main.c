#include <stdio.h>
#include <stdlib.h>
#include <memoryapi.h>
#include "safe_strings.c"

enum Token {
    CUSTOM,
    OPEN_BRACE,
    CLOSE_BRACE,
    OPEN_PAREN,
    CLOSE_PAREN,
    SEMICOLON,
    i32,
    u8,
    RETURN,
    INT_LITERAL,
} typedef Token;

void print_token(Token input_token) {
    switch (input_token) {
        case CUSTOM:
            printf("_"); break;
        case OPEN_BRACE:
            printf("{"); break;
        case CLOSE_BRACE:
            printf("}"); break;
        case OPEN_PAREN:
            printf("("); break;
        case CLOSE_PAREN:
            printf(")"); break;
        case SEMICOLON:
            printf(";"); break;
        case i32:
            printf("int"); break;
        case u8:
            printf("u8"); break;
        case RETURN:
            printf("return"); break;
        case INT_LITERAL:
            printf("[value]"); break;
    }
}

/**
* Dumps the contents of a file into a char*
*/
string map_file_to_memory(char* file) {
    FILE* src_file = fopen(file, "rb");
    if (!src_file) {printf("Source file not found"); return NULL;}

    fseek(src_file,0,SEEK_END);
    int length = ftell(src_file);
    printf("length: %d\n",length);
    fseek(src_file,0,SEEK_SET);
    char* buffer = calloc(length, sizeof(char));
    string ret_str = malloc(sizeof(string));

    fread(buffer,1,length,src_file);
    fclose(src_file);
    ret_str->raw_string = buffer;
    ret_str->length = length;
    return ret_str;
}

Token match_6(string str) {

}

Token match_5(string str) {

}

Token match_4(string str) {

}

Token match_3(string str) {

}

Token match_1() {
    
}

Token match(string str) {
    switch (str->length) {
        case 6:
            return match_6(str);
        case 5:
            return match_5(str);
        case 4:
            return match_4(str);
        case 3:
            if (str->raw_string[0] == 'i' && str->raw_string[1] == 'n' && str->raw_string[2] == 't') {return i32;}
            return i32;
            //return match_3(str);
        case 2:
            if (str->raw_string[0] == 'u' && str->raw_string[1] == '8') {return u8;}
            return CUSTOM;
        case 1:
            if (str->raw_string[0] == 'u' && str->raw_string[1] == '8') {return u8;}
        default:
            return CUSTOM;
    }
}

int main(int argc, char* argv[]) {
    string str = map_file_to_memory(".\\testcase\\main.glad");

    println(str);

    clear(str);
}