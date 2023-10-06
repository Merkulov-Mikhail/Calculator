#include <stdlib.h>



const char* INPUT_FILE    = "input.txt";
const char* ASSEMBLY_FILE = "ass.ass";


enum COMMANDS{
    HLT = -1,

    BASIC_COMMANDS = 1 << 4 ,
    IN ,
    OUT,

    STACK_COMMANDS = 1 << 10,
    PUSH,

    CALC_COMMANDS  = 1 << 12,
    ADD ,
    SUB ,
    MUL ,
    DIV ,
    SQRT,
    SIN ,
    COS ,
};
