//
// Copyright ( c ) 2014 Philip Wellnitz
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

#pragma once
#include <vector>

typedef uint8_t  u8;
typedef uint16_t u16;

/**
 * @brief Struct to store the RGB values of a pixel.
 */
struct pixel {
    u8 m_red;         ///< R value
    u8 m_green;       ///< G value
    u8 m_blue;        ///< B value
    u8 m_transparent; ///< pixel is transparent

    constexpr pixel( u8 p_red, u8 p_green, u8 p_blue, u8 p_transparent = 0 )
        : m_red( p_red ), m_green( p_green ), m_blue( p_blue ), m_transparent( p_transparent ) {
    }

    constexpr auto operator<=>( const pixel& p_other ) const = default;

    constexpr pixel operator^=( const pixel& p_other ) {
        u16 tp        = 510 - ( this->m_transparent + p_other.m_transparent );
        u16 ttp       = 255 - ( this->m_transparent );
        u16 otp       = 255 - ( p_other.m_transparent );
        this->m_red   = this->m_red * ttp / tp + p_other.m_red * otp / tp;
        this->m_green = this->m_green * ttp / tp + p_other.m_green * otp / tp;
        this->m_blue  = this->m_blue * ttp / tp + p_other.m_blue * otp / tp;
        return *this;
    }

    constexpr pixel operator&=( const pixel& p_other ) {
        u16 tp        = 255;
        u16 otp       = 255 - ( p_other.m_transparent );
        u16 ttp       = p_other.m_transparent;
        this->m_red   = this->m_red * ttp / tp + p_other.m_red * otp / tp;
        this->m_green = this->m_green * ttp / tp + p_other.m_green * otp / tp;
        this->m_blue  = this->m_blue * ttp / tp + p_other.m_blue * otp / tp;
        return *this;
    }

    constexpr pixel operator+=( const pixel& p_other ) {
        this->m_red += p_other.m_red;
        this->m_green += p_other.m_green;
        this->m_blue += p_other.m_blue;
        return *this;
    }

    constexpr pixel operator-=( const pixel& p_other ) {
        this->m_red -= p_other.m_red;
        this->m_green -= p_other.m_green;
        this->m_blue -= p_other.m_blue;
        return *this;
    }

    constexpr pixel operator*=( u8 p_scale ) {
        this->m_red *= p_scale;
        this->m_green *= p_scale;
        this->m_blue *= p_scale;
        return *this;
    }

    constexpr pixel operator/=( u8 p_scale ) {
        this->m_red /= p_scale;
        this->m_green /= p_scale;
        this->m_blue /= p_scale;
        return *this;
    }

    friend constexpr pixel operator^( pixel p_lhs, const pixel& p_rhs ) {
        p_lhs ^= p_rhs;
        return p_lhs;
    }

    friend constexpr pixel operator&( pixel p_lhs, const pixel& p_rhs ) {
        p_lhs &= p_rhs;
        return p_lhs;
    }

    friend constexpr pixel operator+( pixel p_lhs, const pixel& p_rhs ) {
        p_lhs += p_rhs;
        return p_lhs;
    }

    friend constexpr pixel operator-( pixel p_lhs, const pixel& p_rhs ) {
        p_lhs -= p_rhs;
        return p_lhs;
    }

    friend constexpr pixel operator*( pixel p_lhs, u8 p_rhs ) {
        p_lhs *= p_rhs;
        return p_lhs;
    }

    friend constexpr pixel operator/( pixel p_lhs, u8 p_rhs ) {
        p_lhs /= p_rhs;
        return p_lhs;
    }
};

#define SCALE( x, mx ) \
    ( ( ( x ) < 0 ) ? 0 : ( (int) ( 256.0 * ( (double) ( x ) / (double) ( mx ) ) ) ) )

/**
 * @brief Struct to hold a bitmap.
 */
struct bitmap {
  private:
    std::vector<std::vector<pixel>> m_pixels; ///< The raw data
  public:
    size_t m_width;  ///< The image width
    size_t m_height; ///< The image height
    bitmap( size_t p_width = 0, size_t p_height = 0 );
    bitmap( const char* p_path );

    pixel& operator( )( size_t p_x, size_t p_y );
    pixel  operator( )( size_t p_x, size_t p_y ) const;

    /**
     * @param p_path String containing the path to the file to write the file to
     * @return Non-zero if an error occured.
     */
    int writeToFile( const char* p_path ) const;
};
