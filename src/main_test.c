#include "internal.h"
#include "todo.h"

int main(int argc, char** argv)
{
    init_program(4096);

    Config config = config_load();
    todo_print(config);

    return 0;
}
