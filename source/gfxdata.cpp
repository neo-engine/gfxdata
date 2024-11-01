#include <algorithm>
#include <cassert>
#include <cstdio>
#include <cstring>
#include <filesystem>
#include <map>
#include <string>
#include <vector>

#include "gfxdata.h"

constexpr u8 PKMN_SPRITE = 96;

bool DEBUG = false;

void print_tiled( FILE* p_f, u8* p_image_data, u8 p_width, u8 p_height ) {
    // Tile the given sprite into 8x8 blocks
    for( size_t y = 0; y < p_height; y += 8 ) {
        for( size_t x = 0; x < p_width; x += 4 ) {
            for( size_t by = 0; by < 8; ++by ) {
                fwrite( p_image_data + ( y + by ) * p_width + x, 1, 4, p_f );
            }
        }
    }
}

void print_tiled( FILE* p_f, u8* p_image_data ) {
    // Tile the given sprite into 8x8 blocks
    // 64x64 chunk
    for( size_t y = 0; y < 64; y += 8 ) {
        for( size_t x = 0; x < 32; x += 4 ) {
            for( size_t by = 0; by < 8; ++by ) {
                fwrite( p_image_data + ( y + by ) * 48 + x, 1, 4, p_f );
            }
        }
    }
    // 64x32 chunk
    for( size_t y = 0; y < 64; y += 8 ) {
        for( size_t x = 32; x < 48; x += 4 ) {
            for( size_t by = 0; by < 8; ++by ) {
                fwrite( p_image_data + ( y + by ) * 48 + x, 1, 4, p_f );
            }
        }
    }
    // 32x64 chunk
    for( size_t y = 64; y < 96; y += 8 ) {
        for( size_t x = 0; x < 32; x += 4 ) {
            for( size_t by = 0; by < 8; ++by ) {
                fwrite( p_image_data + ( y + by ) * 48 + x, 1, 4, p_f );
            }
        }
    }
    // 32x32 chunc
    for( size_t y = 64; y < 96; y += 8 ) {
        for( size_t x = 32; x < 48; x += 4 ) {
            for( size_t by = 0; by < 8; ++by ) {
                fwrite( p_image_data + ( y + by ) * 48 + x, 1, 4, p_f );
            }
        }
    }
}

int col_dis( int p_1, int p_2 ) {
    return abs( red( p_1 ) - red( p_2 ) ) + abs( green( p_1 ) - green( p_2 ) )
           + abs( blue( p_1 ) - blue( p_2 ) );
}

map<string, bitmap> readPictures( const string& p_path ) {
    auto       res = map<string, bitmap>( );
    error_code ec;

    for( auto& p : fs::directory_iterator( p_path ) ) {
        if( !p.is_regular_file( ec ) || ec ) { continue; }
        auto fnm   = p.path( ).stem( );
        auto nm    = p.path( );
        res[ fnm ] = bitmap( nm.c_str( ) );
    }

    return res;
}

vector<vector<bitmap>> readTranslatablePictures( const string& p_path ) {
    auto res = vector<vector<bitmap>>( );

    auto tmp = readPictures( p_path );
    for( const auto& [ id, img ] : tmp ) {
        u32 num, lang;
        sscanf( id.c_str( ), "%u.%u", &num, &lang );

        if( num >= res.size( ) ) { res.resize( num + 1 ); }
        if( lang >= res[ num ].size( ) ) { res[ num ].resize( lang + 1, img ); }
        res[ num ][ lang ] = img;
    }
    return res;
}

vector<bitmap> readNumberedPictures( const string& p_path ) {
    auto res = vector<bitmap>( );

    auto tmp = readPictures( p_path );
    for( const auto& [ id, img ] : tmp ) {
        u32 num = 0;
        if( !sscanf( id.c_str( ), "%u", &num ) ) { continue; }

        if( num >= res.size( ) ) { res.resize( num + 10, img ); }
        res[ num ] = img;
    }
    return res;
}

vector<std::pair<bitmap, std::pair<u16, u16>>>
readNumberedPicturesWithSize( const string& p_path, u16 p_defaultWidth, u16 p_defaultHeight ) {
    auto res = vector<std::pair<bitmap, std::pair<u16, u16>>>( );

    auto tmp = readPictures( p_path );
    for( const auto& [ id, img ] : tmp ) {
        u32 num = 0;
        u32 wid = 0;
        u32 hei = 0;
        u32 pal = 0;

        if( sscanf( id.c_str( ), "%u_%u_%u_%u", &num, &wid, &hei, &pal ) ) {
        } else if( sscanf( id.c_str( ), "%u_%u_%u", &num, &wid, &hei ) ) {
        } else if( sscanf( id.c_str( ), "%u", &num ) ) {
            wid = p_defaultWidth;
            hei = p_defaultHeight;
        } else {
            continue;
        }

        if( num >= res.size( ) ) {
            res.resize( num + 10, { bitmap{ p_defaultWidth, p_defaultHeight },
                                    { p_defaultWidth, p_defaultHeight } } );
        }
        res[ num ] = { img, { wid, hei + ( pal << 8 ) } };
    }
    return res;
}

map<u32, bitmap> readIndexedPictures( const string& p_path ) {
    auto res = map<u32, bitmap>( );

    auto tmp = readPictures( p_path );
    for( const auto& [ id, img ] : tmp ) {
        u32 num = 0;
        if( !sscanf( id.c_str( ), "%u", &num ) ) { continue; }
        res[ num ] = img;
    }
    return res;
}

map<pkmnInfo, bitmap> readPKMNPictures( const string& p_path ) {
    auto tmp = readPictures( p_path );
    auto res = map<pkmnInfo, bitmap>( );
    for( const auto& [ id, img ] : tmp ) {
        auto cinfo = pkmnInfo( );

        // parse id
        auto ptr = id.begin( );
        while( ptr != id.end( ) && ( *ptr >= '0' && *ptr <= '9' ) ) {
            cinfo.m_idx *= 10;
            cinfo.m_idx += ( *ptr ) - '0';
            ptr++;
        }

        if( ptr == id.end( ) ) {
            res[ cinfo ] = img;
            continue;
        }

        for( u8 i = 0; i < 2; ++i ) {
            if( *ptr == 's' || *ptr == 'S' ) {
                cinfo.m_shiny = true;
                if( ++ptr == id.end( ) ) {
                    res[ cinfo ] = img;
                    continue;
                }
            }
            if( *ptr == 'f' || *ptr == 'F' ) {
                cinfo.m_female = true;
                if( ++ptr == id.end( ) ) {
                    res[ cinfo ] = img;
                    continue;
                }
            }
            if( *ptr == 'm' || *ptr == 'M' ) {
                cinfo.m_female = false;
                if( ++ptr == id.end( ) ) {
                    res[ cinfo ] = img;
                    continue;
                }
            }
            if( *ptr == 'b' || *ptr == 'B' ) {
                if( ++ptr == id.end( ) ) {
                    res[ cinfo ] = img;
                    continue;
                }
            }
            if( *ptr == 'r' || *ptr == 'R' ) {
                cinfo.m_right = true;
                if( ++ptr == id.end( ) ) {
                    res[ cinfo ] = img;
                    continue;
                }
            }
        }

        if( *ptr == '-' || *ptr == '_' ) {
            ++ptr;
            while( ptr != id.end( ) && ( *ptr >= '0' && *ptr <= '9' ) ) {
                cinfo.m_forme *= 10;
                cinfo.m_forme += ( *ptr ) - '0';
                ptr++;
            }
        }

        if( ptr == id.end( ) ) {
            res[ cinfo ] = img;
            continue;
        }

        for( u8 i = 0; i < 2; ++i ) {
            if( *ptr == 's' || *ptr == 'S' ) {
                cinfo.m_shiny = true;
                if( ++ptr == id.end( ) ) {
                    res[ cinfo ] = img;
                    continue;
                }
            }
            if( *ptr == 'f' || *ptr == 'F' ) {
                cinfo.m_female = true;
                if( ++ptr == id.end( ) ) {
                    res[ cinfo ] = img;
                    continue;
                }
            }
            if( *ptr == 'm' || *ptr == 'M' ) {
                cinfo.m_female = false;
                if( ++ptr == id.end( ) ) {
                    res[ cinfo ] = img;
                    continue;
                }
            }
            if( *ptr == 'b' || *ptr == 'B' ) {
                if( ++ptr == id.end( ) ) {
                    res[ cinfo ] = img;
                    continue;
                }
            }
            if( *ptr == 'r' || *ptr == 'R' ) {
                cinfo.m_right = true;
                if( ++ptr == id.end( ) ) {
                    res[ cinfo ] = img;
                    continue;
                }
            }
        }
    }
    return res;
}

void printImage( u16 p_pal[ 16 ], u8& p_colorsUsed, FILE* p_out, const string& p_name,
                 const bitmap& p_img, u16 p_height, u16 p_width, u8 p_frames, u8 p_threshold,
                 bool p_rsddata ) {
    size_t SCALE = 1;
    if( p_img.m_width == 2 * p_width * p_frames || p_img.m_height == 2 * p_height ) { SCALE = 2; }

    u8 start                                  = 0;
    u8 image_data[ 256 * 256 * 20 / 4 + 100 ] = { 0 };

    map<u16, u8> palidx;

    palidx[ 0xceed ] = 0;
    palidx[ 0x530e ] = 0;

    for( u8 i = 0; i < p_colorsUsed; ++i ) { palidx[ p_pal[ i ] ] = i; }
    p_colorsUsed = std::max( p_colorsUsed, u8{ !p_img( 0, 0 ).m_transparent } );

    for( size_t frame = 0; frame < p_frames; ++frame )
        for( size_t y = 0; y < p_height; y++ )
            for( size_t x = 0; x < p_width; x++ ) {
                size_t nx = x + p_width * frame;

                u16 conv_color = ( conv( p_img( nx * SCALE, y * SCALE ).m_red ) )
                                 | ( conv( p_img( nx * SCALE, y * SCALE ).m_green ) << 5 )
                                 | ( conv( p_img( nx * SCALE, y * SCALE ).m_blue ) << 10 )
                                 | ( 1 << 15 );
                if( !p_img( nx * SCALE, y * SCALE ).m_transparent ) { conv_color = p_pal[ 0 ]; }
                if( !palidx.count( conv_color ) ) {
                    // Check if the new color is very close to an existing color
                    u8 min_del = 255, del_p = 0;
                    for( u8 p = 1 + start; p < 16; ++p ) {
                        if( col_dis( conv_color, p_pal[ p ] ) < min_del ) {
                            min_del = col_dis( conv_color, p_pal[ p ] );
                            del_p   = p;
                        }
                    }

                    if( min_del < p_threshold && p_colorsUsed + start ) {
                        fprintf( stderr,
                                 "[%s] replacing \x1b[48;2;%u;%u;%um%3hx\x1b[0;00m"
                                 " with \x1b[48;2;%u;%u;%um%3hx\x1b[0;00m (%hu)\n",
                                 p_name.c_str( ), red( conv_color ), blue( conv_color ),
                                 green( conv_color ), conv_color, red( p_pal[ del_p ] ),
                                 blue( p_pal[ del_p ] ), green( p_pal[ del_p ] ), p_pal[ del_p ],
                                 del_p );
                        palidx[ conv_color ] = del_p;
                    } else if( p_colorsUsed + start >= 16 ) {
                        fprintf( stderr, "[%s] To COLORFUL:", p_name.c_str( ) );
                        fprintf( stderr,
                                 " replacing \x1b[48;2;%u;%u;%um%3hx\x1b[0;00m"
                                 " with \x1b[48;2;%u;%u;%um%3hx\x1b[0;00m\n",
                                 red( conv_color ), blue( conv_color ), green( conv_color ),
                                 conv_color, red( p_pal[ del_p ] ), blue( p_pal[ del_p ] ),
                                 green( p_pal[ del_p ] ), p_pal[ del_p ] );
                        palidx[ conv_color ] = del_p;
                    } else {
                        p_pal[ p_colorsUsed + start ] = conv_color;
                        palidx[ conv_color ]          = p_colorsUsed++;
                    }
                }
                image_data[ p_width * p_height * frame + y * p_width + x ]
                    = start + palidx[ conv_color ];
            }

    size_t numTiles = p_height * p_width * p_frames, numColors = 16;

    for( size_t x = 0; x < 16; ++x ) {
        printf( "\x1b[48;2;%u;%u;%um%04hx\x1b[0;00m ", red( p_pal[ x ] ), blue( p_pal[ x ] ),
                green( p_pal[ x ] ), p_pal[ x ] );
    }
    printf( "\n" );
    printf( "\n" );

    for( size_t x = 0; x < numTiles; ++x ) {
        if( image_data[ x ] ) {
            printf( "\x1b[48;2;%u;%u;%um%3hx\x1b[0;00m", red( p_pal[ image_data[ x ] ] ),
                    blue( p_pal[ image_data[ x ] ] ), green( p_pal[ image_data[ x ] ] ),
                    image_data[ x ] );
        } else {
            printf( "   " );
        }
        if( ( x % p_width ) == p_width - 1 ) printf( "\n" );
    }
    printf( "\n" );

    // As we are dealing with sprites here, two neighboring pixels share a single byte.
    for( size_t i = 0; i < numTiles / 2; ++i ) {
        image_data[ i ] = ( image_data[ 2 * i + 1 ] << 4 ) | image_data[ 2 * i ];
    }

    fwrite( p_pal, sizeof( u16 ), numColors, p_out );

    if( p_rsddata ) {
        u8 meta[ 3 ] = { p_frames, static_cast<u8>( p_width ), static_cast<u8>( p_height ) };
        fwrite( meta, sizeof( u8 ), 3, p_out );
    }

    for( size_t fr = 0; fr < p_frames; ++fr ) {
        if( p_height == PKMN_SPRITE && p_width == PKMN_SPRITE ) {
            print_tiled( p_out, image_data + fr * p_width / 2 * p_height );
        } else {
            print_tiled( p_out, image_data + fr * p_width / 2 * p_height, p_width / 2, p_height );
        }
    }
}

void printImage( FILE* p_out, const string& p_name, const bitmap& p_img, u16 p_height, u16 p_width,
                 u8 p_frames, u8 p_threshold, bool p_rsddata, u16 p_transparent, bool p_palLast ) {
    u8     col   = 0;
    size_t SCALE = 1;
    if( p_img.m_width == 2 * p_width * p_frames || p_img.m_height == 2 * p_height ) { SCALE = 2; }

    u8  start                                  = 0;
    u8  image_data[ 256 * 256 * 20 / 4 + 100 ] = { 0 };
    u16 pal[ 300 ]                             = { 0 };

    map<u16, u8> palidx;
    if( p_transparent ) {
        palidx[ p_transparent ] = 0;
        pal[ 0 ]                = p_transparent;
    } else if( !p_img( 0, 0 ).m_transparent ) {
        palidx[ 0 ] = 0;
        pal[ 0 ]    = 0;
        start       = 1;
    }

    if( p_transparent ) {
        fprintf( stderr, "[%s] Using transparent color \x1b[48;2;%u;%u;%um%3hx\x1b[0;00m\n",
                 p_name.c_str( ), red( p_transparent ), blue( p_transparent ),
                 green( p_transparent ), p_transparent );
        start = 1;
    }

    bool had_replace = false;

    for( size_t frame = 0; frame < p_frames; ++frame )
        for( size_t y = 0; y < p_height; y++ )
            for( size_t x = 0; x < p_width; x++ ) {
                size_t nx         = x + p_width * frame;
                u16    conv_color = ( conv( p_img( nx * SCALE, y * SCALE ).m_red ) )
                                 | ( conv( p_img( nx * SCALE, y * SCALE ).m_green ) << 5 )
                                 | ( conv( p_img( nx * SCALE, y * SCALE ).m_blue ) << 10 )
                                 | ( 1 << 15 );
                if( !p_img( nx * SCALE, y * SCALE ).m_transparent ) { conv_color = p_transparent; }
                if( !palidx.count( conv_color ) ) {
                    // Check if the new color is very close to an existing color
                    u8 min_del = 255, del_p = 0;
                    for( u8 p = 1 + start; p < 16; ++p ) {
                        if( col_dis( conv_color, pal[ p ] ) < min_del ) {
                            min_del = col_dis( conv_color, pal[ p ] );
                            del_p   = p;
                        }
                    }

                    if( min_del < p_threshold && col + start ) {
                        fprintf( stderr,
                                 "[%s] replacing \x1b[48;2;%u;%u;%um%3hx\x1b[0;00m"
                                 " with \x1b[48;2;%u;%u;%um%3hx\x1b[0;00m (%hu)\n",
                                 p_name.c_str( ), red( conv_color ), blue( conv_color ),
                                 green( conv_color ), conv_color, red( pal[ del_p ] ),
                                 blue( pal[ del_p ] ), green( pal[ del_p ] ), pal[ del_p ], del_p );
                        palidx[ conv_color ] = del_p;
                        had_replace          = true;
                    } else if( col + start > 16 ) {
                        fprintf( stderr, "[%s] To COLORFUL:", p_name.c_str( ) );
                        fprintf( stderr,
                                 " replacing \x1b[48;2;%u;%u;%um%3hx\x1b[0;00m"
                                 " with \x1b[48;2;%u;%u;%um%3hx\x1b[0;00m\n",
                                 red( conv_color ), blue( conv_color ), green( conv_color ),
                                 conv_color, red( pal[ del_p ] ), blue( pal[ del_p ] ),
                                 green( pal[ del_p ] ), pal[ del_p ] );
                        palidx[ conv_color ] = del_p;
                        had_replace          = true;
                    } else {
                        pal[ col + start ]   = conv_color;
                        palidx[ conv_color ] = start + col++;
                    }
                }
                image_data[ p_width * p_height * frame + y * p_width + x ] = palidx[ conv_color ];
            }

    size_t numTiles = p_height * p_width * p_frames, numColors = 16;

    if( had_replace || DEBUG ) {
        printf( "[%s]: ", p_name.c_str( ) );
        for( auto i = 0; i < 16; ++i ) {
            printf( "\x1b[48;2;%u;%u;%um%3hx\x1b[0;00m", red( pal[ i ] ), blue( pal[ i ] ),
                    green( pal[ i ] ), pal[ i ] );
        }
        printf( "\n" );
        for( size_t x = 0; x < numTiles; ++x ) {
            if( image_data[ x ] ) {
                printf( "\x1b[48;2;%u;%u;%um%3hx\x1b[0;00m", red( pal[ image_data[ x ] ] ),
                        blue( pal[ image_data[ x ] ] ), green( pal[ image_data[ x ] ] ),
                        image_data[ x ] );
            } else {
                printf( "   " );
            }

            if( ( x % p_width ) == p_width - 1 ) printf( "\n" );

            //        if( x % ( numTiles / 6 ) == ( numTiles / 6 ) - 1 ) printf( "\n" );
        }
        printf( "\n" );
    }

    // As we are dealing with sprites here, two neighboring pixels share a single byte.
    for( size_t i = 0; i < numTiles / 2; ++i ) {
        image_data[ i ] = ( image_data[ 2 * i + 1 ] << 4 ) | image_data[ 2 * i ];
    }

    if( !p_palLast ) { fwrite( pal, sizeof( u16 ), numColors, p_out ); }

    if( p_rsddata ) {
        u8 meta[ 3 ] = { p_frames, static_cast<u8>( p_width ), static_cast<u8>( p_height ) };
        fwrite( meta, sizeof( u8 ), 3, p_out );
    }

    for( size_t fr = 0; fr < p_frames; ++fr ) {
        if( p_height == PKMN_SPRITE && p_width == PKMN_SPRITE ) {
            print_tiled( p_out, image_data + fr * p_width / 2 * p_height );
        } else {
            print_tiled( p_out, image_data + fr * p_width / 2 * p_height, p_width / 2, p_height );
        }
    }

    if( p_palLast ) {
        pal[ 0 ] = 0;
        fwrite( pal, sizeof( u16 ), numColors, p_out );
    }
}

void printPKMNPictures( const char* p_name, bool p_female, bool p_shiny,
                        map<pkmnInfo, bitmap>& p_images, u16 p_height, u16 p_width, u8 p_frames,
                        u8 p_threshold ) {
    auto fname = string( FSROOT "/" ) + string( p_name );
    fs::create_directories( std::string( FSROOT ) );
    if( p_female ) { fname += "f"; }
    if( p_shiny ) { fname += "s"; }
    fname += ".pkmn.sprb";

    FILE* f = fopen( fname.c_str( ), "wb" );
    for( u16 i = 0; i <= MAX_PKMN; ++i ) {
        pkmnInfo pin = { i, 0, p_shiny, p_female };
        string   nm  = string( p_name ) + " " + to_string( i ) + " f" + to_string( p_female ) + " *"
                    + to_string( p_shiny );

        if( p_images.count( pin ) ) {
            printImage( f, nm, p_images[ pin ], p_height, p_width, p_frames, p_threshold );
            continue;
        }
        if( p_female ) { pin.m_female = false; }
        if( p_images.count( pin ) ) {
            printImage( f, nm, p_images[ pin ], p_height, p_width, p_frames, p_threshold );
            continue;
        }
        if( p_shiny ) { pin.m_shiny = false; }
        if( p_images.count( pin ) ) {
            printImage( f, nm, p_images[ pin ], p_height, p_width, p_frames, p_threshold );
            continue;
        }

        pin.m_idx = 0;
        printImage( f, nm, p_images[ pin ], p_height, p_width, p_frames, p_threshold );
    }
    fclose( f );

    // write forms into separate files

    for( const auto& [ info, img ] : p_images ) {
        if( !info.m_forme ) { continue; }

        if( info.m_shiny != p_shiny && info.m_female != p_female ) {
            auto i2 = info;
            i2.m_shiny ^= 1;
            i2.m_female ^= 1;
            if( p_images.count( i2 ) ) { continue; }
        }
        if( info.m_shiny != p_shiny ) {
            auto i2 = info;
            i2.m_shiny ^= 1;
            if( p_images.count( i2 ) ) { continue; }
        }
        if( info.m_female != p_female ) {
            auto i2 = info;
            i2.m_female ^= 1;
            if( p_images.count( i2 ) ) { continue; }
        }

        fname = string( FSROOT "/" ) + string( p_name ) + "/"
                + to_string( info.m_idx / MAX_ITEMS_PER_DIR );
        fs::create_directories( fname );
        fname += "/" + to_string( info.m_idx ) + "_" + to_string( info.m_forme );
        if( p_female ) { fname += "f"; }
        if( p_shiny ) { fname += "s"; }
        fname += ".raw";
        auto nm = string( p_name ) + " " + to_string( info.m_idx ) + "_" + to_string( info.m_forme )
                  + " f" + to_string( p_female ) + " *" + to_string( p_shiny );
        f = fopen( fname.c_str( ), "wb" );
        printImage( f, nm, img, p_height, p_width, p_frames, p_threshold );
        fclose( f );
    }
}

constexpr u32 MAX_FRAMES_64_64 = 8;
constexpr u32 SIZE_64_64       = 64 * 64 / 2;
struct rsdBankHeader {
    u32 m_maxEntrySize = MAX_FRAMES_64_64 * SIZE_64_64 + 16 * sizeof( u16 ) + 3;
};

void printPKMNPicturesSemiSimple( const char* p_name, bool p_female, bool p_shiny,
                                  map<pkmnInfo, bitmap>& p_images, u8 p_threshold ) {

    auto FRAMES = 8;
    auto RSD    = true;

    auto fname = string( FSROOT "/" ) + string( p_name );
    fs::create_directories( std::string( FSROOT ) );
    if( p_female ) { fname += "f"; }
    if( p_shiny ) { fname += "s"; }
    fname += ".rsdb";

    FILE*         f = fopen( fname.c_str( ), "wb" );
    rsdBankHeader header{ };
    fwrite( &header, 1, sizeof( rsdBankHeader ), f );
    for( u16 i = 0; i <= MAX_PKMN; ++i ) {
        pkmnInfo pin = { i, 0, p_shiny, p_female };
        string   nm  = string( p_name ) + " " + to_string( i ) + " f" + to_string( p_female ) + " *"
                    + to_string( p_shiny );

        auto HEIGHT = 32, WIDTH = 32;

        if( p_images.count( pin ) ) {
            HEIGHT = p_images[ pin ].m_height / 2;
            WIDTH  = p_images[ pin ].m_width / ( 2 * FRAMES );
            printImage( f, nm, p_images[ pin ], HEIGHT, WIDTH, FRAMES, p_threshold, RSD );
            // write padding
            auto currentSize = 3 /*meta*/ + 16 * sizeof( u16 ) /*palette*/
                               + FRAMES * WIDTH * HEIGHT / 2;

            printf( "[%s] sz %d %d, writing %d padding.\n", nm.c_str( ), WIDTH, HEIGHT,
                    int( header.m_maxEntrySize ) - int( currentSize ) );
            for( auto p = currentSize; p < header.m_maxEntrySize; ++p ) { std::putc( 0, f ); }

            continue;
        }
        if( p_female ) { pin.m_female = false; }
        if( p_images.count( pin ) ) {
            HEIGHT = p_images[ pin ].m_height / 2;
            WIDTH  = p_images[ pin ].m_width / ( 2 * FRAMES );
            printImage( f, nm, p_images[ pin ], HEIGHT, WIDTH, FRAMES, p_threshold, RSD );
            // write padding
            auto currentSize = 3 /*meta*/ + 16 * sizeof( u16 ) /*palette*/
                               + FRAMES * WIDTH * HEIGHT / 2;

            printf( "[%s] sz %d %d, writing %d padding.\n", nm.c_str( ), WIDTH, HEIGHT,
                    int( header.m_maxEntrySize ) - int( currentSize ) );
            for( auto p = currentSize; p < header.m_maxEntrySize; ++p ) { std::putc( 0, f ); }

            continue;
        }
        if( p_shiny ) { pin.m_shiny = false; }
        if( p_images.count( pin ) ) {
            HEIGHT = p_images[ pin ].m_height / 2;
            WIDTH  = p_images[ pin ].m_width / ( 2 * FRAMES );
            printImage( f, nm, p_images[ pin ], HEIGHT, WIDTH, FRAMES, p_threshold, RSD );

            // write padding
            auto currentSize = 3 /*meta*/ + 16 * sizeof( u16 ) /*palette*/
                               + FRAMES * WIDTH * HEIGHT / 2;

            printf( "[%s] sz %d %d, writing %d padding.\n", nm.c_str( ), WIDTH, HEIGHT,
                    int( header.m_maxEntrySize ) - int( currentSize ) );
            for( auto p = currentSize; p < header.m_maxEntrySize; ++p ) { std::putc( 0, f ); }

            continue;
        }

        pin.m_idx = 0;
        if( p_images.count( pin ) ) {
            printf( "[%s] does not exist\n", nm.c_str( ) );
            HEIGHT = 32;
            WIDTH  = 32;
            printImage( f, nm, p_images[ pin ], HEIGHT, WIDTH, FRAMES, p_threshold, RSD );
        } else {
            HEIGHT = WIDTH = 0;
        }
        // write padding
        auto currentSize = 3 /*meta*/ + 16 * sizeof( u16 ) /*palette*/
                           + FRAMES * WIDTH * HEIGHT / 2;

        printf( "[%s] sz %d %d, writing %d padding.\n", nm.c_str( ), WIDTH, HEIGHT,
                int( header.m_maxEntrySize ) - int( currentSize ) );
        for( auto p = currentSize; p < header.m_maxEntrySize; ++p ) { std::putc( 0, f ); }
    }
    fclose( f );

    // write forms into separate files

    for( const auto& [ info, img ] : p_images ) {
        if( !info.m_forme ) { continue; }

        if( info.m_shiny != p_shiny && info.m_female != p_female ) {
            auto i2 = info;
            i2.m_shiny ^= 1;
            i2.m_female ^= 1;
            if( p_images.count( i2 ) ) { continue; }
        }
        if( info.m_shiny != p_shiny ) {
            auto i2 = info;
            i2.m_shiny ^= 1;
            if( p_images.count( i2 ) ) { continue; }
        }
        if( info.m_female != p_female ) {
            auto i2 = info;
            i2.m_female ^= 1;
            if( p_images.count( i2 ) ) { continue; }
        }

        auto fname = string( FSROOT "/" ) + string( p_name ) + "/"
                     + to_string( info.m_idx / MAX_ITEMS_PER_DIR );
        fs::create_directories( fname );
        fname += "/" + to_string( info.m_idx );
        if( info.m_forme ) { fname += "_" + to_string( info.m_forme ); }

        if( p_female ) { fname += "f"; }
        if( p_shiny ) { fname += "s"; }
        if( RSD ) {
            fname += ".rsd";
        } else {
            fname += ".raw";
        }
        auto nm = string( p_name ) + " " + to_string( info.m_idx )
                  + ( info.m_forme ? "_" + to_string( info.m_forme ) : "" ) + " f"
                  + to_string( p_female ) + " *" + to_string( p_shiny );
        FILE* f = fopen( fname.c_str( ), "wb" );

        if( img.m_height >= 32 ) {
            printImage( f, nm, img, img.m_height / 2, img.m_width / ( 2 * FRAMES ), FRAMES,
                        p_threshold, RSD );
        } else {
            printImage( f, nm, img, img.m_height, img.m_width / FRAMES, FRAMES, p_threshold, RSD );
        }
        fclose( f );
    }
}

void printPKMNPicturesSimple( const char* p_name, bool p_female, bool p_shiny,
                              map<pkmnInfo, bitmap>& p_images, u8 p_frames, u8 p_threshold,
                              bool p_rsd ) {
    for( const auto& [ info, img ] : p_images ) {
        if( info.m_shiny != p_shiny && info.m_female != p_female ) {
            auto i2 = info;
            i2.m_shiny ^= 1;
            i2.m_female ^= 1;
            if( p_images.count( i2 ) ) { continue; }
        }
        if( info.m_shiny != p_shiny ) {
            auto i2 = info;
            i2.m_shiny ^= 1;
            if( p_images.count( i2 ) ) { continue; }
        }
        if( info.m_female != p_female ) {
            auto i2 = info;
            i2.m_female ^= 1;
            if( p_images.count( i2 ) ) { continue; }
        }

        auto fname = string( FSROOT "/" ) + string( p_name ) + "/"
                     + to_string( info.m_idx / MAX_ITEMS_PER_DIR );
        fs::create_directories( fname );
        fname += "/" + to_string( info.m_idx );
        if( info.m_forme ) { fname += "_" + to_string( info.m_forme ); }

        if( p_female ) { fname += "f"; }
        if( p_shiny ) { fname += "s"; }
        if( p_rsd ) {
            fname += ".rsd";
        } else {
            fname += ".raw";
        }
        auto nm = string( p_name ) + " " + to_string( info.m_idx )
                  + ( info.m_forme ? "_" + to_string( info.m_forme ) : "" ) + " f"
                  + to_string( p_female ) + " *" + to_string( p_shiny );
        FILE* f = fopen( fname.c_str( ), "wb" );

        if( img.m_height >= 32 ) {
            printImage( f, nm, img, img.m_height / 2, img.m_width / ( 2 * p_frames ), p_frames,
                        p_threshold, p_rsd );
        } else {
            printImage( f, nm, img, img.m_height, img.m_width / p_frames, p_frames, p_threshold,
                        p_rsd );
        }
        fclose( f );
    }
}
