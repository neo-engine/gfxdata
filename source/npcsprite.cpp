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

constexpr u16 TRANSPARENT_COLOR = 0;

constexpr u32 MAX_FRAMES_16_32 = 20;
constexpr u32 SIZE_16_32       = 16 * 32 / 2;

u16 FORCE_PAL_3[ 16 ] = {
    TRANSPARENT_COLOR,
    0x2285,
    0x15c8,
    0x1d67,
    0x1505,
    0x363a,
    0x294c,
    0x2973,
    0x18c6,
    0x46fe,
    0x2d9a,
    0x24f8,
    0x2ac4,
    0x2a89,
    0x1d40,
    0x25e2,
};

u16 FORCE_PAL_8[ 16 ] = {
    TRANSPARENT_COLOR,
    0x7bde,
    0x0000,
    0x0000,
    0x52f7,
    0x4231,
    0x396b,
    0x3107,
    0x28a4,
    0x7710,
    0x72cd,
    0x437e,
    0x2eda,
    0x2255,
    0x0dd0,
    0x112d,
};

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

        auto force_pal = 0;

        if( !cwd ) { cwd = wd; }
        if( !chg ) { chg = hg; }
        if( chg > 255 ) {
            force_pal = chg >> 8;
            chg &= 255;
        }

        printf( "%d: %d %d extracted sizes: %d %d ~> %d frames, force pal %d\n", i, img.m_width,
                img.m_height, cwd, chg, img.m_width / cwd, force_pal );
        fflush( stdout );

        if( force_pal == 3 ) {
            u8 colsUsed = 16;
            printImage( FORCE_PAL_3, colsUsed, f, fname, img, chg, cwd, img.m_width / cwd,
                        THRESHOLD, true );

        } else if( force_pal == 8 ) {
            u8 colsUsed = 16;
            printImage( FORCE_PAL_8, colsUsed, f, fname, img, chg, cwd, img.m_width / cwd,
                        THRESHOLD, true );
        } else {
            printImage( f, fname, img, chg, cwd, img.m_width / cwd, THRESHOLD, true,
                        TRANSPARENT_COLOR, false );
        }
        // write padding
        auto currentSize
            = 3 /*meta*/ + 16 * sizeof( u16 ) /*palette*/ + img.m_width * img.m_height / 2;

        printf( "%d: writing %u padding.\n", i, header.m_maxEntrySize - currentSize );
        for( auto p = currentSize; p < header.m_maxEntrySize; ++p ) { std::putc( 0, f ); }
    }
    fclose( f );
    return 0;
}
