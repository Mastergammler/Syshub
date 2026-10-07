# Optimizing strings file memory handling
#ticket/closed

## Todo
- [x] Remove loading whole file
- [x] Read strings via file stream directly
- [x] Leave more space for the actual db file

## Notes
- This is a temporary solution, because i also need a clean db file
    -> Else the db file will just grow endlessly
- But for the strings i actually don't need that much memory!
