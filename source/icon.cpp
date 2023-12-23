// Converts a pkmn sprite to a rsd file.
//
// Arguments: input path, output file name
//
// The sprite may not contain more than 16 colors. The program automatically combines
// similar colors and will do so very aggressively to enforce this color limit.

#include <cassert>
#include <cstdio>

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

    u8 pal_compress = 1; // # of sprites that share a palette
    if( p_argc >= 7 ) { sscanf( p_argv[ 6 ], "%hhu", &pal_compress ); }

    auto images = readNumberedPictures( p_argv[ 1 ] );

    fs::create_directories( std::string( FSROOT ) );
    auto fname = string( FSROOT "/" ) + string( p_argv[ 2 ] ) + ".icon.rawb";

    FILE* f = fopen( fname.c_str( ), "wb" );
    assert( f );

    u16 pal[ 16 ] = { 0 };
    u8  colsUsed  = 0;

    for( size_t i = 0; i < images.size( ); ++i ) {
        if( pal_compress <= 1 || i % pal_compress == 0 ) {
            memset( pal, 0, sizeof( pal ) );
            colsUsed = 0;
        }
        printImage( pal, colsUsed, f, string( p_argv[ 2 ] ) + " " + to_string( i ), images[ i ], hg,
                    wd, fr, THRESHOLD );
    }

    printf( fname.c_str( ) );
    fclose( f );
    return 0;
}
