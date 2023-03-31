#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <math.h>
#include <time.h>
#include <immintrin.h>

#define LOG_NVERIFY

#include "../lib/logs/log.h"
#include "../lib/algorithm/algorithm.h"

#include <SFML/Graphics.hpp>

#include "maldenbrot.h"
#include "maldenbrot_frame.h"

static const double RADIUS_MAX = 100.0;

//--------------------------------------------------------------------------------------------------------------------------------
// DSL
//--------------------------------------------------------------------------------------------------------------------------------

#define $color  paint->pixels_color
#define $width  paint->pixels_width
#define $height paint->pixels_height

#define $scale paint->scale
#define $x_min paint->x_min
#define $y_max paint->y_max

//--------------------------------------------------------------------------------------------------------------------------------
// intrin
//--------------------------------------------------------------------------------------------------------------------------------

bool maldenbrot_frame_intrin(maldenbrot *const paint)
{
    log_verify(paint != nullptr, false);

    sf::Color main_color_sf = sf::Color::Red;
    unsigned  main_color    = main_color_sf.toInteger();

    __m256d vec_step_x     = _mm256_set_pd (3.0 * $scale, 2.0 * $scale, $scale, 0.0);
    __m256d vec_radius_max = _mm256_set1_pd(RADIUS_MAX);

    double cur_x = $x_min,
           cur_y = $y_max;

    for (size_t pixels_y = 0; pixels_y     < $height; pixels_y += 1) {
    for (size_t pixels_x = 0; pixels_x + 3 < $width ; pixels_x += 4)
        {
            __m256d vec_cur_y = _mm256_set1_pd(cur_y);
            __m256d vec_cur_x = _mm256_add_pd(_mm256_set1_pd(cur_x), vec_step_x);

            __m256d vec_i_y = vec_cur_y;
            __m256d vec_i_x = vec_cur_x;

            __m256i   vec_opacity = _mm256_setzero_si256();
            unsigned char opacity = 255;
            do
            {
                __m256d vec_square_y = _mm256_mul_pd(     vec_i_y,      vec_i_y);
                __m256d vec_square_x = _mm256_mul_pd(     vec_i_x,      vec_i_x);
                __m256d vec_radius_2 = _mm256_add_pd(vec_square_x, vec_square_y);

                for (int i = 0; i < 4; ++i) vec_opacity[i] = (vec_radius_2[i] > 100 && !vec_opacity[i]) ? opacity : vec_opacity[i];

                //bool mask = (vec_opacity[0] && vec_opacity[1]) && (vec_opacity[2] && vec_opacity[3]);
                //if  (mask) break;

                __m256d mask  = _mm256_cmp_pd(vec_radius_2, vec_radius_max, _CMP_LE_OS);
                if (_mm256_testz_si256((__m256i) mask, (__m256i) mask) == 1) break;

                vec_i_y = _mm256_mul_pd(     vec_i_y,      vec_i_x); vec_i_y = _mm256_add_pd(vec_i_y, vec_i_y); vec_i_y = _mm256_add_pd(vec_i_y, vec_cur_y);
                vec_i_x = _mm256_sub_pd(vec_square_x, vec_square_y);                                            vec_i_x = _mm256_add_pd(vec_i_x, vec_cur_x);
 
                opacity--;
            }
            while (opacity != 0);
            for   (unsigned i = 0; i < 4; ++i) $color[pixels_y * $width + pixels_x + i] = main_color ^ (unsigned) vec_opacity[i];

            cur_x += 4 * $scale;
        }
        cur_y -= $scale;
        cur_x  = $x_min;
    }

    return true;
}

//--------------------------------------------------------------------------------------------------------------------------------
// cycle_separated
//--------------------------------------------------------------------------------------------------------------------------------

#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wunused-function"

static void set1_epi64(long   *const dest, const long          src);
static void set1_pd   (double *const dest, const double        src);
static void add1_pd   (double *const dest, const double        src);
static void  mov_pd   (double *const dest, const double *const src);
static void  add_pd   (double *const dest, const double *const src);
static void  sub_pd   (double *const dest, const double *const src);
static void  mul_pd   (double *const dest, const double *const src);
static void mul1_pd   (double *const dest, const double        src);

static void set_pd(double *const dest, const double f0, const double f1, const double f2, const double f3);

//--------------------------------------------------------------------------------------------------------------------------------

bool maldenbrot_frame_cycle_separated(maldenbrot *const paint)
{
    log_verify(paint != nullptr, false);

    sf::Color main_color_sf = sf::Color::Red;
    unsigned  main_color    = main_color_sf.toInteger();

    double vec_step_x    [4] = {}; set_pd (vec_step_x    , 0.0, $scale, 2.0 * $scale, 3.0 * $scale);
    double vec_radius_max[4] = {}; set1_pd(vec_radius_max, RADIUS_MAX);

    double cur_x = $x_min,
           cur_y = $y_max;

    for (size_t pixels_y = 0; pixels_y     < $height; pixels_y += 1) {
    for (size_t pixels_x = 0; pixels_x + 3 < $width ; pixels_x += 4)
        {
            double vec_cur_y[4] = {}; set1_pd(vec_cur_y, cur_y);
            double vec_cur_x[4] = {}; set1_pd(vec_cur_x, cur_x); add_pd(vec_cur_x, vec_step_x);

            double vec_i_y  [4] = {}; mov_pd(vec_i_y, vec_cur_y);
            double vec_i_x  [4] = {}; mov_pd(vec_i_x, vec_cur_x);

            unsigned long vec_opacity[4] =  {}; set1_epi64((long *) vec_opacity, 0);
            unsigned char     opacity    = 255;
            do
            {
                double vec_square_y[4] = {}; mov_pd(vec_square_y,      vec_i_y); mul_pd(vec_square_y,      vec_i_y);
                double vec_square_x[4] = {}; mov_pd(vec_square_x,      vec_i_x); mul_pd(vec_square_x,      vec_i_x);
                double vec_radius_2[4] = {}; mov_pd(vec_radius_2, vec_square_x); add_pd(vec_radius_2, vec_square_y);

                for (int i = 0; i < 4; ++i) vec_opacity[i] = (vec_radius_2[i] > 100 && !vec_opacity[i]) ? opacity : vec_opacity[i];

                bool mask = (vec_opacity[0] && vec_opacity[1]) && (vec_opacity[2] && vec_opacity[3]);
                if  (mask) break;

                mul_pd (vec_i_y,      vec_i_x); mul1_pd(vec_i_y,          2.0); add_pd(vec_i_y, vec_cur_y);
                mov_pd (vec_i_x, vec_square_x); sub_pd (vec_i_x, vec_square_y); add_pd(vec_i_x, vec_cur_x);
 
                opacity--;
            }
            while (opacity != 0);
            for   (unsigned i = 0; i < 4; ++i) $color[pixels_y * $width + pixels_x + i] = main_color ^ (unsigned) vec_opacity[i];

            cur_x += 4 * $scale;
        }
        cur_y -= $scale;
        cur_x  = $x_min;
    }

    return true;
}

//--------------------------------------------------------------------------------------------------------------------------------

static void set1_epi64(long   *const dest, const long          src) { for (int i = 0; i < 4; ++i) dest[i]  = src   ; }
static void set1_pd   (double *const dest, const double        src) { for (int i = 0; i < 4; ++i) dest[i]  = src   ; }
static void add1_pd   (double *const dest, const double        src) { for (int i = 0; i < 4; ++i) dest[i]  = src   ; }
static void  mov_pd   (double *const dest, const double *const src) { for (int i = 0; i < 4; ++i) dest[i]  = src[i]; }
static void  add_pd   (double *const dest, const double *const src) { for (int i = 0; i < 4; ++i) dest[i] += src[i]; }
static void  sub_pd   (double *const dest, const double *const src) { for (int i = 0; i < 4; ++i) dest[i] -= src[i]; }
static void  mul_pd   (double *const dest, const double *const src) { for (int i = 0; i < 4; ++i) dest[i] *= src[i]; }
static void mul1_pd   (double *const dest, const double        src) { for (int i = 0; i < 4; ++i) dest[i] *= src   ; }

static void set_pd(double *const dest, const double f0, const double f1, const double f2, const double f3)
{
    dest[0] = f0; dest[1] = f1; dest[2] = f2; dest[3] = f3;
}

#pragma GCC diagnostic pop

//--------------------------------------------------------------------------------------------------------------------------------
// cycle_all_in
//--------------------------------------------------------------------------------------------------------------------------------

bool maldenbrot_frame_cycle_all_in(maldenbrot *const paint)
{
    log_verify(paint != nullptr, false);

    sf::Color main_color_sf = sf::Color::Red;
    unsigned  main_color    = main_color_sf.toInteger();

    double cur_x = $x_min,
           cur_y = $y_max;

    for (size_t pixels_y = 0; pixels_y     < $height; pixels_y += 1) {
    for (size_t pixels_x = 0; pixels_x + 3 < $width ; pixels_x += 4)
        {
            double   vec_cur_y  [4] = {cur_y, cur_y,          cur_y,              cur_y             };
            double   vec_cur_x  [4] = {cur_x, cur_x + $scale, cur_x + 2 * $scale, cur_x + 3 * $scale};

            double   vec_i_y    [4] = {}; for (int i = 0; i < 4; ++i) { vec_i_y[i] = vec_cur_y[i]; }
            double   vec_i_x    [4] = {}; for (int i = 0; i < 4; ++i) { vec_i_x[i] = vec_cur_x[i]; }

            unsigned vec_opacity[4] = {0, 0, 0, 0};

            unsigned char opacity   = 255;
            do
            {
                double vec_square_y[4] = {}; for (int i = 0; i < 4; ++i) { vec_square_y[i] = vec_i_y     [i] * vec_i_y     [i]; }
                double vec_square_x[4] = {}; for (int i = 0; i < 4; ++i) { vec_square_x[i] = vec_i_x     [i] * vec_i_x     [i]; }
                double vec_radius_2[4] = {}; for (int i = 0; i < 4; ++i) { vec_radius_2[i] = vec_square_y[i] + vec_square_x[i]; }

                for (int i = 0; i < 4; ++i) { vec_opacity[i] = (vec_radius_2[i] > 100 && !vec_opacity[i]) ? opacity : vec_opacity[i]; }

                bool mask = (vec_opacity[0] && vec_opacity[1]) && (vec_opacity[2] && vec_opacity[3]);
                if  (mask) break;

                for (int i = 0; i < 4; ++i) { vec_i_y[i] = 2 *  vec_i_x[i] *      vec_i_y[i] + vec_cur_y[i]; }
                for (int i = 0; i < 4; ++i) { vec_i_x[i] = vec_square_x[i] - vec_square_y[i] + vec_cur_x[i]; }
        
                opacity--;
            }
            while (opacity != 0);
            for   (unsigned i = 0; i < 4; ++i) $color[pixels_y * $width + pixels_x + i] = main_color ^ vec_opacity[i];

            cur_x += 4 * $scale;
        }
        cur_y -= $scale;
        cur_x  = $x_min;
    }

    return true;
}

//--------------------------------------------------------------------------------------------------------------------------------
// simple
//--------------------------------------------------------------------------------------------------------------------------------

bool maldenbrot_frame_simple(maldenbrot *const paint)
{
    log_verify(paint != nullptr, false);

    sf::Color main_color_sf = sf::Color::Red;
    unsigned  main_color    = main_color_sf.toInteger();

    double cur_x = $x_min,
           cur_y = $y_max;

    for (size_t pixels_x = 0; pixels_x < $width ; ++pixels_x) {
    for (size_t pixels_y = 0; pixels_y < $height; ++pixels_y)
        {
            double x_i = cur_x,
                   y_i = cur_y;

            unsigned char opacity = 255;
            do
            {
                double x_square = x_i * x_i;
                double y_square = y_i * y_i;
                double radius_2 = x_square + y_square;

                if (radius_2 > 100) break;

                y_i = 2 * x_i * y_i + cur_y;
                x_i = x_square - y_square + cur_x;

                opacity--;
            }
            while (opacity != 0);
            $color[pixels_y * $width + pixels_x] = main_color ^ opacity;

            cur_y -= $scale;
        }
        cur_x += $scale;
        cur_y  = $y_max;
    }

    return true;
}
