# Adding new fields/properties in the TodoItem
1. Add the field in the [[TodoItem]]
2. Modify the `tddb_write_next_item` to accomodate new property
3. Create the next `item_read_next_vX` method
4. Specify it in the `todo_read_next_item`  
5. Increase version constant in `todo.h`
6. (Opt) Add a migration strategy from the last version to this one
    -> Choosing a default value on read

=> Alignment has to be correct, alignment bytes have to be read and written!

> !! *The migration is currently limited to the last version only (eg v2 -> v3)
      migration from v1 -> v3 will not do the same* !!
