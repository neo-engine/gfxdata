// Arguments: input path, output file name
//
// The sprite may not contain more than 16 colors. The program automatically combines
// similar colors and will do so very aggressively to enforce this color limit.

#include <cstdio>
#include <map>

#include "gfxdata.h"

constexpr u8 HEIGHT = 96, WIDTH = 96, THRESHOLD = 0, NUM_FRAMES = 1;

map<pkmnInfo, bitmap> images;

int main( int p_argc, char** p_argv ) {

    if( p_argc < 2 ) {
        printf( "Too few arguments.\n" );
        return 1;
    }

    images = readPKMNPictures( p_argv[ 1 ] );

    printPKMNPictures( p_argv[ 2 ], false, false, images, 32, 32, 1 );
    printPKMNPictures( p_argv[ 2 ], false, true, images, 32, 32, 1 );
    printPKMNPictures( p_argv[ 2 ], true, false, images, 32, 32, 1 );
    printPKMNPictures( p_argv[ 2 ], true, true, images, 32, 32, 1 );

    return 0;
}
