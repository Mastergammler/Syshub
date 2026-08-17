#include "internal.h"
#include "todo.h"

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

    StrPoolOptions opt = {.pool_idx = POOL_DISPLAY};
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

void print_mem_used(str title, uint64_t used, uint64_t max, int memDisplayMax)
{
    StrPoolOptions opt = {.pool_idx = POOL_DISPLAY};

    int usedCharCount = (float)used / max * memDisplayMax;

    str usedChars = str_repeat(opt, str_static(":"), usedCharCount);
    str padRight =
        str_repeat(opt, str_static(" "), memDisplayMax - usedCharCount);

    // TODO: MGLIBC PadLeft with char function (for space etc)
    str_printc("[%%] %/% %", fmt_s(usedChars), fmt_s(padRight),
               fmt_n(used, .places = 4), fmt_n(max, .places = 4), fmt_s(title));
}

void print_memory(Config config)
{
    StrPoolOptions opt = {.pool_idx = POOL_DISPLAY};
    int memDisplayMax = 40;

    str_printc("");
    StrPool* pool = POOL(POOL_DEFAULT);
    print_mem_used(str_static("StrDefault"), pool->cursor_idx, pool->capacity,
                   memDisplayMax);
    StrPool* tmp = POOL(POOL_DISPLAY);
    print_mem_used(str_static("StrDisplay"), tmp->cursor_idx, tmp->capacity,
                   memDisplayMax);
    print_mem_used(str_static("Program(Dyn)"), Prog.dyn_mem.cursor,
                   Prog.dyn_mem.capacity, memDisplayMax);
}

void refresh_view(Config config)
{
    clear_terminal();
    print_memory(config);
    todo_print(config);
}

int main(int argc, char** argv)
{
    init_program(4096);

    StrPoolOptions opt = {.pool_idx = POOL_DEFAULT};
    Config config = config_load();

    int fd = inotify_init1(0);
    if (fd == -1)
    {
        str_printc("[ERR] Unable to open notify file");
        return 2;
    }

    StrSplitResult fileNameSplit = str_split_last(config.todo_file, '/');
    // NOTE: we need to watch the parent dir, else it doesn't work
    //  -> becaues nvim replaces the whole file on save
    int wd = inotify_add_watch(fd, str_cstr(fileNameSplit.head),
                               IN_MODIFY | IN_CLOSE_WRITE | IN_MOVED_TO);
    if (wd == -1)
    {
        str_printc("[ERR] Unalbe to add notify watcher");
        close(fd);
        return 3;
    }

    refresh_view(config);

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

        refresh_view(config);
    }

    return 0;
}
