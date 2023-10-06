#include "config.h"

#include <string.h>
#include <stdio.h>
#include <math.h>


int main(){
    FILE* input  = fopen(INPUT_FILE, "r");
    FILE* output = fopen(ASSEMBLY_FILE, "w");

    if ( !input ){
        printf("Error: no input file");
        return -1;
    }
    if ( !output ){
        printf("Error: could not create final file");
        return -1;
    }

    int retValue  = 0;
    char command[10] = {};
    int step = 1;

    while ( 1 ){

        retValue = fscanf(input, "%s", command);
        if ( retValue <= 0 ){
            printf("ERROR/EOF\n");
            break;
        }

        if      ( !strcmp(command, "add") ){

            fprintf(output, "%d\n", COMMANDS::ADD);
            printf("[%d] ADD COMMAND\n", step);
        }

        else if ( !strcmp(command, "sub") ){

            fprintf(output, "%d\n", COMMANDS::SUB);
            printf("[%d] SUB COMMAND\n", step);
        }

        else if ( !strcmp(command, "mul") ){

            fprintf(output, "%d\n", COMMANDS::MUL);
            printf("[%d] MUL COMMAND\n", step);
        }

        else if ( !strcmp(command, "div") ){

            fprintf(output, "%d\n", COMMANDS::DIV);
            printf("[%d] DIV COMMAND\n", step);
        }

        else if ( !strcmp(command, "push")){

            double elem = 0;
            retValue = fscanf(input, "%lf", &elem);

            if ( retValue <= 0 ){
                printf("ERROR\n");
                break;
            }

            fprintf(output, "%d %lf\n", COMMANDS::PUSH, elem);
            printf("[%d] PUSH COMMAND\n", step);
        }
        else if ( !strcmp(command, "sqrt") ){

            fprintf(output, "%d\n", COMMANDS::SQRT);
            printf("[%d] SQRT COMMAND\n", step);
        }
        else if ( !strcmp(command, "sin")){

            fprintf(output, "%d\n", COMMANDS::SIN);
            printf("[%d] SIN COMMAND\n", step);
        }
        else if ( !strcmp(command, "cos")){

            fprintf(output, "%d\n", COMMANDS::COS);
            printf("[%d] COS COMMAND\n", step);
        }
        else if ( !strcmp(command, "in")){

            fprintf(output, "%d\n", COMMANDS::IN);
            printf("[%d] IN COMMAND\n", step);
        }
        else if ( !strcmp(command, "out")){

            fprintf(output, "%d\n", COMMANDS::OUT);
            printf("[%d] OUT COMMAND\n", step);
        }
        else if ( !strcmp(command, "hlt")){

            fprintf(output, "%d\n", COMMANDS::HLT);
            printf("[%d] HLT COMMAND\n", step);
        }
        else {

            printf("[%d] UNKNOWN COMMAND %s\n", step, command);
            break;
        }
        step++;
    }
    fclose(input);
    fclose(output);
}
