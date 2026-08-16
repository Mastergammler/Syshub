#include <string/module.h>

StringMemory* StrMem;

int main(int argc, char** argv)
{
    // TODO: load from config
    str file = str_static("test.txt");
    FILE* tododb = fopen(file.chars, "a");

    str todoText = str_static(argv[1]);

    // fmt accordingly etc
    // fwrite(todoText.chars, 1, todoText.len, tododb);
    fprintf(tododb, "\n%s", argv[1]);
    fclose(tododb);

    return 0;
}
