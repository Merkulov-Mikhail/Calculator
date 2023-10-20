#include "config.h"

#include <string.h>
#include <stdio.h>
#include <math.h>


#define ADD_COMMAND(name, num, nargs, ...)\
    if ( !strcmp(command, #name) ){\
        comName  = #name;\
        comNum   = num;\
        comNargs = nargs;\
    }\
    else\


static int loadLabels(Label* labels, size_t *size, FILE* input)

void destroyCommentary(   char* arr );
int  parseArgument( const char* buf );


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

    int  usedBytes = 0;
    int  step      = 1;
    int  retValue  = 0;

    char command[MAX_LINE_LENGTH] = {};

    size_t size   = LABEL_COUNT;
    Label* labels = ( Label* ) calloc( sizeof( Label ), LABEL_COUNT );

    int res = loadLabels(labels, &size, input);

    if ( res ){
        return -1;
    }

    while ( 1 ){

        retValue = fscanf(input, "%s", command);
        const char* comName = nullptr;
        int comNum = -1, comNargs = 0;

        if ( retValue == 0 ){
            printf("[%d] ERROR\n", step);
            break;
        }
        else if ( retValue == -1 ){
            printf("[%d] EOF\n",   step);
            break;
        }

        #include "commands.h"
        /*else*/{
            printf("[%d] UNKNOWN COMMAND %s\n", step, command);
            break;
        }
        if (!comNargs){
            *(char*) (commandSegment + usedBytes++) = comNum;
        }
        else {

            char buf[MAX_LINE_LENGTH] = {};
            retValue = fscanf(input, "%s", buf);

            if ( retValue <= 0 ){
                printf("[%d] ERROR: %s with no argument\n", step, comName);
                break;
            }

            destroyCommentary( buf );

            int doBreak = 0;

            if ( JMP <= comNum && comNum <= JE  ) {

                double value = 0;

                int res = sscanf( buf, "%d %s", value );

                *(char*) (commandSegment + usedBytes++) = comNum;

                if ( res == 0 ){ // label is not an clean address, it's a name

                    char labelName[MAX_LINE_LENGTH] = {};
                    res = sscanf( buf, "%s %s", labelName );

                    switch ( res ){ // qweasd found
                        case 2:
                            printf("[%d] ERROR: Unknown data after argument\n", step);
                            doBreak = 1;
                            break;
                        case 0:
                            printf("[%d] ERROR: No argument or line limit exceeded\n", step);
                            doBreak = 1;
                            break;
                        case 1:
                            uint64_t address = getFromLabelAddress()
                            usedBytes += sizeof( double );

                            continue;
                    }
                }
                else if (res == 1){
                    *(char*) (commandSegment + usedBytes) = value;
                    usedBytes += sizeof(double);
                }
                else{
                    printf("[%d] ERROR: Unknown data after argument\n", step);
                    doBreak = 1;
                }
            }
            else{


                int res = parseArgument( buf );
                /*
                    See ARGUMENT_RESPONSE
                    If res is not present in ARGUMENT_RESPONSE, than res contains register address
                */
                switch (res){

                    case LINE_ERROR:
                        printf("[%d] ERROR: No argument or line limit exceeded", step);
                        doBreak = 1;
                        break;

                    case BAD_ARGUMENT:
                        printf("[%d] ERROR: Incorrect argument", step);
                        doBreak = 1;
                        break;

                    case UNKNOWN_DATA:
                        printf("[%d] ERROR: Unknown data after argument", step);
                        doBreak = 1;
                        break;

                    case IS_NUMBER:
                        *(char*)   (commandSegment + usedBytes++) = comNum | I_BIT;
                        sscanf(buf, "%lf", (double*) (commandSegment + usedBytes));
                        usedBytes += sizeof(double);
                        break;

                    default:
                        *(char*) (commandSegment + usedBytes++) = comNum | R_BIT;
                        *(char*) (commandSegment + usedBytes++) = res;
                        break;

                }
            }
            if (doBreak)
                break;
        }
        #ifdef LOGGING
            printf("[%d] %s COMMAND\n", step, comName);
        #endif
        step++;
        if ( usedBytes + 12 >= size ){

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

    int retValue = sscanf(buf, "%lf", &elem); // if argument is a immediate const

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


static int loadLabels(Label* labels, size_t *size, FILE* input){

    char command[MAX_LINE_LENGTH] = {};

    uint64_t retValue = 0;
    int labelCounter = 0;

    while ( 1 ){
        retValue = fgets(command, MAX_LINE_LENGTH, input);

        if ( command[0] == ':' ){

            labels[labelCounter].pi   = usedBytes;

            labels[labelCounter].name = ( char* ) calloc( sizeof( char ), strlen(command) );
            strncpy(labels[labelCounter].name, command + 1, strlen(command));

            labelCounter++;
            if ( labelCounter == *size ){

                *size *= 2;
                void* ptr = realloc(labels, *size);

                if (ptr){

                    labels = ptr;
                    continue;

                }

                printf("ERROR: too much labels");
                return -1;

            }

            continue;
        }

        if (!retValue)
            return 0;
    }
}
