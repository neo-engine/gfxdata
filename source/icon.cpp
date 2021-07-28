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

    auto images = readNumberedPictures( p_argv[ 1 ] );

    fs::create_directories( std::string( FSROOT ) );
    auto fname = string( FSROOT "/" ) + string( p_argv[ 2 ] ) + ".icon.rawb";

    FILE* f = fopen( fname.c_str( ), "wb" );
    assert( f );

    for( size_t i = 0; i < images.size( ); ++i ) {
        printImage( f, string( p_argv[ 2 ] ) + " " + to_string( i ), images[ i ], hg, wd, fr,
                    THRESHOLD );
    }

    printf( fname.c_str( ) );
    fclose( f );
    return 0;
}
