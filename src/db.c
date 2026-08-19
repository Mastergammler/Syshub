#include "internal.h"

void migrate_dbs(Config config)
{
    // NOTE: using the default pool which should be fine, since this is a
    // standalone run utility
    str dbBu = str_formatc("%.%", fmt_s(config.paths.todo_db), fmt_s(BU_EXT));
    str mvdbcmd =
        str_formatc("mv % %", fmt_s(config.paths.todo_db), fmt_s(dbBu));

    system(str_cstr(mvdbcmd));
}
