#ifndef SPU_DEFINED
    #define SPU_DEFINED

    #include <inttypes.h>
    #include "stack.h"
    #include "config.h"

    struct SPU{
        stack stk;
        int64_t rax;
        int64_t rbx;
        int64_t rcx;
        int64_t rdx;
        Label*  labels;
        int ip;
        void* cs;
    };

#endif
