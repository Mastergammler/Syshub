
When i refactor a config value, it will not change the name automatically.
And the [[lsp]] is also not detecting it as non existing property anymore.
This is quite [[annoying]] and leads to easy errors.

```cpp
match_str(cur, NAMEOF(config.db_folder), &config.db_folder);
```

**Solution**
Dunno, the lsp should catch these cases, but i guess this is a limitation of 
the language. It happens ofc, because the macro just turns into a string, and 
therefore it does not evaluate the input as expression.
I'm not sure if there is another way to map these values to the properties of 
a file by name though...

=> Maybe as single macro, for both values?
```cpp
#define CONFIG_MAP(varExpr) NAMEOF(varExpr), &varExpr
```
-> Yes, this actually solves it perfectly, because the right expression is 
   evaulated by the compiler, and the macro keeps both values in sync!
