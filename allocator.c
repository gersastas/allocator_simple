#include <stddef.h>
#include <stdint.h>

#define SMALL_SIZE  15
#define LARGE_SIZE  180
#define POOL_SIZE   1024

#define ALIGN sizeof(void*)
#define ALIGN_UP(x) (((x) + ALIGN - 1) & ~(ALIGN - 1))

typedef struct MemoryBlock {
    struct MemoryBlock* next;
} MemoryBlock;

static uint8_t pool_15[POOL_SIZE * ALIGN_UP(SMALL_SIZE)];
static uint8_t pool_180[POOL_SIZE * ALIGN_UP(LARGE_SIZE)];

static MemoryBlock* free_15 = NULL;
static MemoryBlock* free_180 = NULL;

static int initialized = 0;

static void init_pool(void)
{
    size_t i;
    MemoryBlock* block;

    for (i = 0; i < POOL_SIZE; i++) {
        block = (MemoryBlock*)(pool_15 + i * ALIGN_UP(SMALL_SIZE));
        block->next = free_15;
        free_15 = block;
    }

    for (i = 0; i < POOL_SIZE; i++) {
        block = (MemoryBlock*)(pool_180 + i * ALIGN_UP(LARGE_SIZE));
        block->next = free_180;
        free_180 = block;
    }

    initialized = 1;
}

void* malloc(size_t size)
{
    MemoryBlock* block;

    if (!initialized)
        init_pool();

    if (size <= SMALL_SIZE) {
        if (!free_15)
            return NULL;
        block = free_15;
        free_15 = block->next;
        return (void*)block;
    }

    if (size <= LARGE_SIZE) {
        if (!free_180)
            return NULL;
        block = free_180;
        free_180 = block->next;
        return (void*)block;
    }

    return NULL;
}

void free(void* ptr)
{
    MemoryBlock* block;

    if (!ptr)
        return;

    if ((uint8_t*)ptr >= pool_15 &&
        (uint8_t*)ptr < pool_15 + sizeof(pool_15)) {

        block = (MemoryBlock*)ptr;
        block->next = free_15;
        free_15 = block;
        return;
    }

    if ((uint8_t*)ptr >= pool_180 &&
        (uint8_t*)ptr < pool_180 + sizeof(pool_180)) {

        block = (MemoryBlock*)ptr;
        block->next = free_180;
        free_180 = block;
        return;
    }

}
