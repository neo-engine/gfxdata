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
%:define conv( a ) ( (u8) ( ( a ) * 31 / 255 ) )
%:define revconv( a ) ( ( ( a ) * 255 / 31 ) )

%:define green( a ) ( revconv( ( ( a ) >> 10 ) & 31 ) )
%:define blue( a ) ( revconv( ( ( a ) >> 5 ) & 31 ) )
%:define red( a ) ( revconv( ( a ) & 31 ) )

using namespace std;
namespace fs = std::filesystem;

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

    bitmap   in( p_argv[ 1 ] );
    fs::path outp( p_argv[ 2 ] );

    u8 HEIGHT = in.m_height / 4;
    u8 WIDTH  = in.m_width / 4;

    auto nw = WIDTH / 32 * 32;
    auto nh = HEIGHT / 32 * 32;
    auto sx = 0, sy = 0;
    auto sx2 = 0, sy2 = 0;

#define WD( x ) ( ( x ) * WIDTH + sx )
#define HG( y ) ( ( y ) * HEIGHT + sy )
    if( WIDTH % 32 || HEIGHT % 32 ) {
        printf( "%s has wrong dimension %d %d, cropping to %d %d\n", outp.stem( ).c_str( ),
                WIDTH / 2, HEIGHT / 2, nw / 2, nh / 2 );
        sx  = ( WIDTH - nw ) / 2;
        sy  = ( HEIGHT - nh ) / 2;
        sx2 = ( WIDTH - nw ) - sx;
        sy2 = ( HEIGHT - nh ) - sy;
    }

    bitmap out( 8 * nw, nh );

    copy_part( in, out, WD( 1 ), HG( 0 ), 0 * nw, 0, nw, nh );
    copy_part( in, out, WD( 3 ), HG( 0 ), 1 * nw, 0, nw, nh );
    copy_part( in, out, WD( 1 ), HG( 3 ), 2 * nw, 0, nw, nh );
    copy_part( in, out, WD( 3 ), HG( 3 ), 3 * nw, 0, nw, nh );
    copy_part( in, out, WD( 1 ), HG( 1 ), 4 * nw, 0, nw, nh );
    copy_part( in, out, WD( 3 ), HG( 1 ), 5 * nw, 0, nw, nh );
    copy_part( in, out, WD( 1 ), HG( 2 ), 6 * nw, 0, nw, nh );
    copy_part( in, out, WD( 3 ), HG( 2 ), 7 * nw, 0, nw, nh );

    auto pidx = 0, form = 0;
    char buffer[ 10 ] = { 0 };

    auto a = 0, b = 0, c = 0, d = 0;

    if( ( a = sscanf( outp.stem( ).c_str( ), "%u_%u%[sf]", &pidx, &form, buffer ) ) < 3 ) {
        if( ( b = sscanf( outp.stem( ).c_str( ), "%u_%u", &pidx, &form ) ) < 2
            && ( c = sscanf( outp.stem( ).c_str( ), "%u%[sf]", &pidx, buffer ) ) < 2 ) {
            d = sscanf( outp.stem( ).c_str( ), "%u", &pidx );
        }
    }

    string sz = "";
    // string( "_" ) + to_string( nw / 2 ) + "_" + to_string( nh / 2 );

    // printf( "%s ~> %d %d %d %d | %d-%d%s\n", outp.stem( ).c_str( ), a, b, c, d, pidx, form,
    //        buffer );

    if( form ) {
        fs::path res = outp.parent_path( )
                       / ( std::to_string( pidx ) + "_" + std::to_string( form )
                           + std::string( buffer ) + sz + outp.extension( ).string( ) );
        out.writeToFile( res.c_str( ) );
    } else {
        fs::path res = outp.parent_path( )
                       / ( std::to_string( pidx ) + std::string( buffer ) + sz
                           + outp.extension( ).string( ) );
        out.writeToFile( res.c_str( ) );
    }
}
