//#include "config.h"

//#include <string.h>
//#include <stdio.h>
//#include <math.h>


#define ADD_COMMAND(name, num, nargs, ...)                                                                          \
    if ( !strcmp(command, #name) ){\
        if (!nargs){\
            *(char*) (commandSegment + usedBytes++) = num;\
            printf("[%d]" ##name "COMMAND\n", step);\
        }\
        else{\
            char buf[MAX_LINE_LENGTH] = {};\
            retValue = fscanf(input, "%s", buf);\
\
            if ( retValue <= 0 ){ \
                printf("[%d] ERROR:"##name " with no argument\n", step);\
                break;\
            }\
\
            destroyCommentary( buf );\
\
            res = parseArgument( buf );\
            /*\
                See ARGUMENT_RESPONSE\
                If res is not present in ARGUMENT_RESPONSE, than res contains register address\
            */\
            switch (res){\
\
                case LINE_ERROR:\
                    printf("[%d] ERROR: No argument or line limit exceeded", step);\
                    return;\
\
                case BAD_ARGUMENT:\
                    printf("[%d] ERROR: Incorrect argument (%s)", step, symb);\
                    return;\
\
                case UNKNOWN_DATA:\
                    printf("[%d] ERROR: Unknown data after argument", step);\
                    return;\
\
                case IS_NUMBER:\
                    *(char*)   (commandSegment + usedBytes++) = num | I_BIT;\
                    *(double*) (commandSegment + usedBytes)   = elem;\
                    usedBytes += sizeof(double);\
                    break;\
\
                default:\
                    *(char*) (commandSegment + usedBytes++) = num | R_BIT;\
                    *(char*) (commandSegment + usedBytes++) = res;\
                    break;\
\
            }\
        }\
        printf("[%d]"##name" COMMAND\n", step);\
    }\
    else                                                                                \




void destroyCommentary( char* arr );


int main(){
    FILE* input  = fopen(INPUT_FILE, "r");
    FILE* output = fopen(ASSEMBLY_FILE, "wb");

    if ( !input ){
        printf("Error: no input file");
        return -1;
    }
    if ( !output ){
        printf("Error: could not create final file");
        return -1;
    }

    int size = RAM_SIZE;
    void* commandSegment = calloc(sizeof(char), size);

    int usedBytes = 0;
    int step = 1;
    int retValue = 0;
    char command[10] = {};

    while ( 1 ){

        retValue = fscanf(input, "%s", command);
        if ( retValue <= 0 ){
            printf("ERROR/EOF\n");
            break;
        }
        #include "commands.h"
        /*else*/{
            printf("[%d] UNKNOWN COMMAND %s\n", step, command);
            break;
        }
        step++;
        if ( usedBytes + 12 >= size ){

            commandSegment = realloc( commandSegment, usedBytes );
            fwrite( commandSegment, sizeof(char), usedBytes, output );

            usedBytes = 0;
        }

    }
    fclose(input);


    commandSegment = realloc( commandSegment, usedBytes );
    fwrite( commandSegment, sizeof(char), usedBytes, output );
    fclose(output);
}


void destroyCommentary( char* arr ){
    char* pos = strchr(arr, ';');
    if (pos)
        arr[pos - arr] = '\0';
}

void addCharByte( void* dest, char value ){
    *(char*) dest = value;
}


int parseArgument(const char* buf){
    double elem = 0;

    retValue = sscanf(buf, "%lf", &elem); // if argument is a immediate const

    if ( retValue ){

        if ( sscanf(buf, "%*s %*s") == 2 ) // catching qweasd
            return UNKNOWN_DATA;

        else
            return IS_NUMBER;
    }

    else{

        char symb[3] = {};

        if ( sscanf(buf, "%s", symb) ){

            if ( symb[0] == 'r' && symb[2] == 'x' && symb[1] - 'a' < 4 ){ // push r?x qweasd

                if ( sscanf(buf, "%*s %*s") == 2 ) // catching qweasd
                    return UNKNOWN_DATA;

                else // no qweasd, all Good
                    return symb[1] - 'a';
            }

            else{ // not a valiable register
                return BAD_ARGUMENT;
            }
        }
        else{
            return LINE_ERROR;
        }
    }
}
