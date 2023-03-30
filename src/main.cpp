#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <math.h>

#include "../lib/logs/log.h"
#include "../lib/algorithm/algorithm.h"

#include <SFML/Graphics.hpp>

#include "maldenbrot.h"

static const size_t WND_SIZE = 500;

int main()
{
    sf::RenderWindow wnd(sf::VideoMode(WND_SIZE, WND_SIZE), "MOLDENBROT");

    maldenbrot paint = {};
    maldenbrot_ctor(&paint, WND_SIZE, WND_SIZE, 3.0 / WND_SIZE, -1.5, 1.5);

    while (wnd.isOpen())
    {
        maldenbrot_frame(&paint);
        maldenbrot_draw (&paint, &wnd);

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