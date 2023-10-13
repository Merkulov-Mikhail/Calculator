#include <inttypes.h>
#include "stack.h"

struct SPU{
    stack* stk;
    int64_t rax;
    int64_t rbx;
    int64_t rcx;
    int64_t rdx;
    int ip;
    void* cs;
};

