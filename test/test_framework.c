#include <stdio.h>
#include <stdarg.h>

int assertTrue(int mustBeTrue, const char *failureMessage, ...)
{	
    if (!mustBeTrue) {
		va_list ap;
		va_start(ap, failureMessage);
        vfprintf(stderr, failureMessage, ap);
		va_end(ap);
        return -1;
    }
    return 0;
}
