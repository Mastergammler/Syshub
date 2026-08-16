#include "internal.h"

int main(int argc, char** argv)
{
    init_program(4096);

    StrPoolOptions opt = {.pool_idx = POOL_DEFAULT};
    Config c = load_config(&Prog.memory, opt);

    str_printc("TodoFile: '%'", fmt_s(c.todo_file));

    // debug_print_pool(*POOL(opt.pool_idx), 32);

    return 0;
}
