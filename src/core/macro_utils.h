#pragma once

#ifndef MACRO_UTILS_H
#define MACRO_UTILS_H
// do function only once macro
#define DO_ONCE(code) \
    do { \
        static int done = 0; \
        if (!done) { \
            done = 1; \
            code; \
        } \
    } while(0)

#endif // MACRO_UTILS_H
