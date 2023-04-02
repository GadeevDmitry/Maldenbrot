#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <math.h>

#include "../lib/logs/log.h"
#include "../lib/algorithm/algorithm.h"

#include <SFML/Graphics.hpp>

#include "maldenbrot.h"
#include "maldenbrot_frame.h"
#include "maldenbrot_settings.h"

int main()
{
    bool (*frame_func)  (maldenbrot *const paint) = nullptr;
    sf::RenderWindow wnd(sf::VideoMode(WND_SIZE, WND_SIZE), "DEFAULT NAME");

#if   defined(CYCLE_SEPARATED)

    frame_func = maldenbrot_frame_cycle_separated;
    wnd.setTitle("CYCLE SEPARATED");

#elif defined(CYCLE_ALL_IN)

    frame_func = maldenbrot_frame_cycle_all_in;
    wnd.setTitle("CYCLE ALL IN");

#elif defined(SIMPLE)

    frame_func = maldenbrot_frame_simple;
    wnd.setTitle("SIMPLE");

#else

    frame_func = maldenbrot_frame_intrin;
    wnd.setTitle("INTRIN");

#endif

    maldenbrot paint = {};
    maldenbrot_ctor(&paint, WND_SIZE, WND_SIZE, SCALE / WND_SIZE, X_MIN, Y_MAX);

    while (wnd.isOpen())
    {
        (*frame_func)  (&paint);
        maldenbrot_draw(&paint, &wnd);

        sf::Event event;
        while (wnd.pollEvent(event))
        {
            if (event.type == sf::Event::Closed)     { wnd.close(); break; }
            if (event.type == sf::Event::KeyPressed)
            {
                #pragma GCC diagnostic push
                #pragma GCC diagnostic ignored "-Wswitch-enum"

                switch (event.key.code)
                {
                    case sf::Keyboard::Down:        maldenbrot_scale_more (&paint); break;
                    case sf::Keyboard::Up:          maldenbrot_scale_less (&paint); break;

                    case sf::Keyboard::W:           maldenbrot_shift_up   (&paint); break;
                    case sf::Keyboard::S:           maldenbrot_shift_down (&paint); break;
                    case sf::Keyboard::A:           maldenbrot_shift_left (&paint); break;
                    case sf::Keyboard::D:           maldenbrot_shift_right(&paint); break;

                    default:                                                        break;
                }

                #pragma GCC diagnostic pop
            }
        }
    }

    maldenbrot_dtor(&paint);
}