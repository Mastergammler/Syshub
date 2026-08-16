#include "internal.h"

#include <stdint.h>

#include <sys/inotify.h>
#include <unistd.h>

#define EVENT_BUF_SIZE (1024 * (sizeof(struct inotify_event) + 16))

#define ANSI_CLS "\033[2J\033[H"

void clear_terminal()
{
    str_printc(ANSI_CLS "System Hub initialized ...");
}

void print_file(str fileName)
{
    FILE* file = fopen(fileName.chars, "r");

    if (!file)
    {
        // TODO: some ansi coloring would be nice
        str_printc("[ERR] Could not open file '%'", fmt_s(fileName));
        return;
    }

    StrPoolOptions opt = {.pool_idx = POOL_FILE};
    str_pool_reset(opt);

    fseek(file, 0, SEEK_END);
    int fileLen = ftell(file);
    fseek(file, 0, SEEK_SET);

    char* srcPtr = pool_use(opt, fileLen);
    str content = {.chars = srcPtr, .len = fileLen};
    fread(srcPtr, 1, fileLen, file);

    // str_print(content);
    fwrite(content.chars, 1, content.len, stdout);
    fflush(stdout);
}

int main(int argc, char** argv)
{
    init_program(4096);

    StrPoolOptions opt = {.pool_idx = POOL_DEFAULT};
    Config c = load_config(&Prog.memory, opt);

    int fd = inotify_init1(0);
    if (fd == -1)
    {
        str_printc("[ERR] Unable to open notify file");
        return 2;
    }

    StrSplitResult fileNameSplit = str_split_last(c.todo_file, '/');

    // NOTE: we need to watch the parent dir, else it doesn't work
    //  -> becaues nvim replaces the whole file on save
    int wd = inotify_add_watch(fd, fileNameSplit.head.chars,
                               IN_MODIFY | IN_CLOSE_WRITE | IN_MOVED_TO);
    if (wd == -1)
    {
        str_printc("[ERR] Unalbe to add notify watcher");
        close(fd);
        return 3;
    }

    clear_terminal();
    str_printc("File: %", fmt_s(c.todo_file));
    print_file(c.todo_file);

    char buffer[EVENT_BUF_SIZE];
    // wait loop, will get invoked by the notify system
    while (1)
    {
        int n = read(fd, buffer, sizeof(buffer));

        if (n == -1)
        {
            str_printc("[ERR] Event loop crashed, unalbe to read events");
            return 4;
        }

        clear_terminal();
        print_file(c.todo_file);
    }

    return 0;
}
