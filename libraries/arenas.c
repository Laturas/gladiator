#ifndef INCLUDES
	#include "ALL_INCLUDES"
#endif

#define ARENA_HEADER_SIZE sizeof(u64) * 2
#define PGSIZE 4096

static void mem_commit(void* Pointer, u64 allocation_size) {
	VirtualAlloc(Pointer, allocation_size, MEM_COMMIT, PAGE_READWRITE);
}

arena aalloc(u64 arena_size) {
	u64* full_size = (u64 *)VirtualAlloc(0, arena_size + sizeof(u64) + sizeof(u64), MEM_RESERVE, PAGE_NOACCESS);
	//if (full_size == NULL) {debug_print("Error: full_size variable is null!\n"); exit(1);}

	arena newarena = {
		full_size,
		full_size + sizeof(u64*),
		(u8*)(full_size + sizeof(u64*) + sizeof(u64*)),
	};
	mem_commit(newarena.buffer, PGSIZE);

	(*newarena.size) = ARENA_HEADER_SIZE;

	(*newarena.max_size) = arena_size;
	return newarena;
}

#define _awrite(_arena, byte_num, val) _arena.buffer[(*_arena.size) + byte_num] = val

#define next_4096_multiple(input) ((input) >> 12) + 1

/**
 * Pushes size amount of 0 bytes to the given arena and returns a pointer to it.
 */
void* apush_buf(arena pushed_arena, u64 amt) {
	u64 current_size = (*pushed_arena.size);
	if (current_size + amt > (*pushed_arena.max_size)) {
		debug_print("Arena overflow error!\n");
		printf("Attempted to push object of size %llu onto an arena with only %llu bytes remaining. (The arena's max size was %llu)", 
			amt,
			(*pushed_arena.max_size) - current_size,
			(*pushed_arena.max_size)
		);
		exit(1);
	}

	int next_mul = next_4096_multiple(current_size + amt);
	int current_mul = next_4096_multiple(current_size);

	if (next_mul > current_mul) {
		mem_commit(pushed_arena.buffer, PGSIZE * next_mul);
		printf("Expanded!\n");
	}

	for (u64 i = 0; i < amt; i++) {
	   _awrite(pushed_arena, i, 0);
	}
	(*pushed_arena.size) += amt;

	return pushed_arena.buffer + (*pushed_arena.size) - amt;
}


void* _apush(arena pushed_arena, const void* const obj_to_push, u64 size_of_pushed_obj) {
	u8* pushed_obj = (u8*)obj_to_push;
	u64 current_size = (*pushed_arena.size);
	if (current_size + size_of_pushed_obj > (*pushed_arena.max_size)) {
		debug_print("Arena overflow error!\n");
		printf("Attempted to push object of size %llu onto an arena with only %llu bytes remaining. (The arena's max size was %llu)", 
			size_of_pushed_obj,
			(*pushed_arena.max_size) - current_size,
			(*pushed_arena.max_size)
		);
		exit(1);
	}

	int next_mul = next_4096_multiple(current_size + size_of_pushed_obj);
	int current_mul = next_4096_multiple(current_size);

	if (next_mul > current_mul) {
		mem_commit(pushed_arena.buffer, PGSIZE * next_mul);
		printf("Expanded!\n");
	}

	for (u64 i = 0; i < size_of_pushed_obj; i++) {
	   _awrite(pushed_arena, i, pushed_obj[i]);
	}
	(*pushed_arena.size) += size_of_pushed_obj;

	return pushed_arena.buffer + (*pushed_arena.size) - size_of_pushed_obj;
}

void pop(arena arena_to_clear, u64 pop_amount_bytes) {
	if (pop_amount_bytes > (*arena_to_clear.size)) {printf("Cannot pop off arena, not enough bytes to pop. Exiting..."); exit(1);}
	(*arena_to_clear.size) = (*arena_to_clear.size) - pop_amount_bytes;
}

void aclear(arena arena_to_clear) {
	VirtualFree(arena_to_clear.buffer, *arena_to_clear.size, MEM_DECOMMIT);
	mem_commit(arena_to_clear.buffer, PGSIZE);
	(*arena_to_clear.size) = ARENA_HEADER_SIZE;
}

void afree(arena arena_to_free) {
	VirtualFree(arena_to_free.buffer, 0, MEM_RELEASE);
}