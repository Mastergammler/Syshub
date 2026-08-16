#include <stdint.h>
#include <string/macros.h>
#include <string/module.h>

#include <sys/inotify.h>
#include <unistd.h>

#define EVENT_BUF_SIZE (1024 * (sizeof(struct inotify_event) + 16))

typedef enum
{
    POOL_DEFAULT,
    POOL_PRINT,
    POOL_FILE,
    POOL_count
} StrPools;

typedef struct
{
    // Timer timer;
    Arena memory;
    AllocContext mem;
    StringMemory strings;

} Program;
static Program Prog;
StringMemory* StrMem;

void init_program(uint64_t memory)
{
    MglibcAllocCtx = &Prog.mem;
    Prog.mem = ctx_init_default(&Prog.memory);

    // timer_start(&Prog.timer);
    arena_init(&Prog.memory, memory);

    str_pool_prepare(Prog.mem, &Prog.strings, POOL_count);
    define_pool(POOL_DEFAULT, 256, .is_default = true);
    define_pool(POOL_FILE, 1024);
    define_pool(POOL_PRINT, 256, .is_print = true);
    str_pool_finalize(Prog.mem);
}

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

    if (argc < 2)
    {
        str_printc("Usage: <file-to-watch>");
        return 1;
    }

    str fileArg = str_alloc(argv[1]);

    int fd = inotify_init1(0);
    if (fd == -1)
    {
        str_printc("[ERR] Unable to open notify file");
        return 2;
    }

    // NOTE: we need to watch the parent dir, else it doesn't work
    //  -> becaues nvim replaces the whole file on save
    int wd = inotify_add_watch(fd, "." /*fileArg.chars*/,
                               IN_MODIFY | IN_CLOSE_WRITE | IN_MOVED_TO);
    if (wd == -1)
    {
        str_printc("[ERR] Unalbe to add notify watcher");
        close(fd);
        return 3;
    }

    clear_terminal();
    print_file(fileArg);

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
        print_file(fileArg);
    }

    return 0;
}
