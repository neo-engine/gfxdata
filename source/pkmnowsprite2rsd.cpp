// Converts a sprite to a rsd file
// Arguments: ###*.png, where ### is the id of the corresponding sprite
// HEIGHT, WIDTH, NUM_FRAMES (if > 0, generates *rsd), THERSHOLD, MAX_ITEMS_PER_DIR
//
// Outputs the file into folder (###)/30 (padded with zeros to a 2 character name) if ###
// exists; otherwise outputs to working dir
//
// The sprite may not contain more than 16 colors. The program automatically combines
// similar colors and will do so very aggressively to enforce this color limit.

#include <cstdio>
#include <map>
#include "gfxdata.h"

using namespace std;
namespace fs = filesystem;
map<pkmnInfo, bitmap> images;

int main( int p_argc, char** p_argv ) {
    if( p_argc < 2 ) {
        printf( "Too few arguments.\n" );
        return 1;
    }

    images = readPKMNPictures( p_argv[ 1 ] );

    printPKMNPicturesSimple( p_argv[ 2 ], false, false, images, 8, 0, true );
    printPKMNPicturesSimple( p_argv[ 2 ], false, true, images, 8, 0, true );
    printPKMNPicturesSimple( p_argv[ 2 ], true, false, images, 8, 0, true );
    printPKMNPicturesSimple( p_argv[ 2 ], true, true, images, 8, 0, true );
}
