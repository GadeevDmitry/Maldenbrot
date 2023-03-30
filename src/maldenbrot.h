#ifndef MALDENBROT_H
#define MALDENBROT_H

struct maldenbrot
{
    unsigned *pixels_color;
    size_t    pixels_width;
    size_t    pixels_height;

    double scale;
    double x_min, y_max;
};

bool maldenbrot_ctor(maldenbrot *const paint, const size_t pixels_width,
                                              const size_t pixels_height, const double scale,
                                                                          const double x_min,
                                                                          const double y_max);
void maldenbrot_dtor(maldenbrot *const paint);
//--------------------------------------------------------------------------------------------------------------------------------
void maldenbrot_scale_more (maldenbrot *const paint);
void maldenbrot_scale_less (maldenbrot *const paint);

void maldenbrot_shift_up   (maldenbrot *const paint);
void maldenbrot_shift_down (maldenbrot *const paint);
void maldenbrot_shift_left (maldenbrot *const paint);
void maldenbrot_shift_right(maldenbrot *const paint);
//--------------------------------------------------------------------------------------------------------------------------------
bool maldenbrot_frame(maldenbrot *const paint);
bool maldenbrot_draw (maldenbrot *const paint, sf::RenderWindow *const wnd);

#endif //MALDENBROT_H