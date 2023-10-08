//
// Copyright ( c ) 2014 Philip Wellnitz
// Using libpng
//

//
// This file is part of "Rechteckschoner".
//
// "Rechteckschoner" is distributed to be useful, but need not
// to be. Further, it is distributed WITHOUT ANY WARRANTY;
// without even the implied warranty of MERCHANTABILITY or
// FITNESS FOR A PARTICULAR PURPOSE.
//
// Permission is hereby granted, free of charge, to any person obtaining
// a copy of this software and associated documentation files ( the
// "Software" ), to deal in the Software without restriction, including
// without limitation the rights to use, copy, modify, merge, publish,
// distribute, sublicense, and/or sell copies of the Software, and to
// permit persons to whom the Software is furnished to do so, subject to
// the following conditions:
//
// The above copyright notice and this permission notice shall be included
// in all copies or substantial portions of the Software.
//

#include <cstdint>
#include <cstdio>
#include <cstdlib>

#include <png.h>

#include "bitmap.h"

///////////////////////////////////////////////////////////////////////////////
//
// Bitmap
//
///////////////////////////////////////////////////////////////////////////////

bitmap::bitmap( size_t p_width, size_t p_height ) {
    m_width  = p_width;
    m_height = p_height;
    m_pixels
        = std::vector<std::vector<pixel>>( p_width, std::vector<pixel>( p_height, { 0, 0, 0 } ) );
}

bitmap::bitmap( const char* p_path ) {
    static png_structp pngPtr;
    static png_infop   infoPtr;

    FILE* fd = std::fopen( p_path, "r" );
    if( !fd ) {
        std::fprintf( stderr, "Reading PNG at %s failed. (BAD FILE PATH)\n", p_path );
        return;
    }

    unsigned char sig[ 8 ];
    std::fread( sig, 1, 8, fd );
    if( !png_check_sig( sig, 8 ) ) {
        std::fprintf( stderr, "Reading PNG at %s failed. (BAD SIGNATURE)\n", p_path );
        return;
    }

    pngPtr = png_create_read_struct( PNG_LIBPNG_VER_STRING, NULL, NULL, NULL );
    if( !pngPtr ) {
        std::fprintf( stderr, "Reading PNG at %s failed. (OUT OF MEMORY)\n", p_path );
        return;
    }

    infoPtr = png_create_info_struct( pngPtr );
    if( !infoPtr ) {
        png_destroy_read_struct( &pngPtr, NULL, NULL );
        std::fprintf( stderr, "Reading PNG at %s failed. (OUT OF MEMORY)\n", p_path );
        return;
    }

    if( setjmp( png_jmpbuf( pngPtr ) ) ) {
        png_destroy_read_struct( &pngPtr, &infoPtr, NULL );
        std::fprintf( stderr, "Reading PNG at %s failed. (SETJMP TRIGGERED)\n", p_path );
        return;
    }

    png_init_io( pngPtr, fd );
    png_set_sig_bytes( pngPtr, 8 );
    png_read_info( pngPtr, infoPtr );

    m_width          = png_get_image_width( pngPtr, infoPtr );
    m_height         = png_get_image_height( pngPtr, infoPtr );
    png_byte clrType = png_get_color_type( pngPtr, infoPtr );
    png_byte bitDpt  = png_get_bit_depth( pngPtr, infoPtr );

    if( bitDpt == 16 ) png_set_strip_16( pngPtr );

    if( clrType == PNG_COLOR_TYPE_PALETTE ) png_set_strip_16( pngPtr );

    if( clrType == PNG_COLOR_TYPE_GRAY && bitDpt < 8 ) png_set_expand_gray_1_2_4_to_8( pngPtr );

    if( png_get_valid( pngPtr, infoPtr, PNG_INFO_tRNS ) ) png_set_tRNS_to_alpha( pngPtr );

    if( clrType == PNG_COLOR_TYPE_RGB || clrType == PNG_COLOR_TYPE_GRAY
        || clrType == PNG_COLOR_TYPE_PALETTE )
        png_set_filler( pngPtr, 0xFF, PNG_FILLER_AFTER );

    if( clrType == PNG_COLOR_TYPE_GRAY || clrType == PNG_COLOR_TYPE_GRAY_ALPHA )
        png_set_gray_to_rgb( pngPtr );

    png_read_update_info( pngPtr, infoPtr );

    png_bytep* row_pointers = (png_bytep*) malloc( sizeof( png_bytep ) * m_height );
    for( size_t y = 0; y < m_height; y++ )
        row_pointers[ y ] = (png_byte*) malloc( png_get_rowbytes( pngPtr, infoPtr ) );

    png_read_image( pngPtr, row_pointers );

    fclose( fd );

    m_pixels
        = std::vector<std::vector<pixel>>( m_width, std::vector<pixel>( m_height, { 0, 0, 0 } ) );

    for( size_t y = 0; y < m_height; y++ ) {
        png_bytep row = row_pointers[ y ];
        for( size_t x = 0; x < m_width; x++ ) {
            png_bytep px = &( row[ x * 4 ] );
            if( px[ 3 ] )
                m_pixels[ x ][ y ] = { px[ 0 ], px[ 1 ], px[ 2 ], 255 };
            else
                m_pixels[ x ][ y ] = { 0, 0, 0, 0 };
        }
    }
}

///////////////////////////////////////////////////////////////////////////////
// Bitmap - Operator overloadings
///////////////////////////////////////////////////////////////////////////////

pixel& bitmap::operator( )( size_t p_x, size_t p_y ) {
    if( p_x < 0 || p_x >= m_width || p_y < 0 || p_y >= m_height ) {
        //        std::fprintf( stderr, "No pixel at ( %lu | %lu )!\n", p_x, p_y );
        static pixel defPixel = { 0, 0, 0, 1 };
        return defPixel;
    }
    return m_pixels[ p_x ][ p_y ];
}
pixel bitmap::operator( )( size_t p_x, size_t p_y ) const {
    if( p_x < 0 || p_x >= m_width || p_y < 0 || p_y >= m_height ) {
        //        std::fprintf( stderr, "No pixel at ( %lu | %lu )!\n", p_x, p_y );
        static pixel defPixel = { 0, 0, 0, 1 };
        return defPixel;
    }
    return m_pixels[ p_x ][ p_y ];
}

///////////////////////////////////////////////////////////////////////////////
// Bitmap - PUBLIC functions
///////////////////////////////////////////////////////////////////////////////

//
// Write bitmap to file
//
int bitmap::writeToFile( const char* p_path ) const {
    FILE*       fd;
    png_structp pngPtr  = NULL;
    png_infop   infoPtr = NULL;
    size_t      x, y;
    png_byte**  rowPointers = NULL;

    int status = -1;

    int pixelSize = 4, depth = 8;

    fd = std::fopen( p_path, "wb" );
    if( !fd ) {
        std::fprintf( stderr, "Could not open file %s for writing.\n", p_path );
        goto FOPEN_FAILED;
    }

    pngPtr = png_create_write_struct( PNG_LIBPNG_VER_STRING, NULL, NULL, NULL );
    if( pngPtr == NULL ) {
        std::fprintf( stderr, "Could not create png write struct.\n" );
        status = -2;
        goto PNG_CREATE_WRITE_STRUCT_FAILED;
    }

    infoPtr = png_create_info_struct( pngPtr );
    if( pngPtr == NULL ) {
        std::fprintf( stderr, "Could not create png info struct.\n" );
        status = -3;
        goto PNG_CREATE_INFO_STRUCT_FAILED;
    }

    if( setjmp( png_jmpbuf( pngPtr ) ) ) {
        std::fprintf( stderr, "Png failure\n" );

        status = -4;
        goto PNG_FAILURE;
    }

    // Set image attributes

    png_set_IHDR( pngPtr, infoPtr, m_width, m_height, depth, PNG_COLOR_TYPE_RGB_ALPHA,
                  PNG_INTERLACE_NONE, PNG_COMPRESSION_TYPE_DEFAULT, PNG_FILTER_TYPE_DEFAULT );

    // Initialize rows of png

    rowPointers = (png_byte**) png_malloc( pngPtr, m_height * sizeof( png_bytep ) );
    for( y = 0; y < m_height; ++y ) {
        png_byte* row    = (png_byte*) png_malloc( pngPtr, m_width * pixelSize );
        rowPointers[ y ] = row;
        for( x = 0; x < m_width; ++x ) {
            auto px = operator( )( x, y );
            *row++  = px.m_red;
            *row++  = px.m_green;
            *row++  = px.m_blue;
            *row++  = px.m_transparent;
        }
    }

    // Write data to fd

    png_init_io( pngPtr, fd );
    png_set_rows( pngPtr, infoPtr, rowPointers );
    png_write_png( pngPtr, infoPtr, PNG_TRANSFORM_IDENTITY, NULL );

    status = 0;

    for( y = 0; y < m_height; ++y ) png_free( pngPtr, rowPointers[ y ] );
    png_free( pngPtr, rowPointers );

PNG_FAILURE:
PNG_CREATE_INFO_STRUCT_FAILED:
    png_destroy_write_struct( &pngPtr, &infoPtr );
PNG_CREATE_WRITE_STRUCT_FAILED:
    fclose( fd );
FOPEN_FAILED:
    return status;
}
