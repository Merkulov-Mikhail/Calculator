#ifndef STACK_H

    #define STACK_H
    #define DEBUG
    #define DUMP


    #include <cstddef>
    #include <cstdint>
    #include <stdlib.h>

    #include "hash.h"

    const int MULTYPLIER = 2;
    const int START_CAPACITY = 2;

    typedef int64_t elem_t;
    typedef uint64_t egypt_t;
    typedef uint64_t stackSize_t;

    struct stack{
        egypt_t     slave;
        void*       data;
        stackSize_t capacity;
        stackSize_t size;
        egypt_t     pharaon;
        hash_t      structHash;
        hash_t      dataHash;
    };

    enum STACK_ERRORS{
        OK                  = 0,
        STACK_NULL          = 1 << 0,
        STACK_DATA_NULL     = 1 << 1,
        STACK_OUT_OF_BOUNDS = 1 << 2,
        STACK_UNDERFLOW     = 1 << 3,
        STACK_OUT_OF_MEMORY = 1 << 4,
        EGYPT_SYSTEM_DOWN   = 1 << 5,
        HASH_DATA_ERROR     = 1 << 6,
        HASH_STRUCT_ERROR   = 1 << 7,
    };


    void stackCtor( stack* stk );
    void stackDtor( stack* stk );

    uint64_t stackPush( stack* stk, elem_t  value );
    uint64_t stackPop ( stack* stk, elem_t* retValue=nullptr );


#endif
