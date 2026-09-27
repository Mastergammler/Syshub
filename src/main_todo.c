#include "internal.h"
#include "todo.h"

int main(int argc, char** argv)
{
    init_program(4096);

    if (argc < 2)
    {
        str_printc("Usage: <todo text> | /<cmd> [args...]");
        return 0;
    }
    Config config = config_load();
    Files files = {.db = config.paths.todo_db,
                   .strings = config.paths.todo_strings};

    str firstArg = str_alloc(argv[1]);
    if (str_starts_with(firstArg, str_static("/fin")))
    {
        if (argc != 3)
        {
            str_printc("Usage: /fin <todo-id>");
            return 0;
        }

        int requestedId = atoi(argv[2]);
        TodoResult res = todo_mark_done(files.db, requestedId);
        if (res == TODO_NOT_FOUND)
        {
            str_printc("-> Todo with the id % not found", fmt_n(requestedId));
        }
        else if (res == TODO_NO_ACTION)
        {
            str_printc("-> Todo with the id % already done",
                       fmt_n(requestedId));
        }
    }
    else if (str_starts_with(firstArg, str_static("/rm")))
    {
        if (argc != 3)
        {
            str_printc("Usage: /rm <todo-id>");
            return 0;
        }

        int requestedId = atoi(argv[2]);
        TodoResult res = todo_remove(files.db, requestedId);
        if (res == TODO_NOT_FOUND)
        {
            str_printc("-> Todo with the id % not found", fmt_n(requestedId));
        }
        else if (res == TODO_NO_ACTION)
        {
            str_printc("-> Todo with the id % already marked deleted ",
                       fmt_n(requestedId));
        }
    }
    else
    {
        str todoText = build_arg_string(argc, argv, false);
        todo_add(files, todoText);
    }

    return 0;
}
