#include "internal.h"
#include "todo.h"

bool is_num(str str)
{
    if (str.len == 0) return false;

    for (int i = 0; i < str.len; i++)
    {
        if (str.chars[i] < '0' || str.chars[i] > '9') return false;
    }

    return true;
}

str construct_byte_sequence(int argc, char** argv)
{
    void* bytes = arena_use(&Prog.dyn_mem, 0);
    for (int i = 2; i < argc; i++)
    {
        // it's ok here, because it's only shortlived
        str str = str_static(argv[i]);

        if (is_num(str))
        {
            ParseNRes numRes = str_parse_n(str);
            int parsed = (int)numRes.val;
            int* num = arena_use(&Prog.dyn_mem, sizeof(int));
            *num = parsed;
        }
        else
        {
            char* chars = arena_use(&Prog.dyn_mem, str.len);
            memcpy(chars, str.chars, str.len);
        }
    }

    void* last = arena_use(&Prog.dyn_mem, 0);

    return (str){.chars = bytes, .len = last - bytes};
}

/*
 * Prepends bytes to a file, parses numbers to int bytes
 *
 * Dependencies: cp utility
 */
int main(int argc, char** argv)
{
    init_program(4096);

    if (argc < 3)
    {
        str_printc("Usage: <file to prepend> [<prepend args>...]");
        return 0;
    }

    str srcPath = str_alloc(argv[1]);
    str tmpPath = str_static("tmp.bin");
    str byteSeq = construct_byte_sequence(argc, argv);

    FILE* tmpFile = fopen(tmpPath.chars, "wb");
    FILE* srcFile = fopen(srcPath.chars, "rb");

    if (!srcFile)
    {
        str_printc("Error: Could not open file: '%'", fmt_s(srcPath));
        return -1;
    }

    if (!tmpFile)
    {
        str_printc("Error: Could not open tmp file: '%'", fmt_s(tmpPath));
        return -1;
    }

    fwrite(byteSeq.chars, byteSeq.len, 1, tmpFile);
    copy_content_buffered(srcFile, tmpFile);

    fclose(tmpFile);
    fclose(srcFile);

    // create backup before override etc
    system(str_formatc("cp % %.bu", fmt_s(srcPath), fmt_s(srcPath)).chars);
    if (rename(tmpPath.chars, srcPath.chars) != 0)
    {
        str_printc("Unable to move file % to %", fmt_s(tmpPath),
                   fmt_s(srcPath));
    }
    return 0;
}
