#ifndef SYSHUB_TYPES
#define SYSHUB_TYPES

#include <string/types.h>

typedef struct
{
    str todo_db;
    str todo_strings;
} ConfigPaths;

typedef struct
{
    str db_folder;
    int max_col;
    int todo_fin_color;

    ConfigPaths paths;
} Config;

#endif
