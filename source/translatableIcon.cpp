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

    vector<FILE*> fout = vector<FILE*>( );

    for( u8 i = 0; i < NUM_LANGUAGES; ++i ) {
        string nm = string( FSROOT "/" ) + string( p_argv[ 2 ] ) + "." + to_string( i ) + ".rawb";
        FILE*  f  = fopen( nm.c_str( ), "wb" );
        assert( f );
        fout.push_back( f );
    }

    for( const auto& v : images ) {
        for( u8 i = 0; i < NUM_LANGUAGES; ++i ) {
            auto nm = to_string( i );
            printImage( fout[ i ], nm, v[ i ], hg, wd, fr, 0 );
        }
    }

    for( auto f : fout ) { fclose( f ); }

    return 0;
}
