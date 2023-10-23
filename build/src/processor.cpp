#include "processor.h"
#include "config.h"
#include "stack.h"

#include <string.h>
#include <stdio.h>
#include <math.h>
#include <sys/stat.h>

const double PRECISION = 1000;


#define ADD_JMP_COMMAND( name, func ) \
    if ( command == (COMMANDS::name | I_BIT) ){\
        stackPop( stk, &second  );   \
        stackPop( stk, &first );     \
                                     \
        if ( first func second ) {      \
            uint64_t pos = *( uint64_t* ) ( processor.cs + processor.ip );\
            processor.ip = pos;      \
        }                            \
        else{                        \
            processor.ip += sizeof( uint64_t );\
        }                            \
    }                                \
    else


elem_t _add_( elem_t first, elem_t second );
elem_t _sub_( elem_t first, elem_t second );
elem_t _div_( elem_t first, elem_t second );
elem_t _mul_( elem_t first, elem_t second );

void push( stack* stk, elem_t* first, elem_t* second, elem_t ( *func ) ( elem_t, elem_t ) );
void SPUCtor( SPU* spu );

int main(){
    FILE* file = fopen( ASSEMBLY_FILE, "rb" );

    if ( !file ){
        printf( "Error: no input file" );
        return -1;
    }

    struct stat buf = {};

    stat( ASSEMBLY_FILE, &buf );
    int fileSize = buf.st_size;

    int command   = 0;

    SPU processor = {};
    SPUCtor( &processor );
    stack* stk = &processor.stk;

    fileSize = fread( processor.cs, sizeof( char ), fileSize, file );
    int step = 0;
    elem_t first = 0, second = 0;
    while ( processor.ip < fileSize ){

        command = *( char* ) ( processor.cs + processor.ip );
        processor.ip++;

        if      ( command == COMMANDS::ADD ){

            push( stk, &first, &second, _add_ );

            #ifdef LOGGING
                printf( "[%d] ADDED %.3lf TO STACK ( %.3lf + %.3lf )\n", step, _add_( first, second ) / PRECISION, first / PRECISION, second / PRECISION );
            #endif
        }

        else if ( command == COMMANDS::SUB ){

            push( stk, &first, &second, _sub_ );
            #ifdef LOGGING
                printf( "[%d] ADDED %.3lf TO STACK ( %.3lf - %.3lf )\n", step, _sub_( first, second ) / PRECISION, second / PRECISION, first / PRECISION );
            #endif
        }

        else if ( command == COMMANDS::MUL ){

            push( stk, &first, &second, _mul_ );
            #ifdef LOGGING
                printf( "[%d] ADDED %.3lf TO STACK ( %.3lf * %.3lf )\n", step, _mul_( first, second ) / PRECISION, first / PRECISION, second / PRECISION );
            #endif
        }

        else if ( command == COMMANDS::DIV ){

            push( stk, &first, &second, _div_ );

            if ( first == 0 ){
                printf( "Error: zero division\n" );
                return -1;
            }
            else{
                #ifdef LOGGING
                    printf( "[%d] ADDED %.3lf TO STACK ( %.3lf / %.3lf )\n", step, _div_( first, second ) / PRECISION, second / PRECISION, first / PRECISION );
                #endif
            }
        }

        else if ( command == ( COMMANDS::PUSH | I_BIT ) ){

            double elem = *( double* ) ( processor.cs + processor.ip );

            stackPush( stk, ( elem_t ) elem * PRECISION );

            processor.ip += sizeof( double );
            #ifdef LOGGING
                printf( "[%d] PUSHED %lf INTO STACK\n", step, elem );
            #endif
        }

        else if ( command == ( COMMANDS::PUSH | R_BIT ) ){

            char reg = *( char* ) ( processor.cs + processor.ip );
            bool res = 0;

            switch ( reg ){
                case 0:
                        stackPush( stk, ( elem_t ) processor.rax );
                        break;
                case 1:
                        stackPush( stk, ( elem_t ) processor.rbx );
                        break;
                case 2:
                        stackPush( stk, ( elem_t ) processor.rcx );
                        break;
                case 3:
                        stackPush( stk, ( elem_t ) processor.rdx );
                        break;
                default:
                        printf( "[%d] BAD REGISTER\n", step );
                        res = 1;
            }
            if ( res )
                break;

            processor.ip++;

            #ifdef LOGGING
                printf( "[%d] PUSHED INTO STACK FROM %d REGISTER\n", step, reg );
            #endif
        }
        else if ( command == ( COMMANDS::POP | R_BIT ) ){

            char reg = *( char* ) ( processor.cs + processor.ip );
            elem_t value = 0;
            bool res = 0;

            switch ( reg ){
                case 0:
                        stackPop( stk, &value );
                        processor.rax = value;
                        break;
                case 1:
                        stackPop( stk, &value );
                        processor.rbx = value;
                        break;
                case 2:
                        stackPop( stk, &value );
                        processor.rcx = value;
                        break;
                case 3:
                        stackPop( stk, &value );
                        processor.rdx = value;
                        break;
                default:
                        printf( "[%d] BAD REGISTER\n", step );
                        res = 1;
            }
            if ( res )
                break;

            #ifdef LOGGING
                printf( "[%d] PUSHED INTO STACK FROM %d REGISTER\n", step, reg );
            #endif
            processor.ip++;
        } else
        #include "jmps.h"
        if ( command == COMMANDS::SQRT ){

            stackPop( stk, &first );
            double tmp = first / PRECISION;
            stackPush( stk, ( elem_t ) ( sqrt( tmp ) * PRECISION ) );

            #ifdef LOGGING
                printf( "[%d] ADDED %.3lf TO STACK\n", step, sqrt( tmp ) );
            #endif
        }

        else if ( command == COMMANDS::SIN ){

            stackPop( stk, &first );
            stackPush( stk, ( elem_t ) ( sin( first ) * PRECISION ) );
            #ifdef LOGGING
                printf( "[%d] ADDED %.3lf TO STACK\n", step, sin( first ) );
            #endif
        }

        else if ( command == COMMANDS::COS ){

            stackPop( stk, &first );
            stackPush( stk, ( elem_t ) ( cos( first ) * PRECISION ) );
            #ifdef LOGGING
                printf( "[%d] ADDED %.3lf TO STACK\n", step, cos( first ) );
            #endif
        }

        else if ( command == COMMANDS::IN ){
            #ifdef LOGGING
                printf( "USER_INPUT: " );
            #endif

            scanf( "%lld", &first );
            stackPush( stk, first * PRECISION );

            #ifdef LOGGING
                printf( "[%d] IN CALL, PUSHED %lf\n", step, first );
            #endif
        }

        else if ( command == COMMANDS::OUT ){

            stackPop( stk, &first );
            #ifdef LOGGING
                printf( "---\n[OUT]: ");
            #endif

            printf("%.3lf\n", first / PRECISION);

            #ifdef LOGGING
                printf("\n---\n" );
            #endif
        }

        else if ( command == COMMANDS::HLT ){
            #ifdef LOGGING
                printf( "[%d] HLT, breaking\n", step );
                break;
            #endif
        }

        else {
            printf( "[%d] UNKNOWN COMMAND %d\n", step, command );
        }
        step++;
        #ifdef LOGGING
            printf( "current step [%d] ( %d )\n", step, command );
        #endif
    }
    fclose( file );
}


void push( stack* stk, elem_t* first, elem_t* second, elem_t ( *func ) ( elem_t, elem_t ) ){
    stackPop( stk, first );
    stackPop( stk, second );
    stackPush( stk, func( *first, *second ) );
}


elem_t _add_( elem_t first, elem_t second ){
    return first + second;
}

elem_t _sub_( elem_t first, elem_t second ){
    return second - first;
}

elem_t _mul_( elem_t first, elem_t second ){
    return first * second / PRECISION;
}

elem_t _div_( elem_t first, elem_t second ){

    if ( first == 0 )
        return second;

    double tmp = second / ( double ) first;
    return ( elem_t ) ( tmp * PRECISION );
}

void SPUCtor( SPU* spu ){
    stackCtor( &spu->stk );
    spu->cs  = calloc( sizeof( char ), RAM_SIZE );
    spu->ip  = 0;
    spu->rax = 0;
    spu->rbx = 0;
    spu->rcx = 0;
    spu->rdx = 0;
}
