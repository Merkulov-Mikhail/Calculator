#ifndef SPU_DEFINED
    #define SPU_DEFINED

    #include <inttypes.h>
    #include "stack.h"
    #include "config.h"

    struct SPU{
        stack stk;
        stack rs; // recursion stack
        int64_t rax;
        int64_t rbx;
        int64_t rcx;
        int64_t rdx;
        Label*  labels;
        uint64_t ip;
        void* cs;
    };

#endif
