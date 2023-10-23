#include "config.h"

#include <string.h>
#include <stdio.h>
#include <math.h>


#define ADD_COMMAND( name, num, nargs, ... )\
    if ( !strcmp( command, #name ) ){\
        comName  = #name;\
        comNum   = num;\
        comNargs = nargs;\
    }\
    else\



void destroyCommentary(   char* arr );
int  parseArgument( const char* buf );


struct LabelPoints{
    uint64_t     pc;
    char* labelName;
};


int main(){
    FILE* input  = fopen( INPUT_FILE, "r" );
    FILE* output = fopen( ASSEMBLY_FILE, "wb" );

    if ( !input ){
        printf( "Error: no input file" );
        return -1;
    }
    if ( !output ){
        printf( "Error: could not create final file" );
        return -1;
    }

    int size = RAM_SIZE;
    void* commandSegment = calloc( sizeof( char ), size );

    // Shows how many bytes have been written in current page
    int usedBytes       = 0;
    // Shows how many bytes have been written in total
    // It is used to write named labels
    int globalUsedBytes = 0;

    int step            = 1;
    int retValue        = 0;

    char command[MAX_LINE_LENGTH] = {};

    Label* labels =            ( Label* )       calloc( sizeof( Label ), LABEL_COUNT );
    int labelCounter = 0;

    LabelPoints* labelPoints = ( LabelPoints* ) calloc( sizeof( labelPoints ), LABEL_COUNT );
    int labelPointsCounter = 0;

    while ( 1 ){

        retValue = fscanf( input, "%s", command );

        const char* comName = nullptr;
        int comNum = -1, comNargs = 0;

        if ( retValue == 0 ){
            printf( "[%d] ERROR\n", step );
            break;
        }
        else if ( retValue == -1 ){
            printf( "[%d] EOF\n",   step );
            break;
        }

        if ( command[0] == ':' ){

            labels[labelCounter].pi   = usedBytes;

            labels[labelCounter].labelName = ( char* ) calloc( sizeof( char ), strlen( command ) );
            strncpy( labels[labelCounter].labelName, command + 1, strlen( command ) );

            labelCounter++;

            continue;
        }
        #include "commands.h"
        /*else*/{
            printf( "[%d] UNKNOWN COMMAND %s\n", step, command );
            break;
        }
        if ( !comNargs ){
            *( char* ) ( commandSegment + usedBytes++ ) = comNum;
        }
        else {

            char buf[MAX_LINE_LENGTH] = {};
            retValue = fscanf( input, "%s", buf );

            if ( retValue <= 0 ){
                printf( "[%d] ERROR: %s with no argument\n", step, comName );
                break;
            }

            destroyCommentary( buf );

            int doBreak = 0;

            if ( JMP <= comNum && comNum <= JE  ) {

                double value = 0;

                int res = sscanf( buf, "%d %s", value );
                *( char* ) ( commandSegment + usedBytes++ ) = comNum | I_BIT;

                if ( res == 0 ){ // label is not an clean address, it's a name

                    char labelName[MAX_LINE_LENGTH] = {};
                    res = sscanf( buf, "%s %s", labelName );

                    switch ( res ){ // qweasd found
                        case 2:
                            printf( "[%d] ERROR: Unknown data after argument\n", step );
                            doBreak = 1;
                            break;
                        case 0:
                            printf( "[%d] ERROR: No argument or line limit exceeded\n", step );
                            doBreak = 1;
                            break;
                        case 1:

                            labelPoints[labelPointsCounter].pc = usedBytes;
                            labelPoints[labelPointsCounter].labelName = ( char* ) calloc( sizeof( char ), strlen( labelName ) );

                            strncpy( labelPoints[labelPointsCounter].labelName, labelName, strlen( labelName ) );

                            labelPointsCounter++;
                            usedBytes += sizeof( double );

                    }
                }
                else if ( res == 1 ){
                    *( char* ) ( commandSegment + usedBytes ) = value;
                    usedBytes += sizeof( double );
                }
                else{
                    printf( "[%d] ERROR: Unknown data after argument\n", step );
                    doBreak = 1;
                }
            }
            else{


                int res = parseArgument( buf );
                /*
                    See ARGUMENT_RESPONSE
                    If res is not present in ARGUMENT_RESPONSE, than res contains register address
                */
                switch ( res ){

                    case LINE_ERROR:
                        printf( "[%d] ERROR: No argument or line limit exceeded", step );
                        doBreak = 1;
                        break;

                    case BAD_ARGUMENT:
                        printf( "[%d] ERROR: Incorrect argument", step );
                        doBreak = 1;
                        break;

                    case UNKNOWN_DATA:
                        printf( "[%d] ERROR: Unknown data after argument", step );
                        doBreak = 1;
                        break;

                    case IS_NUMBER:
                        *( char* )   ( commandSegment + usedBytes++ ) = comNum | I_BIT;
                        sscanf( buf, "%lf", ( double* ) ( commandSegment + usedBytes ) );
                        usedBytes += sizeof( double );
                        break;

                    default:
                        *( char* ) ( commandSegment + usedBytes++ ) = comNum | R_BIT;
                        *( char* ) ( commandSegment + usedBytes++ ) = res;
                        break;

                }
            }
            if ( doBreak )
                break;
        }
        #ifdef LOGGING
            printf( "[%d] %s COMMAND\n", step, comName );
        #endif
        step++;
        if ( usedBytes + 12 >= size ){

            fwrite( commandSegment, sizeof( char ), usedBytes, output );

            usedBytes = 0;
        }

    }
    fclose( input );

    fwrite( commandSegment, sizeof( char ), usedBytes, output );
    for (     int pointsPos = 0; pointsPos < labelPointsCounter; pointsPos++ ) {
        int foundLabel = 0;

        for ( int labelsPos = 0; labelsPos < labelCounter;       labelsPos++ ) {

            if ( !strcmp( labels[labelsPos].labelName, labelPoints[pointsPos].labelName ) ){

                fseek( output, labelPoints[pointsPos].pc, SEEK_SET );
                fwrite( &( labels[labelsPos].pi ), sizeof( uint64_t ), 1, output );

                foundLabel = 1;
                break;
            }
        }
        if ( !foundLabel ){
            printf( "ERROR: unknown label ( %s )\n", labelPoints[pointsPos].labelName );
            break;
        }
    }

    fclose( output );

    //---------------------------------------------------
    for ( int pos = 0; pos < labelPointsCounter; pos++ ){
        free( labelPoints[pos].labelName );
    }

    free( labelPoints );

    for ( int pos = 0; pos < labelCounter; pos++ ){
        free( labels[pos].labelName );
    }

    free( labels );
    free( commandSegment );
    //---------------------------------------------------
}


void destroyCommentary( char* arr ){
    char* pos = strchr( arr, ';' );
    if ( pos )
        arr[pos - arr] = '\0';
}

void addCharByte( void* dest, char value ){
    *( char* ) dest = value;
}


int parseArgument( const char* buf ){
    double elem = 0;

    int retValue = sscanf( buf, "%lf", &elem ); // if argument is a immediate const

    if ( retValue ){

        if ( sscanf( buf, "%*s %*s" ) == 2 ) // catching qweasd
            return UNKNOWN_DATA;
        else
            return IS_NUMBER;
    }

    else{

        char symb[3] = {};

        if ( sscanf( buf, "%s", symb ) ){

            if ( symb[0] == 'r' && symb[2] == 'x' && symb[1] - 'a' < 4 ){ // push r?x qweasd

                if ( sscanf( buf, "%*s %*s" ) == 2 ) // catching qweasd
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

