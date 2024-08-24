#ifndef ARENAS
    #include <windows.h>
    #define u8 unsigned char
    #define u64 unsigned long long

    #define asize(_arena) (*_arena.size) 
    #define amax_size(_arena) (*_arena.max_size) 

    /** 
     * Try and only stack allocate these if you can.
     * Their structure is size, max_size, and buffer start pointer in that order.
     */
    typedef struct Arena {
        u64* const size; // pointer is constant
        u64* const max_size;
        u8* const buffer;
    } _Arena;

    /** 
     * Try and only stack allocate these if you can. This defines an arena structure.
     * Make sure to free at the end of the stack frame!
     * 
     * @param size the current size of the arena
     * @param max_size the maximum size of the arena
     * @param buffer the pointer to the values inside the arena.
     */
    #define arena _Arena

    /**
     * Reserves an address space of a given specified size.
     */
    arena aalloc(u64 arena_size);

    /**
     * Pushes an array of bytes onto an arena. To push a struct or int use apushlit
     * @param pushed_arena the arena being pushed to
     * @param pushed_obj the array of bytes being pushed onto the arena
     * @param size_of_pushed_obj hopefully self explanatory
     * 
     * @return A pointer to the added element
     */
    void* _apush(arena pushed_arena, const void* const obj_to_push, u64 size_of_pushed_obj);

    /**
     * Macro to push a non-array into an arena
     * @param _arena the arena being pushed to
     * @param item the item being pushed onto the arena
     * 
     * @return A pointer to the added element
     */
    #define apush(_arena, item) _apush(_arena, &item, sizeof(item));
    #define apush_size(_arena, item, size) _apush(_arena, &item, size);

    /**
     * Clears the arena to be used for other things. Decommits the memory but does not *free* it. Use afree for that
     */
    void aclear(arena arena_to_clear);

    /**
     * Destroys an arena and returns all memory used by it to the OS
     */
    void afree(arena arena_to_free);

    #define ARENAS
    #include "arenas.c"
#endif