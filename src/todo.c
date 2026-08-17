#include "todo.h"
#include "internal.h"

void todo_print(Config config)
{
    StrPoolOptions opt = {.pool_idx = POOL_DISPLAY};
    str_pool_reset(opt);
    TodoDb db = todo_read_db(&Prog.dyn_mem, config.todo_db_file);
    str strings = file_read_all(config.todo_strings);

    str lineEl = str_static("_");
    str line = str_repeat(opt, lineEl, config.max_col);
    str_print(line);

    str ws0 = {};
    str ws1 = str_static(" ");
    str ws2 = str_static("  ");

    for (int i = 0; i < db.header.count; i++)
    {
        TodoItem item = db.item_arr[i];
        str ws = item.id > 9 ? ws0 : ws1;
        str text =
            str_sub(strings, item.str_offset, item.str_offset + item.str_len);

        str ansi = {};
        str clear = str_static("\033[0m");
        if (item.done)
        {
            ansi = str_formatc_opt(opt, "\033[38;5;%m",
                                   fmt_n(config.todo_fin_color, .places = 3));
        }

        // TODO: this doesn't look as nice
        // int printLen = 9 + text.len;
        // int missing = config.max_col - printLen;
        str padRight = {}; // str_repeat(opt, ws1, missing);

        str_printc("%%[%] (%) %%%", fmt_s(ansi), fmt_s(ws), fmt_n(item.id),
                   item.done ? fmt_s(str_static("✔")) : fmt_c(' '), fmt_s(text),
                   fmt_s(padRight), fmt_s(clear));
    }
}

void todo_add(TodoDb db, str text)
{
    FILE* dbFile = fopen(db.db_file.chars, "r+");
    if (!dbFile)
    {
        // str_printc("[ERR] File doesn't yet... creating");
        dbFile = fopen(db.db_file.chars, "w+");
    }
    FILE* stringsFile = fopen(db.strings_file.chars, "a");

    fread(&db.header, sizeof(db.header), 1, dbFile);

    db.header.max_id++;
    db.header.count++;

    fseek(dbFile, 0, SEEK_SET);
    fwrite(&db.header, sizeof(db.header), 1, dbFile);
    // append text
    int fileLenB4Write = ftell(stringsFile);
    fwrite(text.chars, 1, text.len, stringsFile);
    // str_printc("Printing into file: %", fmt_s(text));
    fputc('\n', stringsFile);

    TodoItem newItem = {.id = db.header.max_id,
                        .done = false,
                        .str_offset = fileLenB4Write,
                        .str_len = text.len};

    fseek(dbFile, 0, SEEK_END);
    fwrite(&newItem, sizeof(TodoItem), 1, dbFile);

    fclose(dbFile);
    fclose(stringsFile);
}

// TODO: actually 3 modes, found, not found, already done
bool todo_mark_done(str dbPath, int requestedId)
{
    TodoDb db = todo_read_db(&Prog.dyn_mem, dbPath);

    // OPTIMIZE: if it grows, use bin search
    //  -> Because the table would already be sorted
    for (int i = 0; i < db.header.count; i++)
    {
        TodoItem item = db.item_arr[i];
        if (item.id == requestedId)
        {
            FILE* dbFile = fopen(dbPath.chars, "r+");
            item.done = true;

            int offset = sizeof(TodoDbHeader) + i * sizeof(TodoItem);

            fseek(dbFile, offset, SEEK_SET);
            fwrite(&item, sizeof(TodoItem), 1, dbFile);

            return true;
        }
    }

    return false;
}

TodoDb todo_read_db(Arena* mem, str dbPath)
{
    TodoDb db = {};

    ASSERT(mem->memory, "Memory is uninitialized");
    arena_reset(mem);

    FILE* dbFile = fopen(dbPath.chars, "r");
    if (dbFile)
    {
        fread(&db.header, sizeof(db.header), 1, dbFile);
        db.item_arr = arena_use(mem, sizeof(TodoItem) * db.header.count);
        fread(db.item_arr, sizeof(TodoItem), db.header.count, dbFile);

        // NOTE: fclose is not NULL safe!!!
        fclose(dbFile);
    }
    else
    {
        str_printc("[ERR] Db file not found!", fmt_s(dbPath));
    }

    return db;
}
