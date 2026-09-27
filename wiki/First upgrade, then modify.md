#rule [[file format]]

When i need to handle upgrade & versions in a [[file format]], i have to do 
it this way always.
First run all the upgrades, and then my modification, else it will create 
a code mess.

Doing it like this, it's just like a additional (optional) step at the 
beginning of a function, and therefore easy to handle, and the rest of the 
sequence in all other functions, don't even need to change.

So the logic is cleanly separated into:
- File format consistency actions
- Current command execution actions

**Drawbacks**
- We're doing a bit more work, which means a bit more [[efficiency cost]]
   -> We need to read & write the file twice now
     => This is neglectable, because it happens very seldom

