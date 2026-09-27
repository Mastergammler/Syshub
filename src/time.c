#include "internal.h"

time_t time_utc_now()
{
    return time(NULL);
}

bool time_older_than_d(time_t compareTime, int days)
{
    long ageSeconds = days * 24 * 60 * 60;
    return time_utc_now() - compareTime > ageSeconds;
}
