#include "TXLib.h"
#include "processor.h"
#include "config.h"
#include "stack.h"

#include <string.h>
#include <stdio.h>
#include <math.h>
#include <sys/stat.h>


const double PRECISION = 1000;


#ifdef LOGGING
#define ON_LOGGING(...) __VA_ARGS__
#else
#define ON_LOGGING(...)
#endif

#define ADD_JMP_COMMAND( name, func ) \
    if ( command == (COMMANDS::name | I_BIT) ){\
        if ( command != ( COMMANDS::JMP | I_BIT ) ){\
            stackPop( stk, &second  );   \
            stackPop( stk, &first );     \
        }\
                                     \
        uint64_t pos = *( uint64_t* ) ( processor.cs + processor.ip );\
        \
        ON_LOGGING(printf("[%d] "#name" command, result -> ", step));\
        \
        if ( first func second ) {      \
            processor.ip = pos;      \
            ON_LOGGING(printf("jumped to %lld\n", pos));\
        }                            \
        else{                        \
            processor.ip += sizeof( uint64_t );\
            ON_LOGGING(printf("skipped jump to %lld\n", pos));\
        }                            \
    }                                \
    else


elem_t _add_( elem_t first, elem_t second );
elem_t _sub_( elem_t first, elem_t second );
elem_t _div_( elem_t first, elem_t second );
elem_t _mul_( elem_t first, elem_t second );

void push( stack* stk, elem_t* first, elem_t* second, elem_t ( *func ) ( elem_t, elem_t ) );

void SPUCtor( SPU* spu );
void SPUDtor( SPU* spu );

int main(){
    FILE* file = fopen( ASSEMBLY_FILE, "rb" );

    if ( !file ){
        printf( "Error: no input file" );
        return -1;
    }

    struct stat buf = {};

    stat( ASSEMBLY_FILE, &buf );
    uint64_t fileSize = buf.st_size;

    int command   = 0;

    SPU processor = {};
    SPUCtor( &processor );
    stack* stk =            &processor.stk;
    stack* recursionStack = &processor.rs;

    fileSize = fread( processor.cs, sizeof( char ), fileSize, file );
    fclose( file );

    int step = 0;

    elem_t first = 0, second = 0;

    while ( processor.ip < fileSize ){
        // stackDump(stk, 0);
        // littleDump(stk);
        command = *( char* ) ( processor.cs + processor.ip );

        ON_LOGGING(printf( "current step [%d] ( command_ID = %d, command_Address = %lld )\n", step, command, processor.ip ));

        processor.ip++;

        if      ( command == COMMANDS::ADD ){

            push( stk, &first, &second, _add_ );

            ON_LOGGING(printf( "[%d] ADDED %.3lf TO STACK ( %.3lf + %.3lf )\n", step, _add_( first, second ) / PRECISION, first / PRECISION, second / PRECISION ));
        }

        else if ( command == COMMANDS::SUB ){

            push( stk, &first, &second, _sub_ );
            ON_LOGGING(printf( "[%d] ADDED %.3lf TO STACK ( %.3lf - %.3lf )\n", step, _sub_( first, second ) / PRECISION, second / PRECISION, first / PRECISION ));
        }

        else if ( command == COMMANDS::MUL ){

            push( stk, &first, &second, _mul_ );
            ON_LOGGING(printf( "[%d] ADDED %.3lf TO STACK ( %.3lf * %.3lf )\n", step, _mul_( first, second ) / PRECISION, first / PRECISION, second / PRECISION ));
        }

        else if ( command == COMMANDS::DIV ){

            push( stk, &first, &second, _div_ );

            if ( first == 0 ){
                printf( "Error: zero division\n" );
                return -1;
            }
            else{
                ON_LOGGING(printf( "[%d] ADDED %.3lf TO STACK ( %.3lf / %.3lf )\n", step, _div_( first, second ) / PRECISION, second / PRECISION, first / PRECISION ));
            }
        }

        else if ( command == ( COMMANDS::PUSH | I_BIT ) ){

            double elem = *( double* ) ( processor.cs + processor.ip );

            stackPush( stk, ( elem_t ) elem * PRECISION );

            processor.ip += sizeof( double );
            ON_LOGGING(printf( "[%d] PUSHED %lf INTO STACK\n", step, elem ));
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

            ON_LOGGING(printf( "[%d] PUSHED INTO STACK FROM %d REGISTER\n", step, reg ));
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

            ON_LOGGING(printf( "[%d] POPPED FROM STACK INTO %d REGISTER %lld\n", step, reg, value ));
            processor.ip++;
        }

        else if ( command == COMMANDS::SQRT ){

            stackPop( stk, &first );
            double tmp = first / PRECISION;
            stackPush( stk, ( elem_t ) ( sqrt( tmp ) * PRECISION ) );

            ON_LOGGING(printf( "[%d] ADDED %.3lf TO STACK\n", step, sqrt( tmp ) ));
        }

        else if ( command == COMMANDS::SIN ){

            stackPop( stk, &first );
            stackPush( stk, ( elem_t ) ( sin( first ) * PRECISION ) );
            ON_LOGGING(printf( "[%d] ADDED %.3lf TO STACK\n", step, sin( first ) ));
        }

        else if ( command == COMMANDS::COS ){

            stackPop( stk, &first );
            stackPush( stk, ( elem_t ) ( cos( first ) * PRECISION ) );
            ON_LOGGING(printf( "[%d] ADDED %.3lf TO STACK\n", step, cos( first ) ));
        }

        else if ( command == COMMANDS::IN ){
            ON_LOGGING(printf( "USER_INPUT: " ));

            scanf( "%lld", &first );
            stackPush( stk, first * PRECISION );

            ON_LOGGING(printf( "[%d] IN CALL, PUSHED %lld\n", step, first ));
        }

        else if ( command == COMMANDS::OUT ){

            stackPop( stk, &first );
            ON_LOGGING(printf( "---\n[OUT]: "));

            printf("%.3lf\n", first / PRECISION);

            ON_LOGGING(printf("\n---\n" ));
        }

        else if ( command == COMMANDS::HLT ){
            ON_LOGGING(printf( "[%d] HLT, breaking\n", step ));
            break;
        }
        else if ( command == (COMMANDS::CALL | I_BIT) ){

            elem_t address = *(elem_t*) ( processor.cs + processor.ip );
            stackPush( recursionStack, processor.ip );
            processor.ip = address;

        }
        else if ( command == COMMANDS::RET ){
            elem_t address = 0;

            stackPop( recursionStack, &address );

            processor.ip = address + sizeof( uint64_t );
        }
        else
        #include "jmps.h"
        {
            printf( "[%d] UNKNOWN COMMAND %d\n", step, command );
            break;
        }
        step++;
    }
    SPUDtor(&processor);
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
    stackCtor( &spu->rs  );
    spu->cs  = calloc( sizeof( char ),     RAM_SIZE );
    spu->ip  = 0;
    spu->rax = 0;
    spu->rbx = 0;
    spu->rcx = 0;
    spu->rdx = 0;
}

void SPUDtor( SPU* spu ){
    stackDtor( &spu->stk );
    stackDtor( &spu->stk );
    free(spu->cs);
    spu->ip  = -1;
    spu->rax = -1;
    spu->rbx = -1;
    spu->rcx = -1;
    spu->rdx = -1;
}
