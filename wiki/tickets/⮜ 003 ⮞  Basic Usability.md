# Usability features

#ticket/open

## Todos
- [x] Migrate to folder path instead
- [x] Fix memory distribution (strDisplay needs the majority)
- [x] Remove note function
- [x] Add timestamp for filtering
- [ ] Clear function (store to history file or something?)
- [ ] Update usage color (green/yellow/red - gray bg)

## Description
Removing a note is quite important, because sometimes i might make a mistake,
i mean i could also do a edit function instead, but dunno

**Deletion**
If i delete something in the middle, this always leads to problems, because 
now i need to rewrite the whole db / move it forward. The same problem will 
then exist in the strings files, where i have unused strings.

I think the simpler approach might be to add a flag `deleted` and just filter 
out the items from the display.
And then i can have a additional [[defragmentation]] or [[cleanup]] function,
which with you can periodically do a db sweep or something.

**Migrate function**
I need a [[migrate]] function for the db, for cases whenever i change the 
dataset, else i would need to recreate it every time.
I don't have a smart idea how to do it in another way, like [[@John Blow]] has 
with his [[versioning annotations]], dunno how i would do this in c.
Maybe that's just [[out of scope]] for this tool? I can do it when i 
[[rewrite in jai]]

**Folder path**
I want to have some db folder where i can put all the files in, instead of just 
having all these file paths in the config.
Because as the db grows (history file, todo file, strings files etc etc), it 
becomes very tidious to have them all there, and it scales horribly

