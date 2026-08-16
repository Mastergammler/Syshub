#include "internal.h"

StringMemory* StrMem;

void init_program(uint64_t memory)
{
    MglibcAllocCtx = &Prog.mem;
    Prog.mem = ctx_init_default(&Prog.memory);

    // timer_start(&Prog.timer);
    arena_init(&Prog.memory, memory);

    str_pool_prepare(Prog.mem, &Prog.strings, POOL_count);
    define_pool(POOL_DEFAULT, 1024, .is_default = true);
    define_pool(POOL_FILE, 1024);
    define_pool(POOL_PRINT, 256, .is_print = true);
    str_pool_finalize(Prog.mem);
}
