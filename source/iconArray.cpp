// Arguments: input path, output file name
//
// The sprite may not contain more than 16 colors. The program automatically combines
// similar colors and will do so very aggressively to enforce this color limit.

#include <cassert>
#include <cstdio>
#include <map>

#include "gfxdata.h"

constexpr u8 THRESHOLD = 0;

int main( int p_argc, char** p_argv ) {
    if( p_argc < 6 ) {
        printf( "Too few arguments.\n" );
        return 1;
    }

    u16 hg, wd;
    u8  fr;
    sscanf( p_argv[ 3 ], "%hu", &hg );
    sscanf( p_argv[ 4 ], "%hu", &wd );
    sscanf( p_argv[ 5 ], "%hhu", &fr );

    auto images = readTranslatablePictures( p_argv[ 1 ] );

    fs::create_directories( std::string( FSROOT ) );
    auto fname = string( FSROOT "/" ) + string( p_argv[ 2 ] ) + ".icon.rawb";

    FILE* f = fopen( fname.c_str( ), "wb" );
    assert( f );

    size_t mxdim = 0;
    for( size_t i = 0; i < images.size( ); ++i ) { mxdim = max( mxdim, images[ i ].size( ) ); }

    printf( "maxdim %lu\n", mxdim );
    for( size_t i = 0; i < images.size( ); ++i ) {
        for( size_t j = 0; j < mxdim; ++j ) {
            if( j < images[ i ].size( ) ) {
                printImage( f, string( p_argv[ 2 ] ) + " " + to_string( i ) + "-" + to_string( j ),
                            images[ i ][ j ], hg, wd, fr, THRESHOLD );
            } else {
                printImage( f, string( p_argv[ 2 ] ) + " " + to_string( i ) + "-" + to_string( j ),
                            images[ i ][ 0 ], hg, wd, fr, THRESHOLD );
            }
        }
    }

    printf( fname.c_str( ) );
    fclose( f );

    return 0;
}
