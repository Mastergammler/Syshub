#ifndef SYSHUB_TODO
#define SYSHUB_TODO

#include "types.h"
#include <alloc/module.h>
#include <stdbool.h>
#include <stdio.h>
#include <string/types.h>
#include <time.h>

static const char TODO_DB_MAGIC[4] = {'T', 'D', 'D', 'B'};

/* Current write version of the db */
static const int TODO_DB_VERSION = 3;

typedef enum
{
    STREAM_CUR,
    STREAM_START,
    STREAM_END
} StreamPos;

typedef struct
{
    str db;
    str strings;
} Files;

typedef enum
{
    TODO_OK,
    TODO_NOT_FOUND,
    TODO_NO_ACTION
} TodoResult;

/**
 * File header, first bytes to read of the db file
 * Version & Indentification information
 */
typedef struct
{
    char magic[4];
    int version;
    int count;
    int max_id;
} TodoDbHeader;

typedef struct
{
    int id;
    bool done;
    bool deleted;
    int str_offset;
    int str_len;
    time_t creation_time;

} TodoItem;

typedef struct
{
    TodoDbHeader header;
    TodoItem* item_arr;
    // STFO: should this even be in the db?
    //  -> I'm not sure if that's sensible ...
    Files files;

} TodoDb;

TodoDb tddb_read_upgrade(Arena* arrMem, str dbPath);
TodoDbHeader tddb_read_header(FILE* dbFile);
TodoDbHeader tddb_increment_header(FILE* dbFile);
void tddb_upgrade_version(FILE* dbFile, TodoDb db);
void tddb_write_next_item(FILE* dbFile, TodoItem todo, StreamPos pos);

void todo_print(Config config);
TodoResult todo_add(Files files, str text);
TodoResult todo_mark_done(str dbPath, int requestedId);
TodoResult todo_remove(str dbPath, int requestedId);

#endif
