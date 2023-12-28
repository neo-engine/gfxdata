// converts npc sprite sheets into an rsdbank
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

constexpr u32 MAX_FRAMES_16_32 = 20;
constexpr u32 SIZE_16_32       = 16 * 32 / 2;

struct rsdBankHeader {
    u32 m_maxEntrySize = MAX_FRAMES_16_32 * SIZE_16_32 + 16 * sizeof( u16 ) + 3;
};

int main( int p_argc, char** p_argv ) {
    if( p_argc < 3 ) {
        printf( "Too few arguments.\n" );
        return 1;
    }

    u16 hg = 32, wd = 16;

    auto images = readNumberedPicturesWithSize( p_argv[ 1 ], wd, hg );

    fs::create_directories( std::string( FSROOT "/" ) );
    auto fname = string( FSROOT "/" ) + string( p_argv[ 2 ] ) + ".rsdb";
    printf( "%s", fname.c_str( ) );
    auto f = fopen( fname.c_str( ), "wb" );
    assert( f );

    rsdBankHeader header{ };

    fwrite( &header, 1, sizeof( rsdBankHeader ), f );

    for( auto i = 0; i < images.size( ); ++i ) {
        const auto& [ img, sz ] = images[ i ];
        auto [ cwd, chg ]       = sz;

        if( !cwd ) { cwd = wd; }
        if( !chg ) { chg = hg; }

        printf( "%d: %d %d extracted sizes: %d %d ~> %d frames\n", i, img.m_width, img.m_height,
                cwd, chg, img.m_width / cwd );
        fflush( stdout );

        printImage( f, fname, img, chg, cwd, img.m_width / cwd, THRESHOLD, true );
        // write padding
        auto currentSize
            = 3 /*meta*/ + 16 * sizeof( u16 ) /*palette*/ + img.m_width * img.m_height / 2;

        printf( "%d: writing %u padding.\n", i, header.m_maxEntrySize - currentSize );
        for( auto p = currentSize; p < header.m_maxEntrySize; ++p ) { std::putc( 0, f ); }
    }
    fclose( f );
    return 0;
}
