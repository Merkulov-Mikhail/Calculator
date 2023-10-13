#include "config.h"

#include <string.h>
#include <stdio.h>
#include <math.h>


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

        if      ( !strcmp(command, "add") ){
            *(char*) (commandSegment + usedBytes++) = COMMANDS::ADD;

            printf("[%d] ADD COMMAND\n", step);
        }

        else if ( !strcmp(command, "sub") ){

            *(char*) (commandSegment + usedBytes++) = COMMANDS::SUB;
            printf("[%d] SUB COMMAND\n", step);
        }

        else if ( !strcmp(command, "mul") ){

            *(char*) (commandSegment + usedBytes++) = COMMANDS::MUL;
            printf("[%d] MUL COMMAND\n", step);
        }

        else if ( !strcmp(command, "div") ){

            *(char*) (commandSegment + usedBytes++) = COMMANDS::DIV;
            printf("[%d] DIV COMMAND\n", step);
        }

        else if ( !strcmp(command, "push")){

            char buf[MAX_LINE_LENGTH] = {};
            retValue = fscanf(input, "%s", buf);

            if ( retValue <= 0 ){ // no argument -> wrong push
                printf("[%d] ERROR: push with no argument\n", step);
                break;
            }

            destroyCommentary( buf );

            double elem = 0;

            retValue = sscanf(buf, "%lf", &elem); // if argument is a immediate const

            if ( retValue ){

                if ( sscanf(buf, "%*s %*s") == 2 ){ // catching qweasd

                    printf("[%d] ERROR: Unknown data after argument", step);
                    break;
                }
                else{
                    *(char*)   (commandSegment + usedBytes++) = COMMANDS::PUSH | I_BIT;
                    *(double*) (commandSegment + usedBytes)   = elem;
                    usedBytes += sizeof(double);
                }
            }

            else{

                char symb[3] = {};

                if ( sscanf(buf, "%s", symb) ){

                    if ( symb[0] == 'r' && symb[2] == 'x' && symb[1] - 'a' < 4 ){ // push r?x qweasd

                        if ( sscanf(buf, "%*s %*s") == 2 ){ // catching qweasd

                            printf("[%d] ERROR: Unknown data after argument", step);
                            break;

                        }
                        else{ // no qweasd, all Good
                            *(char*) (commandSegment + usedBytes++) = COMMANDS::PUSH | R_BIT;
                            *(char*) (commandSegment + usedBytes++) = symb[1] - 'a';
                        }
                    }

                    else{ // not a valiable register

                        printf("[%d] ERROR: Incorrect argument (%s)", step, symb);
                        break;
                    }
                }
                else{

                    printf("[%d] ERROR: No argument or line limit exceeded", step);
                    break;
                }
            }
            printf("[%d] PUSH COMMAND\n", step);

        }

        else if ( !strcmp( command, "pop" ) ){
            char buf[MAX_LINE_LENGTH] = {};
            retValue = fscanf(input, "%s", buf);

            if ( retValue <= 0 ){ // no argument -> wrong push
                printf("[%d] ERROR: pop with no argument\n", step);
                break;
            }

            destroyCommentary( buf );
            char symb[3] = {};

            if ( sscanf(buf, "%s", symb) ){

                if ( symb[0] == 'r' && symb[2] == 'x' && symb[1] - 'a' < 4 ){ // pop r?x qweasd

                    if ( sscanf(buf, "%*s %*s") == 2 ){ // catching qweasd

                        printf("[%d] ERROR: Unknown data after argument", step);
                        break;

                    }
                    else{ // no qweasd, all Good
                        *(char*) (commandSegment + usedBytes++) = COMMANDS::POP | R_BIT;
                        *(char*) (commandSegment + usedBytes++) = symb[1] - 'a';
                    }
                }

                else{ // not a valiable register

                    printf("[%d] ERROR: Incorrect argument (%s)", step, symb);
                    break;
                }
            }
            else{

                printf("[%d] ERROR: No argument or line limit exceeded", step);
                break;
            }

            printf("[%d] POP COMMAND\n", step);

        }

        else if ( !strcmp(command, "sqrt") ){
            *(char*) (commandSegment + usedBytes++) = COMMANDS::SQRT;
            printf("[%d] SQRT COMMAND\n", step);
        }
        else if ( !strcmp(command, "sin")){

            *(char*) (commandSegment + usedBytes++) = COMMANDS::SIN;
            printf("[%d] SIN COMMAND\n", step);
        }
        else if ( !strcmp(command, "cos")){

            *(char*) (commandSegment + usedBytes++) = COMMANDS::COS;
            printf("[%d] COS COMMAND\n", step);
        }
        else if ( !strcmp(command, "in")){

            *(char*) (commandSegment + usedBytes++) = COMMANDS::IN;
            printf("[%d] IN COMMAND\n", step);
        }
        else if ( !strcmp(command, "out")){

            *(char*) (commandSegment + usedBytes++) = COMMANDS::OUT;
            printf("[%d] OUT COMMAND\n", step);
        }
        else if ( !strcmp(command, "hlt")){

            *(char*) (commandSegment + usedBytes++) = COMMANDS::HLT;
            printf("[%d] HLT COMMAND\n", step);
        }
        else {

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
