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

    __m256d vec_step_x     = _mm256_set_pd (3.0 * $scale, 2.0 * $scale, $scale, 0.0);
    __m256d vec_radius_max = _mm256_set1_pd(RADIUS_MAX);

    double cur_x = $x_min,
           cur_y = $y_max;

    for (size_t pixels_y = 0; pixels_y     < $height; pixels_y += 1) { size_t offset = pixels_y * $width;
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

                __m256i mask  = (__m256i) _mm256_cmp_pd(vec_radius_2, vec_radius_max, _CMP_LE_OS);
                vec_opacity   = _mm256_sub_epi64(vec_opacity, mask);

                if (_mm256_testz_si256(mask, mask) == 1) break;

                vec_i_y = _mm256_mul_pd(     vec_i_y,      vec_i_x); vec_i_y = _mm256_add_pd(vec_i_y, vec_i_y); vec_i_y = _mm256_add_pd(vec_i_y, vec_cur_y);
                vec_i_x = _mm256_sub_pd(vec_square_x, vec_square_y);                                            vec_i_x = _mm256_add_pd(vec_i_x, vec_cur_x);

                opacity--;
            }
            while (opacity != 0);
            for   (unsigned i = 0; i < 4; ++i) $color[offset + pixels_x + i] = (unsigned) vec_opacity[i];

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

static inline void set1_epi64(size_t *const dest, const size_t src) { for (int i = 0; i < 4; ++i) dest[i]  = src; }
static inline void set1_pd   (double *const dest, const double src) { for (int i = 0; i < 4; ++i) dest[i]  = src; }
static inline void mul1_pd   (double *const dest, const double src) { for (int i = 0; i < 4; ++i) dest[i] *= src; }

static inline void  mov_pd   (double *const dest, const double *const src                    ) { for (int i = 0; i < 4; ++i) dest[i] = src [i];        }
static inline void add1_pd   (double *const dest, const double *const src1, const double src2) { for (int i = 0; i < 4; ++i) dest[i] = src1[i] + src2; }

static inline void  sub_epi64(size_t *const dest, const size_t *const src1, const size_t *const src2) { for (int i = 0; i < 4; ++i) dest[i] = src1[i] - src2[i]; }
static inline void  add_pd   (double *const dest, const double *const src1, const double *const src2) { for (int i = 0; i < 4; ++i) dest[i] = src1[i] + src2[i]; }
static inline void  sub_pd   (double *const dest, const double *const src1, const double *const src2) { for (int i = 0; i < 4; ++i) dest[i] = src1[i] - src2[i]; }
static inline void  mul_pd   (double *const dest, const double *const src1, const double *const src2) { for (int i = 0; i < 4; ++i) dest[i] = src1[i] * src2[i]; }

static inline void set_pd    (double *const dest, const double f0,
                                                  const double f1,
                                                  const double f2,
                                                  const double f3) { dest[0] = f0; dest[1] = f1; dest[2] = f2; dest[3] = f3; }

static inline void cmp_le    (size_t *const dest, const double *const src1, const double src2)
{
    for (int i = 0; i < 4; ++i) dest[i] = (src1[i] <= src2) ? ~0UL : 0UL;
}

static inline bool testz_epi64(size_t *const src)
{
    bool ret = true;
    for (int i = 0; i < 4; ++i) ret = (src[i] == 0UL) ? ret : false;

    return ret;
}

#pragma GCC diagnostic pop

//--------------------------------------------------------------------------------------------------------------------------------

bool maldenbrot_frame_cycle_separated(maldenbrot *const paint)
{
    log_verify(paint != nullptr, false);

    double vec_step_x    [4] = {}; set_pd (vec_step_x    , 0.0, $scale, 2.0 * $scale, 3.0 * $scale);
    double vec_radius_max[4] = {}; set1_pd(vec_radius_max, RADIUS_MAX);

    double cur_x = $x_min,
           cur_y = $y_max;

    for (size_t pixels_y = 0; pixels_y     < $height; pixels_y += 1) { size_t offset = pixels_y * $width;
    for (size_t pixels_x = 0; pixels_x + 3 < $width ; pixels_x += 4)
        {
            double vec_cur_y[4] = {}; set1_pd(vec_cur_y, cur_y);
            double vec_cur_x[4] = {}; add1_pd(vec_cur_x, vec_step_x, cur_x);

            double vec_i_y  [4] = {}; mov_pd(vec_i_y, vec_cur_y);
            double vec_i_x  [4] = {}; mov_pd(vec_i_x, vec_cur_x);

            size_t    vec_opacity[4] =  {}; set1_epi64(vec_opacity, 0);
            unsigned char opacity    = 255;
            do
            {
                double vec_square_y[4] = {}; mul_pd(vec_square_y,      vec_i_y,      vec_i_y);
                double vec_square_x[4] = {}; mul_pd(vec_square_x,      vec_i_x,      vec_i_x);
                double vec_radius_2[4] = {}; add_pd(vec_radius_2, vec_square_x, vec_square_y);

                size_t   *mask = (size_t *) vec_radius_2;
                cmp_le   (mask       , vec_radius_2, RADIUS_MAX);
                sub_epi64(vec_opacity, vec_opacity , mask);

                if  (testz_epi64(mask)) break;

                mul_pd(vec_i_y, vec_i_y,      vec_i_x     ); add_pd(vec_i_y, vec_i_y, vec_i_y); add_pd(vec_i_y, vec_i_y, vec_cur_y);
                sub_pd(vec_i_x, vec_square_x, vec_square_y);                                    add_pd(vec_i_x, vec_i_x, vec_cur_x);

                opacity--;
            }
            while (opacity != 0);
            for   (unsigned i = 0; i < 4; ++i) $color[offset + pixels_x + i] = (unsigned) vec_opacity[i];

            cur_x += 4 * $scale;
        }
        cur_y -= $scale;
        cur_x  = $x_min;
    }

    return true;
}

//--------------------------------------------------------------------------------------------------------------------------------
// cycle_all_in
//--------------------------------------------------------------------------------------------------------------------------------

bool maldenbrot_frame_cycle_all_in(maldenbrot *const paint)
{
    log_verify(paint != nullptr, false);

    double cur_x = $x_min,
           cur_y = $y_max;

    for (size_t pixels_y = 0; pixels_y     < $height; pixels_y += 1) { size_t offset = pixels_y * $width;
    for (size_t pixels_x = 0; pixels_x + 3 < $width ; pixels_x += 4)
        {
            double   vec_cur_y  [4] = {cur_y, cur_y,          cur_y,              cur_y             };
            double   vec_cur_x  [4] = {cur_x, cur_x + $scale, cur_x + 2 * $scale, cur_x + 3 * $scale};

            double   vec_i_y    [4] = {}; for (int i = 0; i < 4; ++i) { vec_i_y[i] = vec_cur_y[i]; }
            double   vec_i_x    [4] = {}; for (int i = 0; i < 4; ++i) { vec_i_x[i] = vec_cur_x[i]; }

            size_t    vec_opacity[4] = {0, 0, 0, 0};
            unsigned char opacity    = 255;
            do
            {
                double vec_square_y[4] = {}; for (int i = 0; i < 4; ++i) { vec_square_y[i] = vec_i_y     [i] * vec_i_y     [i]; }
                double vec_square_x[4] = {}; for (int i = 0; i < 4; ++i) { vec_square_x[i] = vec_i_x     [i] * vec_i_x     [i]; }
                double vec_radius_2[4] = {}; for (int i = 0; i < 4; ++i) { vec_radius_2[i] = vec_square_x[i] + vec_square_y[i]; }

                size_t *mask = (size_t *) vec_radius_2;
                for (int i = 0; i < 4; ++i) mask       [i]  = (vec_radius_2[i] <= RADIUS_MAX) ? ~0UL : 0UL;
                for (int i = 0; i < 4; ++i) vec_opacity[i] -= mask[i];

                bool is_break = true;
                for (int i = 0; i < 4; ++i) is_break = (mask[i] == 0) ? is_break : false;
                if  (is_break) break;

                for (int i = 0; i < 4; ++i) { vec_i_y[i] = 2 * (vec_i_x[i] *      vec_i_y[i]) + vec_cur_y[i]; }
                for (int i = 0; i < 4; ++i) { vec_i_x[i] = vec_square_x[i] - vec_square_y[i]  + vec_cur_x[i]; }

                opacity--;
            }
            while (opacity != 0);
            for   (unsigned i = 0; i < 4; ++i) $color[offset + pixels_x + i] = (unsigned) vec_opacity[i];

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

    double cur_x = $x_min,
           cur_y = $y_max;

    for (size_t pixels_y = 0; pixels_y < $height; ++pixels_y) { size_t offset = pixels_y * $width;
    for (size_t pixels_x = 0; pixels_x < $width ; ++pixels_x)
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

                y_i = 2 * x_i * y_i       + cur_y;
                x_i = x_square - y_square + cur_x;

                opacity--;
            }
            while (opacity != 0);
            $color[offset + pixels_x] = ~opacity;

            cur_x += $scale;
        }
        cur_y -= $scale;
        cur_x  = $x_min;
    }

    return true;
}
