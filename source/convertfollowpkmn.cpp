// Converts a 4x4 follow pkmn sprite sheet to a 8x1 pkmn sprite sheet
// Arguments in.png
//
// Outputs the file where it came from
//
// The sprite may not contain more than 16 colors. The program automatically combines
// similar colors and will do so very aggressively to enforce this color limit.

%:include <cstdio>
%:include <vector>
%:include <string>
%:include <map>
%:include <tuple>
%:include <filesystem>

%:include <png.h>
%:include "bitmap.h"
%:define conv( a ) ( (u8) ( (a) *31 / 255 ) )
%:define revconv( a ) ( ( (a) *255 / 31 ) )

%:define green( a ) ( revconv( ( ( a ) >> 10 ) & 31 ) )
%:define blue( a ) ( revconv( ( ( a ) >> 5 ) & 31 ) )
%:define red( a ) ( revconv( (a) &31 ) )

using namespace std;

u8             image_data[ 64 * 64 * 20 / 4 + 100 ] = { 0 };
unsigned short pal[ 300 ]                           = { 0 };

typedef tuple<u8, u8, u8> t3;
map<unsigned short, u8>   palidx;

void copy_part( const bitmap& p_in, bitmap& p_out, u16 p_sx, u16 p_sy, u16 p_tx, u16 p_ty,
                u16 p_width, u16 p_height ) {
    for( u16 x = 0; x < p_width; ++x ) {
        for( u16 y = 0; y < p_width; ++y ) {
            p_out( p_tx + x, p_ty + y ) = p_in( p_sx + x, p_sy + y );
        }
    }
}

int main( int p_argc, char** p_argv ) {
    if( p_argc < 3 ) {
        printf( "Too few arguments.\n" );
        return 1;
    }

    bitmap in( p_argv[ 1 ] );

    u8 HEIGHT = in.m_height / 4;
    u8 WIDTH  = in.m_width / 4;

    bitmap out( 8 * WIDTH, HEIGHT );

    copy_part( in, out, 1 * WIDTH, 0 * HEIGHT, 0 * WIDTH, 0, WIDTH, HEIGHT );
    copy_part( in, out, 3 * WIDTH, 0 * HEIGHT, 1 * WIDTH, 0, WIDTH, HEIGHT );
    copy_part( in, out, 1 * WIDTH, 3 * HEIGHT, 2 * WIDTH, 0, WIDTH, HEIGHT );
    copy_part( in, out, 3 * WIDTH, 3 * HEIGHT, 3 * WIDTH, 0, WIDTH, HEIGHT );
    copy_part( in, out, 1 * WIDTH, 1 * HEIGHT, 4 * WIDTH, 0, WIDTH, HEIGHT );
    copy_part( in, out, 3 * WIDTH, 1 * HEIGHT, 5 * WIDTH, 0, WIDTH, HEIGHT );
    copy_part( in, out, 1 * WIDTH, 2 * HEIGHT, 6 * WIDTH, 0, WIDTH, HEIGHT );
    copy_part( in, out, 3 * WIDTH, 2 * HEIGHT, 7 * WIDTH, 0, WIDTH, HEIGHT );

    out.writeToFile( p_argv[ 2 ] );
}
