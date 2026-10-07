# Migration function & flexible db format
#ticket/closed

## Todos
- [x] Tool to push bytes to the start of the file (version / magic)
- [x] Adjust loading via handling version approach
- [x] Don't remove Padding, because when i write into the table i use offset
- [x] Implement versioning approach
- [x] Create BU files, when version is updated
- [ ] ~Handling invalid header files?~
- [x] Implement Timestamp
    - [x] Syshub hide time config value


## Ideas
- Something like a [[WAD]] file? It needs some header describing the content?
 -> This should work

**Versioning approach**

> *The Version describes a sequence of read operations*

- Read operations should be atomic, to ensure that the [[file format]]
  and the [[struct]] are only [[loosly coupled]]
  (so that a reorder or addition in the struct doesn't break the reading)
- Every version has it's own read method `read_db_v<versionId>(...)`
  which stays consistent over time
 -> It only needs to change when there was a deletion, in that case it just 
    needs to skip X bytes
- On write, always the current version is written to file
 -> There will be only a single write method, for the current version

```cpp
Header { magic, version, count, max_id }
TodoItem {...}

switch(header.version)
{
    case 1: read_db_v1(...);
    case 2: read_db_v2(...);
    default: NOT_IMPLEMENTED();
}

TodoItem read_db_v1(file)
{
    TodoItem item = {};

    void* devNull = 0x0;

    // create macros for each type?
    // INT(item.a) -> that checks size & reads it?
    ASSERT(sizeof(item.a) == sizeof(int));
    fread(&item.a,sizeof(int),file);
    fread(&item.b,sizeof(long),file);
    // in case of deletion -> skip
    fread(&devNull,sizeof(int),file);

    return item;
}

void write_db(TodoItem item)
{
    // create SYNCED macro?
    fwrite(&item.a,sizeof(a),file);
    fwrite(&item.b,sizeof(b),file);
    fwrite(&item.x,sizeof(x),file);
    fwrite(&item.z,sizeof(z),file);
}
```

### Writing Sequence

**Problem:** Every write/modify operation requires a db update.

1. Read header
    -> Check if version changed
    -> Upgrade header
2. If version changed, rewrite db
3. Update the files in question

> Perf is not an issue, because db updates are infrequent
> -> Single update per version change

=> *Procedure is: [[First upgrade, then modify]]*

**Problem** Rewriting the file while it is already open

1. *Write into open stream*
    -> If the todos are shorter, there will be garbage at the end
    => It's more a imperfection, because i read based on count always
2. Reopen & rewrite the file
    -> Now i need to reopen the file stream -> more overhead
    -> I need to reopen twice, because different file modes ...

=> *Garbage at the end is probably very irrelevant & will be overwritten 
    later anyway, so i think this is fine*

### Issues
- I can not remove the padding, because i use the size of the Todo item in the 
  table for writing the offset
    -> This only applies to cases, where the header already matches
    -> Else i would rewrite the whole table anyway, but it makes things too 
       compliacted

