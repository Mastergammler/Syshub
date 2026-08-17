#ifndef SYSHUB_TODO
#define SYSHUB_TODO

#include "types.h"
#include <alloc/module.h>
#include <stdbool.h>
#include <stdio.h>
#include <string/types.h>

typedef struct
{
    int id;
    bool done;
    int str_offset;
    int str_len;

} TodoItem;

typedef struct
{
    int count;
    int max_id;
} TodoDbHeader;

typedef struct
{
    TodoDbHeader header;
    TodoItem* item_arr;
    str db_file;
    str strings_file;

} TodoDb;

void todo_add(TodoDb db, str text);
TodoDb todo_read_db(Arena* arrMem, str dbPath);
void todo_print(Config config);
bool todo_mark_done(str dbPath, int requestedId);

#endif
