# Migration function & flexible db format
#ticket/open

## Todos
- [ ] Tool to push bytes to the start of the file (version / magic)
- [ ] Implement versioning approach
- [ ] Create BU files, when version is updated

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




