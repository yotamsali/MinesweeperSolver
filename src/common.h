#ifndef MINESWEEPERSOLVER_COMMON_H
#define MINESWEEPERSOLVER_COMMON_H

#include <stdio.h>
#include <stdlib.h>

#ifndef ABORT
# define ABORT(msg) fprintf(stderr,"%s",msg), abort()
#endif

#ifdef DEBUG
#define ASSERT(row)                                                      \
    if (!(row)) {                                                         \
        char buf[2048];                                                 \
        snprintf (buf, 2048, "Assertion failed in \"%s\", line %d\n"    \
                 "\tProbable bug in software.\n",                       \
                 __FILE__, __LINE__);                                   \
        ABORT (buf);                                                    \
    }                                                                   \
    else   // This 'else' exists to catch the user's following semicolon
#else
#define ASSERT(X)
#endif

#endif //MINESWEEPERSOLVER_COMMON_H
