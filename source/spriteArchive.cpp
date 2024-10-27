// Arguments: input path, output file name
//
// The sprite may not contain more than 16 colors. The program automatically combines
// similar colors and will do so very aggressively to enforce this color limit.

#include <cassert>
#include <cstdio>
#include <map>

#include "fsdata.h"
#include "gfxdata.h"

constexpr u8  THRESHOLD  = 0;
constexpr u16 STRING_LEN = 100;

int main( int p_argc, char** p_argv ) {
    if( p_argc < 3 ) {
        printf( "Too few arguments.\n" );
        return 1;
    }

    DEBUG = true;

    auto images = readPictures( p_argv[ 1 ] );

    fs::create_directories( std::string( FSROOT ) );
    fs::create_directories( std::string( OUT ) );
    auto fname = string( FSROOT "/" ) + string( p_argv[ 2 ] ) + ".raw.ar";
    auto hname = string( OUT "/" ) + string( p_argv[ 2 ] ) + ".raw.h";

    FILE* f = fopen( fname.c_str( ), "wb" );
    assert( f );
    FILE* header = fopen( hname.c_str( ), "wb" );
    assert( header );
    fprintf( header, "#pragma once\n" );

    u32  spos = 0;
    char writebuffer[ STRING_LEN + 10 ];

    for( auto [ name, image ] : images ) {
        // write image starting position to header

        fprintf( header, "#define " );

        bool scale = false;
        if( name[ 0 ] == '@' ) { scale = true; }

        std::memset( writebuffer, 0, sizeof( writebuffer ) );
        snprintf( writebuffer, 90, "%s", name.c_str( ) + scale );
        printNormalized( writebuffer, header );
        fprintf( header, "_START %hu\n", spos );

        u16 wid = image.m_width / ( 1 + scale );
        u16 hei = image.m_height / ( 1 + scale );

        // write image
        printf( "%u %u\n", hei, wid );
        printImage( f, string( p_argv[ 2 ] ) + " " + name, image, hei, wid, 1, THRESHOLD );

        spos += ( 16 * sizeof( u16 ) );            // palette data
        spos += ( hei * wid / 8 ) * sizeof( u32 ); // sprite data
    }

    printf( "%s\n", fname.c_str( ) );
    printf( "%s\n", hname.c_str( ) );
    fclose( f );
    fclose( header );

    return 0;
}
