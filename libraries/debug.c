#ifndef DEBUG_UTILS
    #define debug_print(values) printf("Error encountered on line number: %d\nIn function: %s\n  ", __LINE__, __FUNCTION__); printf(values); fflush(stdout)
    #define DEBUG_UTILS
#endif