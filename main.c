#include "ALL_INCLUDES"

/**
* Dumps the contents of a file into a char*
*/
string map_file_to_memory(arena aren, char* file) {
    FILE* src_file = fopen(file, "r");
    if (!src_file) { debug_print("PARSER ERROR: Source file not found: "); printf(file); return NULL; }

    // Gets the length of the source file
    fseek(src_file, 0, SEEK_END);
    u32 length = ftell(src_file);
    fseek(src_file, 0, SEEK_SET);
    
    string filestr = newstr(aren);
    char* buffer = apush_buf(aren, length);

    fread(buffer, sizeof(char), length, src_file);
    fclose(src_file);

    filestr->raw_string = buffer;
    filestr->length = length - 1;
    return filestr;
}

static void print_node_list(struct AbstractSyntaxStream ass, const string const str) {
    PolNode* first_cp = ass.first;
    while (first_cp <= ass.last) {
        print_node(*first_cp, str); printf("\n");
        first_cp++;
    }
}

int main(int argc, char* argv[]) {
    if (argc < 2) { printf("Usage: ./<exe> <source file>\n"); return 1; }
    u64 gigabyte = 1073741824 / 6;
    arena rawfile_arena = aalloc(gigabyte);
    arena token_arena = aalloc(gigabyte);
    arena ass_arena = aalloc(gigabyte);

    string str = map_file_to_memory(rawfile_arena, argv[1]);

    if (str == NULL) return 1; //else printf("File read in successfully\n");

    AtomList* atoms = atomize(token_arena, str);
    //print_atom_list(atoms, LINES | FILE_PRESENT, str);
    struct AbstractSyntaxStream ass = generate_ass(ass_arena, atoms, str);
    //print_node_list(ass, str);
    //printf("----------------END COMPILER OUTPUT----------------\n");

    generate_asm(ass, str);

    afree(rawfile_arena);
    afree(token_arena);
}