#include "config.h"
#include "stack.h"

#include <string.h>
#include <stdio.h>
#include <math.h>


const double PRECISION = 1000;

elem_t __add__(elem_t first, elem_t second);
elem_t __sub__(elem_t first, elem_t second);
elem_t __div__(elem_t first, elem_t second);
elem_t __mul__(elem_t first, elem_t second);
void push(stack* stk, elem_t* first, elem_t* second, elem_t (*func) (elem_t, elem_t) );

int main(){
    FILE* file = fopen(INPUT_FILE, "r");

    if ( !file ){
        printf("Error: no input file");
        return -1;
    }

    int retValue  = 0;
    char command[10] = {};

    stack stk;
    stackCtor(&stk);

    int step = 0;

    elem_t first = 0, second = 0;

    while ( 1 ){

        retValue = fscanf(file, "%s", command);
        if ( retValue <= 0 ){
            printf("ERROR");
            break;
        }
        step += 1;

        if      ( !strcmp(command, "add") ){

            push(&stk, &first, &second, __mul__);

            printf("[%d] ADDED %.3lf TO STACK (%.3lf + %.3lf)", step, __add__(first, second) / PRECISION, first / PRECISION, second / PRECISION);
        }

        else if ( !strcmp(command, "sub") ){

            push(&stk, &first, &second, __mul__);
            printf("[%d] ADDED %.3lf TO STACK (%.3lf - %.3lf)", step, __sub__(first, second) / PRECISION, second / PRECISION, first / PRECISION);
        }

        else if ( !strcmp(command, "mul") ){

            push(&stk, &first, &second, __mul__);
            printf("[%d] ADDED %.3lf TO STACK (%.3lf * %.3lf)", step, __mul__(first, second) / PRECISION, first / PRECISION, second / PRECISION);
        }

        else if ( !strcmp(command, "div") ){

            push(&stk, &first, &second, __div__);

            if (first == 0){
                printf("Error: zero division\n");
                return -1;
            }
            else{
                printf("[%d] ADDED %.3lf TO STACK (%.3lf / %.3lf)", step, __div__(first, second) / PRECISION, second / PRECISION, first / PRECISION);
            }
        }
        else if ( !strcmp(command, "push")){

            double elem = 0;
            retValue = fscanf(file, "%lf", &elem);

            if ( retValue <= 0 ){
                printf("ERROR");
                break;
            }

            stackPush(&stk, (elem_t) elem * PRECISION);
        }
        else if ( !strcmp(command, "sqrt") ){

            stackPop(&stk, &first);
            double tmp = first / PRECISION;
            stackPush(&stk, (elem_t) (sqrt(tmp) * PRECISION));
        }
        else if ( !strcmp(command, "sin")){

            stackPop(&stk, &first);
            stackPush(&stk, (elem_t) (sin(first) * PRECISION));
        }
        else if ( !strcmp(command, "cos")){

            stackPop(&stk, &first);
            stackPush(&stk, (elem_t) (cos(first) * PRECISION));
        }
        else if ( !strcmp(command, "in")){
            scanf("%lld", &first);
            stackPush(&stk, first * PRECISION);
        }
        else if ( !strcmp(command, "out")){
            stackPop(&stk, &first);
            printf("%lf", first / PRECISION);
        }
        else if ( !strcmp(command, "hlt")){
            printf("[%d] HLT, breaking", step);
            break;
        }
        else {
            printf("[%d] UNKNOWN COMMAND %s", step, command);
        }

    }
    fclose(file);
}


void push(stack* stk, elem_t* first, elem_t* second, elem_t (*func) (elem_t, elem_t) ){
    stackPop(stk, first);
    stackPop(stk, second);
    stackPush(stk, func(*first, *second));
}


elem_t __add__(elem_t first, elem_t second){
    return first + second;
}

elem_t __sub__(elem_t first, elem_t second){
    return second - first;
}

elem_t __mul__(elem_t first, elem_t second){
    return first * second / PRECISION;
}

elem_t __div__(elem_t first, elem_t second){

    if ( first == 0 )
        return second;

    double tmp = second / (double) first;
    return (elem_t) (tmp * PRECISION);
}
