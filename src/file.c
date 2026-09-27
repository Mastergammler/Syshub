#include "internal.h"

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
