#include "internal.h"

str build_arg_string(int argc, char** argv, bool includeNl)
{
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

    if (includeNl)
    {
        str end = str_alloc_opt(builder, "\n");
        concat.len += end.len;
    }
    else
    {
        str end = str_alloc_opt(builder, "");
        concat.len += end.len;
    }

    return concat;
}
