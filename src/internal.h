#ifndef SYSHUB_INTERNAL
#define SYSHUB_INTERNAL

#include "types.h"
#include <alloc/module.h>
#include <string/macros.h>
#include <string/module.h>

typedef enum
{
    POOL_DEFAULT,
    POOL_PRINT,
    POOL_FILE,
    POOL_count
} StrPools;

typedef struct
{
    // Timer timer;
    Arena memory;
    AllocContext mem;
    StringMemory strings;

} Program;
static Program Prog;

Config load_config(Arena* alloc, StrPoolOptions opt);
void init_program(uint64_t memory);

#endif
