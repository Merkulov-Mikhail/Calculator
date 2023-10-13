#include <stdlib.h>



const char* INPUT_FILE       = "input.txt";
const char* ASSEMBLY_FILE    = "ass.ass";
const char* DISASSEMBLY_FILE = "disass.disass";


const int VERSION         = 0;

const short I_BIT         = 1 << 5;
const short R_BIT         = 1 << 6;

const int MAX_LINE_LENGTH = 20;

const int RAM_SIZE = 1024 * 1024;


#undef IN
#undef OUT


enum COMMANDS{
    HLT = 0,

    PUSH,
    POP ,
    IN  ,
    OUT ,
    ADD ,
    SUB ,
    MUL ,
    DIV ,
    SQRT,
    SIN ,
    COS ,
};


enum ARGUMENT_RESPONSE{
    LINE_ERROR   = -4,
    BAD_ARGUMENT = -3,
    UNKNOWN_DATA = -2,
    IS_NUMBER    = -1,

}
