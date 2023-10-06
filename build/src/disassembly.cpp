#include "config.h"
#include <stdio.h>


int main(){
    FILE* file = fopen(INPUT_FILE, "r");

    if ( !file ){
        printf("Error: no input file");
        return -1;
    }


}
