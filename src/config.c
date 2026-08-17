#include "internal.h"

#define NAMEOF(x) str_static(#x)

typedef struct
{
    str key;
    str value;
} Kvp;

// NOTE: uses the dynamic memory in between!
Kvp* parse_config(str config)
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

void match_str(Kvp cur, str name, str* configVal)
{
    if (str_ends_with(name, cur.key))
    {
        // realloc because we need null termination for posix api
        *configVal = str_allocn(cur.value.chars, cur.value.len);
    }
}

void match_num(Kvp cur, str name, int* num)
{
    if (str_ends_with(name, cur.key))
    {
        *num = str_parse_n(cur.value).val;
    }
}

Config config_load()
{
    Config config = {};

    str home = str_static(getenv("HOME"));
    str configRel = str_static(".config/syshub/config");
    str configPath = str_formatc("%/%", fmt_s(home), fmt_s(configRel));
    str configContent = file_read_all(configPath);
    Kvp* configArr = parse_config(configContent);

    for (int i = 0; i < arr_len(configArr); i++)
    {
        Kvp cur = configArr[i];
        match_str(cur, NAMEOF(Config.todo_file), &config.todo_file);
        match_str(cur, NAMEOF(config.todo_db_file), &config.todo_db_file);
        match_str(cur, NAMEOF(config.todo_strings), &config.todo_strings);
        match_num(cur, NAMEOF(config.max_col), &config.max_col);
        match_num(cur, NAMEOF(config.todo_fin_color), &config.todo_fin_color);
    }

    return config;
}
