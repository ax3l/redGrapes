/* Copyright 2019 Michael Sippel
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

#include <iostream>
#include <thread>
#include <chrono>

#include <redGrapes/redGrapes.hpp>
#include <redGrapes/resource/ioresource.hpp>

int main(void)
{
    spdlog::set_level( spdlog::level::trace );

    redGrapes::init();
    
    redGrapes::IOResource< int > a, b;

    for(int i = 0; i < 1; ++i)
    {
        redGrapes::emplace_task(
            []( auto a )
            {
                std::cout << "Write to A" << std::endl;
                std::this_thread::sleep_for(std::chrono::seconds(1));
                *a = 4;
                std::cout << "Write A done" << std::endl;
            },
            a.write()
        );

        redGrapes::emplace_task(
            []( auto a )
            {
                std::cout << "Read A: " << *a << std::endl;
                std::this_thread::sleep_for(std::chrono::seconds(1));
            },
            a.read()
        );

        redGrapes::emplace_task(
            []( auto b )
            {
                std::cout << "Write to B" << std::endl;
                std::this_thread::sleep_for(std::chrono::seconds(2));
                *b = 7;
                std::cout << "Write B done" << std::endl;
            },
            b.write()
        );

        redGrapes::emplace_task(
            []( auto a, auto b )
            {
                std::cout << "Read A & B: " << *a << ", " << *b << std::endl;
                std::this_thread::sleep_for(std::chrono::seconds(1));
            },
            a.read(),
            b.read()
        );
    }

    redGrapes::finalize();
    
    return 0;
}

