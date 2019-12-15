#include "utility.h"
#include "debug.h"
#include "string.h"

Debug::Debug(char *flagList)
{
    enableFlags = flagList;
}

bool Debug::IsEnabled(char flag)
{
    if (enableFlags != NULL) {
	return ((strchr(enableFlags, flag) != 0)
		|| (strchr(enableFlags, '+') != 0));
    } else {
    	return FALSE;
    }
}
