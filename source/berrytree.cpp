// Converts a berry tree sprite sheet to a rsd file.
//
// Arguments: input path, output file name
//
// The sprite may not contain more than 16 colors. The program automatically combines
// similar colors and will do so very aggressively to enforce this color limit.

#include <cassert>
#include <cstdio>

#include "gfxdata.h"

constexpr u8 THRESHOLD   = 0;
constexpr u8 FRAMES_X    = 4;
constexpr u8 FRAME_SKIP  = 1;
constexpr u8 FRAME_START = 4;

int main( int p_argc, char** p_argv ) {
    if( p_argc < 3 ) {
        printf( "Too few arguments.\n" );
        return 1;
    }

    u16 hg = 32, wd = 16;

    auto images = readIndexedPictures( p_argv[ 1 ] );

    fs::create_directories( std::string( FSROOT "/" ) + string( p_argv[ 2 ] ) );

    for( const auto& [ i, img ] : images ) {
        auto fname = string( FSROOT "/" ) + string( p_argv[ 2 ] ) + to_string( i ) + ".rsd";
        printf( fname.c_str( ) );
        printf( " %d %d\n", img.m_width, img.m_height );
        fflush( stdout );
        auto f = fopen( fname.c_str( ), "wb" );
        assert( f );

        printImage( f, fname, img, hg, wd, 6, THRESHOLD, true );
        fclose( f );
    }

    return 0;
}
