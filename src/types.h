#ifndef SYSHUB_TYPES
#define SYSHUB_TYPES

#include <stdio.h>
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
    int hide_age_days;

    ConfigPaths paths;
} Config;

typedef struct
{
    bool open;
    FILE* stream;
    int len;
    str path;
} FsRes;

typedef struct
{
    int offset;
    int len;
} Section;

#endif
