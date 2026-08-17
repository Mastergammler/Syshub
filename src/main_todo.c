#include "internal.h"
#include "todo.h"

int main(int argc, char** argv)
{
    init_program(4096);

    if (argc < 2)
    {
        str_printc("Usage: <todo text>");
        return 0;
    }
    Config config = config_load();
    TodoDb db = {.db_file = config.todo_db_file,
                 .strings_file = config.todo_strings};

    str firstArg = str_alloc(argv[1]);
    if (str_starts_with(firstArg, str_static("/fin")))
    {
        if (argc != 3)
        {
            str_printc("Usage: /fin <todo-id>");
            return 0;
        }

        int requestedId = atoi(argv[2]);
        if (!todo_mark_done(db.db_file, requestedId))
        {
            str_printc("-> Todo with the id % not found", fmt_n(requestedId));
        }
    }
    else
    {
        str todoText = build_arg_string(argc, argv, false);
        todo_add(db, todoText);
    }

    return 0;
}
