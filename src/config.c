#include "internal.h"

#define NAMEOF(x) str_static(#x)

typedef struct
{
    str key;
    str value;
} Kvp;

Kvp* parse_config(Arena* arrMem, str config)
{
    Kvp* array = NULL;

    // state
    Kvp kvp = {};
    str curVal = {};

    for (int c = 0; c < config.len; c++)
    {
        char cur = config.chars[c];

        if (cur == '\n' || c == config.len - 1)
        {
            // TODO: problem if no '=' was found
            kvp.value = curVal;
            if (kvp.key.len > 0 && kvp.value.len > 0)
            {
                arr_append(array, kvp);
            }

            kvp = (Kvp){};
            curVal = (str){};
            continue;
        }

        // split condition
        if (cur == '=')
        {
            kvp.key = curVal;
            curVal = (str){};
            continue;
        }

        if (cur == ' ')
        {
            continue;
        }

        if (curVal.len == 0) curVal.chars = &config.chars[c];

        curVal.len++;
    }

    return array;
}

Config load_config(Arena* alloc, StrPoolOptions opt)
{
    Config config = {};

    str home = str_static(getenv("HOME"));
    str configRel = str_static(".config/syshub/config");
    str configPath = str_formatc("%/%", fmt_s(home), fmt_s(configRel));

    FILE* configFile = fopen(configPath.chars, "r");
    if (!configFile)
    {
        str_printc("[ERR] Unable to read config file: %", fmt_s(configPath));
        return config;
    }

    fseek(configFile, 0, SEEK_END);
    int fileLen = ftell(configFile);
    fseek(configFile, 0, SEEK_SET);

    char* text = pool_use(opt, fileLen);
    str configContent = {.chars = text, .len = fileLen};
    fread(text, 1, fileLen, configFile);
    fclose(configFile);

    // str_print(configContent);
    Kvp* configArr = parse_config(alloc, configContent);

    /*str_printc("Found config of len %, with % entries", fmt_n(fileLen),
               fmt_n(arr_len(configArr)));*/

    for (int i = 0; i < arr_len(configArr); i++)
    {
        Kvp cur = configArr[i];
        if (str_ends_with(NAMEOF(Config.todo_file), cur.key))
        {
            // need to realloc, to ensure proper null termination
            config.todo_file = str_allocn(cur.value.chars, cur.value.len);
        }
    }

    return config;
}
