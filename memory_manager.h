#ifndef MEMORY_MANAGER_H
#define MEMORY_MANAGER_H

#include <stdint.h>
#include <stddef.h>
#include <stdbool.h>

uintptr_t calculate_distance(void* ptr1, void* ptr2);
void* read_pointer();
void test_calculate_distance();
void mem_init(size_t size);
void* mem_alloc(size_t size);
void mem_free(void* block);
void* mem_resize(void* block, size_t size);
void mem_deinit();
void print_blocks_USR();
void print_blocks_ADMIN();

#endif // MEMORY_MANAGER_H