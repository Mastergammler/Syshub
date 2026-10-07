#include "todo.h"
#include "internal.h"

void todo_print(Config config)
{
    StrPoolOptions opt = {.pool_idx = POOL_DISPLAY};
    str_pool_reset(opt);
    TodoDb db = tddb_read_upgrade(&Prog.dyn_mem, config.paths.todo_db);
    // OPTIMIZE: i can just read from the file stream directly
    //  -> I don't necessarly need to read the whole file immediately
    str strings = file_read_all(config.paths.todo_strings);

    str lineEl = str_static("_");
    str line = str_repeat(opt, lineEl, config.max_col);
    str_print(line);

    str ws0 = {};
    str ws1 = str_static(" ");
    str ws2 = str_static("  ");

    for (int i = 0; i < db.header.count; i++)
    {
        TodoItem item = db.item_arr[i];
        if (item.deleted) continue;
        if (item.done &&
            time_older_than_d(item.completion_time, config.hide_age_days))
            continue;

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
                   item.done ? (FmtHeader*)fmt_s(str_static("✔"))
                             : (FmtHeader*)fmt_c(' '),
                   fmt_s(text), fmt_s(padRight), fmt_s(clear));
    }
}

TodoResult todo_add(Files files, str text)
{
    TodoDb db = tddb_read_upgrade(&Prog.dyn_mem, files.db);

    FILE* dbFile = fopen(files.db.chars, "r+");
    if (!dbFile)
    {
        dbFile = fopen(files.db.chars, "w+");
    }
    FILE* stringsFile = fopen(files.strings.chars, "a");

    db.header = tddb_increment_header(dbFile);
    int posInFile = append_as_line(stringsFile, text);
    TodoItem newItem = {.id = db.header.max_id,
                        .done = false,
                        .str_offset = posInFile,
                        .str_len = text.len,
                        .creation_time = time_utc_now(),
                        .completion_time = 0};

    tddb_write_next_item(dbFile, newItem, STREAM_END);

    fclose(dbFile);
    fclose(stringsFile);

    return TODO_OK;
}

/*
 * NOTE: this writes the todo directly as struct
 * -> Usually I do it property by property
 * -> maybe i should unify this at some point?
 */
void write_todo_at_idx(str dbFilePath, TodoItem item, int idx)
{
    FILE* dbFile = fopen(dbFilePath.chars, "r+");
    int offset = sizeof(TodoDbHeader) + idx * sizeof(TodoItem);
    fseek(dbFile, offset, SEEK_SET);
    fwrite(&item, sizeof(TodoItem), 1, dbFile);
    fclose(dbFile);
}

/*
 * Returns TODO_NO_ACTION in the case the todo is already done
 */
TodoResult todo_mark_done(str dbPath, int requestedId)
{
    TodoDb db = tddb_read_upgrade(&Prog.dyn_mem, dbPath);

    // OPTIMIZE: if it grows, use bin search
    //  -> Because the table would already be sorted
    //  => YAGNI prolly, because i don't wanna show more than 50 items
    //     at once anyway?
    for (int i = 0; i < db.header.count; i++)
    {
        TodoItem item = db.item_arr[i];
        if (item.id == requestedId)
        {
            if (item.done) return TODO_NO_ACTION;
            item.done = true;
            item.completion_time = time_utc_now();

            write_todo_at_idx(dbPath, item, i);
            return TODO_OK;
        }
    }

    return TODO_NOT_FOUND;
}

/*
 * Does not actually remove the todo, just marks it as deleted
 *
 * TODO: File cleanup will implemented on the actual cleanup method
 * -> Overhead of rewriting everything is too high otherwise
 * -> Strings would also need to be cleaned up
 */
TodoResult todo_remove(str dbPath, int requestedId)
{
    TodoDb db = tddb_read_upgrade(&Prog.dyn_mem, dbPath);

    for (int i = 0; i < db.header.count; i++)
    {
        TodoItem item = db.item_arr[i];
        if (item.id == requestedId)
        {
            if (item.deleted) return TODO_NO_ACTION;
            item.deleted = true;

            write_todo_at_idx(dbPath, item, i);
            return TODO_OK;
        }
    }

    return TODO_NOT_FOUND;
}
