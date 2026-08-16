#include "internal.h"

int main(int argc, char** argv)
{
    init_program(4096);

    if (argc < 2)
    {
        str_printc("Usage: <todo text>");
        return 0;
    }

    StrPoolOptions opt = {.pool_idx = POOL_DEFAULT};
    Config c = load_config(&Prog.memory, opt);

    FILE* tododb = fopen(c.todo_file.chars, "a");
    if (!tododb)
    {
        // TODO: some ansi coloring would be nice
        str_printc("[ERR] Could not open file '%'", fmt_s(c.todo_file));
        return 1;
    }

    // NOTE: pretty interesting, this whole thing now is a string builder struct
    // and this is kind of the basic design of the mglibc string handling now
    // which is pretty cool, i can probably create a actual builder sturcture
    // now that handles appending etc correctly
    StrPoolOptions builder = {.pool_idx = POOL_DEFAULT, .substring_mode = true};
    str concat = str_alloc_opt(builder, argv[1]);
    for (int i = 2; i < argc; i++)
    {
        str space = str_alloc_opt(builder, " ");
        str text = str_alloc_opt(builder, argv[i]);

        concat.len += space.len + text.len;
    }
    // just for good measure, we want to finish the string and null temrminate
    // -> even though in this example we don't actually need it
    builder.substring_mode = false;
    str end = str_alloc_opt(builder, "\n");
    concat.len += end.len;

    fwrite(concat.chars, 1, concat.len, tododb);
    fclose(tododb);

    return 0;
}
