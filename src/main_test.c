#include "internal.h"
#include "todo.h"

int main(int argc, char** argv)
{
    init_program(4096);
    str_printc("Test started ...");

    Config config = config_load();
    todo_print(config);

    return 0;
}
