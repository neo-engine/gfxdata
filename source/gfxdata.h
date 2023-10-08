#include <filesystem>
#include <map>
#include <string>
#include <tuple>
#include <vector>

#include <png.h>
#include "bitmap.h"

using namespace std;
namespace fs = filesystem;
typedef long long unsigned u64;
typedef long long int      s64;
typedef unsigned           u32;
typedef int                s32;
typedef unsigned short     u16;
typedef short int          s16;
typedef unsigned char      u8;
typedef signed char        s8;

#define conv( a ) ( (u8) ( (a) *31 / 255 ) )
#define revconv( a ) ( ( (a) *255 / 31 ) )

#define green( a ) ( revconv( ( ( a ) >> 10 ) & 31 ) )
#define blue( a ) ( revconv( ( ( a ) >> 5 ) & 31 ) )
#define red( a ) ( revconv( (a) &31 ) )

typedef tuple<u8, u8, u8> t3;

void print_tiled( FILE* p_f, u8* p_image_data, u8 p_width, u8 p_height );
void print_tiled( FILE* p_f, u8* p_image_data );
int  col_dis( int p_1, int p_2 );

map<string, bitmap> readPictures( const string& p_path );

constexpr t3 major( t3 p_1, t3 p_2, t3 p_3, t3 p_4 ) {
    if( p_1 == p_2 || p_1 == p_3 || p_1 == p_4 ) { return p_1; }
    if( p_2 == p_3 || p_2 == p_4 ) { return p_2; }
    if( p_3 == p_4 ) { return p_3; }
    return p_4;
}

struct pkmnInfo {
    u16  m_idx    = 0;
    u8   m_forme  = 0;
    bool m_shiny  = false;
    bool m_female = false;
    bool m_right  = false;

    constexpr auto operator<=>( const pkmnInfo& ) const = default;
};

vector<vector<bitmap>> readTranslatablePictures( const string& p_path );
vector<bitmap>         readNumberedPictures( const string& p_path );
map<u32, bitmap>       readIndexedPictures( const string& p_path );
map<pkmnInfo, bitmap>  readPKMNPictures( const string& p_path );

void printImage( FILE* p_out, const string& p_name, const bitmap& p_img, u16 p_height, u16 p_width,
                 u8 p_frames, u8 p_threshold, bool p_rsd = false, u16 p_transparent = 0 );

void printPKMNPictures( const char* p_name, bool p_female, bool p_shiny,
                        map<pkmnInfo, bitmap>& p_images, u16 p_height, u16 p_width, u8 p_frames,
                        u8 p_threshold = 0 );

void printPKMNPicturesSimple( const char* p_name, bool p_female, bool p_shiny,
                              map<pkmnInfo, bitmap>& p_images, u8 p_frames, u8 p_threshold = 0,
                              bool p_rsd = false );
