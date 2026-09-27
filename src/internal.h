#ifndef SYSHUB_INTERNAL
#define SYSHUB_INTERNAL

#include "time.h"
#include "types.h"
#include <alloc/module.h>
#include <string/macros.h>
#include <string/module.h>

#define TODO_DB str_static("todo.db")
#define TODO_STR str_static("todo.str")
#define BU_EXT str_static("bu")

#define CONFIG_MAP(varExpr) NAMEOF(varExpr), &varExpr
#define FROM_VAR(var) &var, sizeof(var)

typedef enum
{
    POOL_DEFAULT,
    POOL_PRINT,
    POOL_DISPLAY,
    POOL_count
} StrPools;

typedef struct
{
    // Timer timer;
    Arena mem;
    Arena dyn_mem;
    AllocContext mem_ctx;
    StringMemory strings;

} Program;

extern Program Prog;

Config config_load();
str file_read_all(str path);
void init_program(uint64_t memory);
str build_arg_string(int argc, char** argv, bool includeNl);

#endif
void copy_content_buffered(FILE* contentFile, FILE* destFile);
int append_as_line(FILE* file, str text);
void copy_file(FILE* stream, str targetFile);
time_t time_utc_now();
bool time_older_than_d(time_t compareTime, int days);
