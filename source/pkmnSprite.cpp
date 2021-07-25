// Converts a pkmn sprite to a rsd file.
//
// Arguments: input path, output file name
//
// The sprite may not contain more than 16 colors. The program automatically combines
// similar colors and will do so very aggressively to enforce this color limit.

#include <cstdio>
#include <map>

#include "gfxdata.h"

constexpr u8 THRESHOLD = 0;

map<pkmnInfo, bitmap> images;

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

    images = readPKMNPictures( p_argv[ 1 ] );

    printPKMNPictures( p_argv[ 2 ], false, false, images, hg, wd, fr );
    printPKMNPictures( p_argv[ 2 ], false, true, images, hg, wd, fr );
    printPKMNPictures( p_argv[ 2 ], true, false, images, hg, wd, fr );
    printPKMNPictures( p_argv[ 2 ], true, true, images, hg, wd, fr );

    return 0;
}
