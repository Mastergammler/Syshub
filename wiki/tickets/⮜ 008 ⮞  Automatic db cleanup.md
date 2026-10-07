# Automatic db cleanup
#ticket/open

## Todo
- [ ] Implement auto cleanup for db

## Notes
- The db file will always grow, at some point it will not fit into the memory 
  anymore
    -> I can check beforehand (based on header) if it can fit
    -> If not run a db cleanup (remove deleted & past todos to archive file)
    -> Also cleanup strings file, which requires a rewrite of all the offsets
        => This is probably the most tricky part
- Probably remove deleted todos completely
- Another potential problem -> migrating the archive file when version changed
    -> Maybe it's not a big issue, because i can just run it before
    -> And then append the newly cleaned up files
- I can detect, when the memory is not sufficient to load the whole db
    -> And then run the cleanup command
    -> But also allow for manual cleanup triggering (easier for testing)
