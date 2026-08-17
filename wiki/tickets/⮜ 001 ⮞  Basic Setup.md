# Basic setup

#ticket/closed

> DONE: File DB setup
> NEXT: Integrate into syshub
> OPEN: Actually creating the finish command

## Todos
- [x] Reading file from config
- [x] Proper cli input ("squash arguments")
- [x] Actually creating a indexdb to finish this
    - [x] Implement proper display into syshub
- [x] Possibility to finish todos by id
    - [x] Add handling `/cmd` things
    - [x] modify db file via index
- [x] Draw background color for finished items
- [x] Fix syshub realloc problem (filling up the frame buffer)
- [x] Add ASCII memory use overview
- [x] Add visual alignment

## Description
**IndexDB**
- Bin file: `Id, Done, StrOffset, StrLen`
- Strings file: `Text`
=> Need to watch both? No, just the Db file, end then lookup in the other

```cpp
struct TodoDb
{
    int count; 
    int max_id;
    TodoItem* items;
    str db_file;
    str strings_file;
};

// add timestamps later
struct TodoItem
{
    int id;
    bool done;
    int str_offset;
    int str_len;
}
```

**Memory overview**
`[::::::::::::::::::::::::::   ] 3500/4096 Program` can do it like this
`[:::::::::::::::::::::        ]  230/1024 DefaultPool` can do it like this
