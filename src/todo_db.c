#include "internal.h"
#include "todo.h"

TodoDbHeader tddb_read_header(FILE* dbFile)
{
    TodoDbHeader header = {};

    fseek(dbFile, 0, SEEK_END);
    if (ftell(dbFile) == 0)
    {
        header.version = TODO_DB_VERSION;
        memcpy(&header.magic, &TODO_DB_MAGIC, 4);
        return header;
    }

    fseek(dbFile, 0, SEEK_SET);
    fread(&header.magic, sizeof(char), 4, dbFile);
    fread(&header.version, sizeof(int), 1, dbFile);
    fread(&header.count, sizeof(int), 1, dbFile);
    fread(&header.max_id, sizeof(int), 1, dbFile);

    if (strncmp(header.magic, TODO_DB_MAGIC, 4) != 0)
    {
        header = (TodoDbHeader){};
        str_printc("Error: Invalid db file. Magic does not match");
    }

    return header;
}

void todo_write_header(FILE* dbFile, TodoDbHeader header)
{
    // upgrade version
    // NOTE: now the version of the read file can be desynced
    // but this is actually desired, because if we update header
    // first and then read todos later, we need to know the actual
    // version, of which the todo's are written
    header.version = TODO_DB_VERSION;

    fseek(dbFile, 0, SEEK_SET);
    fwrite(&header.magic, sizeof(char), 4, dbFile);
    fwrite(&header.version, sizeof(int), 1, dbFile);
    fwrite(&header.count, sizeof(int), 1, dbFile);
    fwrite(&header.max_id, sizeof(int), 1, dbFile);
}

/*
 * Always writes the most current version
 *  -> Upgrade & write
 *
 * We need to preserve the paddings, because if we update smth in the table
 * we use the size of the item as offset, so we need to know where it is
 */
void tddb_write_next_item(FILE* dbFile, TodoItem todo, StreamPos pos)
{
    switch (pos)
    {
        case STREAM_CUR: /* do nothing */ break;
        case STREAM_START: fseek(dbFile, 0, SEEK_SET); break;
        case STREAM_END: fseek(dbFile, 0, SEEK_END); break;
        default: ASSERT(false, "StreamPos not implemented %i", pos);
    }

    bool padByte = 0;

    fwrite(FROM_VAR(todo.id), 1, dbFile);
    fwrite(FROM_VAR(todo.done), 1, dbFile);
    fwrite(FROM_VAR(todo.deleted), 1, dbFile);
    fwrite(FROM_VAR(padByte), 2, dbFile);
    fwrite(FROM_VAR(todo.str_offset), 1, dbFile);
    fwrite(FROM_VAR(todo.str_len), 1, dbFile);
    fwrite(FROM_VAR(todo.creation_time), 1, dbFile);
    fwrite(FROM_VAR(todo.completion_time), 1, dbFile);
}

/**
 * v1: id, bool(done,3 pad), str_offset, str_len
 */
TodoItem item_read_next_v1(FILE* dbFile)
{
    TodoItem item = {};
    char* devNull[4];

    fread(&item.id, sizeof(int), 1, dbFile);
    fread(&item.done, sizeof(bool), 1, dbFile);
    // padding exists, because first version would just
    // write the struct directly into memory
    // it is not needed in future versions
    fread(devNull, sizeof(bool), 3, dbFile);
    fread(&item.str_offset, sizeof(int), 1, dbFile);
    fread(&item.str_len, sizeof(int), 1, dbFile);

    return item;
}

/**
 * v1: id, bool(done,deleted,2 pad), str_offset, str_len
 */
TodoItem item_read_next_v2(FILE* dbFile)
{
    TodoItem item = {};
    char* devNull[4];

    fread(&item.id, sizeof(int), 1, dbFile);
    fread(&item.done, sizeof(bool), 1, dbFile);
    fread(&item.deleted, sizeof(bool), 1, dbFile);
    fread(devNull, sizeof(bool), 2, dbFile);
    fread(&item.str_offset, sizeof(int), 1, dbFile);
    fread(&item.str_len, sizeof(int), 1, dbFile);

    // setting default/initial value
    item.creation_time = time(NULL);

    return item;
}

/**
 * v1: id, bool(done,deleted,2 pad), str_offset, str_len, timestamp
 */
TodoItem item_read_next_v3(FILE* dbFile)
{
    TodoItem item = {};
    char* devNull[4];

    fread(&item.id, sizeof(int), 1, dbFile);
    fread(&item.done, sizeof(bool), 1, dbFile);
    fread(&item.deleted, sizeof(bool), 1, dbFile);
    fread(devNull, sizeof(bool), 2, dbFile);
    fread(&item.str_offset, sizeof(int), 1, dbFile);
    fread(&item.str_len, sizeof(int), 1, dbFile);
    fread(&item.creation_time, sizeof(time_t), 1, dbFile);

    // default value for migration
    if (item.done) item.completion_time = item.creation_time;

    return item;
}

/**
 * v1: id, bool(done,deleted,2 pad), str_offset, str_len, timestamp
 */
TodoItem item_read_next_v4(FILE* dbFile)
{
    TodoItem item = {};
    char* devNull[4];

    fread(&item.id, sizeof(int), 1, dbFile);
    fread(&item.done, sizeof(bool), 1, dbFile);
    fread(&item.deleted, sizeof(bool), 1, dbFile);
    fread(devNull, sizeof(bool), 2, dbFile);
    fread(&item.str_offset, sizeof(int), 1, dbFile);
    fread(&item.str_len, sizeof(int), 1, dbFile);
    fread(&item.creation_time, sizeof(time_t), 1, dbFile);
    fread(&item.completion_time, sizeof(time_t), 1, dbFile);

    return item;
}

TodoItem todo_read_next_item(FILE* dbFile, TodoDbHeader header)
{
    switch (header.version)
    {
        case 1: return item_read_next_v1(dbFile);
        case 2: return item_read_next_v2(dbFile);
        case 3: return item_read_next_v3(dbFile);
        case 4: return item_read_next_v4(dbFile);
        default:
        {
            str_printc("DB Version % not implemented!", fmt_n(header.version));
            ASSERT(false, "Reading version %i not implemented!",
                   header.version);
            return (TodoItem){};
        };
    }
}

// DECISION: I'm not rewriting the whole file, because the complexity of
//  reopening and closing the file is just not worth it,
//  while the drawback of potential garbage bytes at the end of the file
//  is minimal
//  Reading is always header count based & it will also correct itself
//  as new todos get added
void tddb_upgrade_version(FILE* dbFile, TodoDb db)
{
    if (db.header.version == TODO_DB_VERSION) return;

    str_printc("Version change detected. Migrating db v% to v%",
               fmt_n(db.header.version), fmt_n(TODO_DB_VERSION));

    copy_file(dbFile, str_formatc("%.db_v%_bu", fmt_s(db.files.db),
                                  fmt_n(db.header.version)));

    todo_write_header(dbFile, db.header);

    // NOTE: if sizeof(TodoItem) decreases we might have garbage bytes
    // at the end of the file
    for (int i = 0; i < db.header.count; i++)
    {
        tddb_write_next_item(dbFile, db.item_arr[i], STREAM_CUR);
    }
}

TodoDbHeader tddb_increment_header(FILE* dbFile)
{
    // STFO: how to handle invalid header file?
    //  -> how to handle file empty?
    TodoDbHeader header = tddb_read_header(dbFile);

    header.max_id++;
    header.count++;

    todo_write_header(dbFile, header);

    return header;
}

bool todo_invalid(TodoItem item)
{
    if (item.id == 0) return true;
    return false;
}

/*
 * Reads & upgrades
 * TodoItem in-memory will always be the current version
 * So we can run the file upgrade after
 */
TodoDb tddb_read_upgrade(Arena* mem, str dbPath)
{
    TodoDb db = {.files = {.db = dbPath}};

    ASSERT(mem->memory, "Memory is uninitialized");
    arena_reset(mem);

    FILE* dbFile = fopen(dbPath.chars, "r+");
    if (dbFile)
    {
        db.header = tddb_read_header(dbFile);
        db.item_arr = arena_use(mem, sizeof(TodoItem) * db.header.count);

        for (int i = 0; i < db.header.count; i++)
        {
            db.item_arr[i] = todo_read_next_item(dbFile, db.header);
        }

        tddb_upgrade_version(dbFile, db);

        // NOTE: fclose is not NULL safe!!!
        fclose(dbFile);
    }
    else
    {
        str_printc("[ERR] Db file not found!", fmt_s(dbPath));
    }

    return db;
}
