#include "internal.h"

StringMemory* StrMem;
Program Prog;

void init_program(uint64_t memory)
{
    MglibcAllocCtx = &Prog.mem_ctx;
    Prog.mem_ctx = ctx_init_default(&Prog.mem);

    // timer_start(&Prog.timer);
    arena_init(&Prog.mem, memory);

    str_pool_prepare(Prog.mem_ctx, &Prog.strings, POOL_count);
    define_pool(POOL_DEFAULT, 1024, .is_default = true);
    define_pool(POOL_DISPLAY, 1024);
    define_pool(POOL_PRINT, 256, .is_print = true);
    str_pool_finalize(Prog.mem_ctx);

    Prog.dyn_mem.capacity = Prog.mem.capacity - Prog.mem.cursor;
    Prog.dyn_mem.memory = arena_use(&Prog.mem, Prog.dyn_mem.capacity);

    // we just need the general prog memory for the start
    // -> Then we just use the dynamic portion of it
    Prog.mem_ctx = ctx_init_default(&Prog.dyn_mem);
}
