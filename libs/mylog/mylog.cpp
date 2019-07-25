#include "mylog.h"

void errorLog(const char * error)
{
    fprintf(stderr, "%s\n", error);
}
