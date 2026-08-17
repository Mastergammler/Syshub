#ifndef SYSHUB_TYPES
#define SYSHUB_TYPES

#include <string/types.h>

typedef struct
{
    str todo_file;
    str todo_db_file;
    str todo_strings;
    int max_col;
    int todo_fin_color;
} Config;

#endif
