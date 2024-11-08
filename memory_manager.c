#include "memory_manager.h"
#include <stdint.h>
#include <stddef.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

uintptr_t calculate_distance(void* ptr1, void* ptr2) {
    uintptr_t address1 = (uintptr_t)ptr1;
    uintptr_t address2 = (uintptr_t)ptr2;
    return (address1 > address2) ? (address1 - address2) : (address2 - address1);
}

void* read_pointer() {
    void* ptr;
    printf("Enter pointer address (in hexadecimal, e.g., 0x7ffee4b8c8c0): ");
    scanf("%p", &ptr);
    return ptr;
}

void test_calculate_distance() {
    void* ptr1 = read_pointer();
    void* ptr2 = read_pointer();

    uintptr_t distance = calculate_distance(ptr1, ptr2);
    printf("Distance between %p and %p is %lu bytes\n", ptr1, ptr2, distance);
}


typedef struct Block {
    size_t size;
    bool free;
    void* memory;
} Block;

Block* block_array;
size_t metadata_count;
size_t memory_pool_size;
void* memory_pool;


void  mem_init(size_t size) {

    memory_pool = malloc(size);

    if (memory_pool == NULL) {
        printf("Memory allocation failed\n");
        exit(1);
    }
    
    
    memory_pool_size = size;

    // Initialize the metadata array with a single large block
    block_array = malloc(sizeof(Block));
    if (!block_array) {
        printf("Failed to allocate metadata array\n");
        exit(1);
    }
    metadata_count = 1;

    block_array[0].size = size;
    block_array[0].free = true;
    block_array[0].memory = memory_pool;

}
// TODO FIX mem_alloc, mem_free, mem_resize and everything else should work now
void* mem_alloc(size_t size) {
    for (size_t i = 0; i < metadata_count; i++) {
        if (block_array[i].free && block_array[i].size >= size) {
            // Split the block if it's larger than needed
            if (block_array[i].size > size) {
                block_array = realloc(block_array, (metadata_count + 1) * sizeof(Block));
                if (!block_array) {
                    printf("Failed to reallocate metadata array\n");
                    exit(1);
                }

                block_array[metadata_count].size = block_array[i].size - size;
                block_array[metadata_count].free = true;
                block_array[metadata_count].memory = (void*)((uintptr_t)block_array[i].memory + size);

                block_array[i].size = size;
                metadata_count++;
            }
            block_array[i].free = false;
            return block_array[i].memory;
        }
    }
    return NULL; // No suitable block found
}

void mem_free(void* block) 
{
    for (size_t i = 0; i < metadata_count; i++) {
        if (block_array[i].memory == block) {
            block_array[i].free = true;

            // Coalesce adjacent free blocks
            for (size_t j = 0; j < metadata_count - 1; j++) 
            {
                if (block_array[j].free && block_array[j + 1].free &&
                    (uintptr_t)block_array[j].memory + block_array[j].size == (uintptr_t)block_array[j + 1].memory) {
                    block_array[j].size += block_array[j + 1].size;
                    for (size_t k = j + 1; k < metadata_count - 1; k++) {
                        block_array[k] = block_array[k + 1];
                    }
                    metadata_count--;
                    block_array = realloc(block_array, metadata_count * sizeof(Block));
                    if (!block_array) {
                        printf("Failed to reallocate metadata array\n");
                        exit(1);
                    }
                }
            }
            return;
        }
        else {
            //printf("Block not found\n");
        }
    }
}
  
void* mem_resize(void* block, size_t size) {
    for (size_t i = 0; i < metadata_count; i++) {
        if (block_array[i].memory == block) {
            if (block_array[i].size == size) {
                return block;
            } else if (block_array[i].size > size) {
                // Shrink the block
                block_array = realloc(block_array, (metadata_count + 1) * sizeof(Block));
                if (!block_array) {
                    printf("Failed to reallocate metadata array\n");
                    exit(1);
                }

                block_array[metadata_count].size = block_array[i].size - size;
                block_array[metadata_count].free = true;
                block_array[metadata_count].memory = (void*)((uintptr_t)block_array[i].memory + size);

                block_array[i].size = size;
                metadata_count++;
                return block_array[i].memory;
            } else {
                // Allocate a new block
                void* new_block = mem_alloc(size);
                if (new_block) {
                    // Copy the data to the new block
                    memcpy(new_block, block, block_array[i].size);
                    // Free the old block
                    mem_free(block);
                }
                return new_block;
            }
        }
    }
    return NULL; // Block not found
    
}

void mem_deinit() {
    free(memory_pool);
    memory_pool = NULL;
    memory_pool_size = 0;

    free(block_array);
    block_array = NULL;
    metadata_count = 0;
}

void print_blocks_ADMIN() {
    for (size_t i = 0; i < metadata_count; i++) {
        printf("Block at %p: size = %zu, free = %s, Momory at = %p\n", 
               (void*)&block_array[i], block_array[i].size, block_array[i].free ? "true" : "false", 
               block_array[i].memory);
    }
}

void print_blocks_USR() {
    for (size_t i = 0; i < metadata_count; i++) {
        printf("Block at %p: size = %zu, free = %s \n", 
               block_array[i].memory, block_array[i].size, block_array[i].free ? "true" : "false");
    }
}


int mainNN() {

    printf("Memory Manager\n");
    printf("How much memory do you want to allocate? ");
    size_t size;
    scanf("%lu", &size);
    mem_init(size);
    printf("Memory pool is allocated\n");
    printf("Memory pool address: %p\n", memory_pool);
    print_blocks_USR();

    while (true)
    {
        printf("1. Allocate memory\n");
        printf("2. Free memory\n");
        printf("3. Resize memory\n");
        printf("4. Print memory blocks\n");
        printf("5. Exit\n");
        int choice;
        scanf("%d", &choice);
        switch (choice)
        {
        case 1:
            printf("Enter the size of memory to allocate: ");
            size_t alloc_size;
            scanf("%lu", &alloc_size);
            void* block = mem_alloc(alloc_size);
            if (block == NULL) {
                printf("Memory allocation failed\n");
            } else {
                printf("Memory allocated at %p\n", block);
            }
            print_blocks_USR();
            break;
        case 2:
            printf("Enter the address of memory to free: ");
            void* free_block;
            scanf("%p", &free_block);
            printf("Freeing memory at %p\n", free_block);
            mem_free(free_block);
            print_blocks_USR();
            break;
        case 3:
            printf("Enter the address of memory to resize: ");
            void* resize_block;
            scanf("%p", &resize_block);
            printf("Enter the new size of memory: ");
            size_t new_size;
            scanf("%lu", &new_size);
            void* new_block = mem_resize(resize_block, new_size);
            if (new_block == NULL) {
                printf("Memory resize failed\n");
            } else {
                printf("Memory resized at %p\n", new_block);
            }
            print_blocks_USR();
            break;
        case 4:
            print_blocks_ADMIN();
            break;
        case 5:
            mem_deinit();
            return 0;
        case 6:
            test_calculate_distance();
            break;
        default:
            printf("Invalid choice\n");
            break;
        
        }
    }
    mem_deinit();
    return 0;
}