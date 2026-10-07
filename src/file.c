#include "internal.h"

FsRes file_read(str path)
{
    FsRes res = {.path = path};
    res.stream = fopen(path.chars, "r");

    if (res.stream)
    {
        fseek(res.stream, 0, SEEK_END);
        res.len = ftell(res.stream);
        res.open = true;

        fseek(res.stream, 0, SEEK_SET);
    }

    return res;
}

void file_close(FsRes res)
{
    // fclose is NOT null safe
    if (res.stream) fclose(res.stream);
}

/*
 * WARN: Modifies the file stream position
 */
str file_read_section(FsRes file, Section sec, StrPoolOptions opt)
{
    bool startExceedsLen = file.len < sec.offset;
    str res = {};
    if (file.open && !startExceedsLen)
    {
        int endPos = min(sec.offset + sec.len, file.len);
        int validLen = endPos - sec.offset;

        res.len = validLen;
        res.chars = pool_use(opt, validLen);

        fseek(file.stream, sec.offset, SEEK_SET);
        fread((void*)res.chars, 1, res.len, file.stream);
    }

    return res;
}

str file_read_all(str path)
{
    FILE* file = fopen(path.chars, "r");

    if (!file)
    {
        // TODO: some ansi coloring would be nice
        str_printc("[ERR] Could not open file '%'", fmt_s(path));
        return (str){};
    }

    StrPoolOptions opt = {.pool_idx = POOL_DISPLAY};
    str_pool_reset(opt);

    fseek(file, 0, SEEK_END);
    int fileLen = ftell(file);
    fseek(file, 0, SEEK_SET);

    char* srcPtr = pool_use(opt, fileLen);
    str content = {.chars = srcPtr, .len = fileLen};
    fread(srcPtr, 1, fileLen, file);
    fclose(file);

    return content;
}

void copy_content_buffered(FILE* contentFile, FILE* destFile)
{
    ASSERT(contentFile, "Content stream not valid!");
    ASSERT(destFile, "Dest stream not valid!");

    int originalFilePos = ftell(contentFile);
    fseek(contentFile, 0, SEEK_SET);

    int n;
    char buf[1024];
    while ((n = fread(buf, 1, sizeof(buf), contentFile)) > 0)
    {
        if (fwrite(buf, 1, n, destFile) != n)
        {
            str_printc("Error during copy operation");
            break;
        }
    }

    fseek(contentFile, originalFilePos, SEEK_SET);
}

/*
 * Returns the byte pos of the appended text in the file
 */
int append_as_line(FILE* file, str text)
{
    int textStartPos = ftell(file);
    fwrite(text.chars, 1, text.len, file);
    fputc('\n', file);

    return textStartPos;
}

void copy_file(FILE* stream, str targetFile)
{
    ASSERT(stream, "Filestream not valid!");

    FILE* copy = fopen(targetFile.chars, "wb");
    if (copy)
    {
        copy_content_buffered(stream, copy);
        fclose(copy);
    }
}
