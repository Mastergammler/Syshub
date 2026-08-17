- Communication via [[FIFO unix pipes]] for the start
 -> Needed especially for data input of the ui (update, change etc)
 -> If needed we can move to [[unix sockets]] later ([[Low Level Academy]])

**General idea**
- Syshub /display just watches a bunch of files and displays them
- It additionally reads from [[FIFO Pipes]] for simple command changes
     - eg change page
     - filter stuff
     - etc etc
- Have a bunch of little unix programs, that handle the input
    - creating todos -> just write it to the file(s)
    - sending specific commands to the [[syshub]]
- Migrating [[stt]] to a proper cli tool as well
    - but this needs to handle the [[sql connection]], which might be a bit 
      more difficult

**Integration**
- integrate it with the rest of my [[pkm]] systems via commands
-> `ticket /add <project-path>` - requires project with .vault file
-> Automatically open the ticket file in nvim after?
-> It needs to run all the ticket creating commands in the right folder
   (change cwd)
- [[stt]] & ticket linking
-> Automatically creating an entry in the appropriate project
-> Maybe sync it with the folder structure? (Does this make sense?)

