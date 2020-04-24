//
// Created by adrien on 09/01/20.
//

#include "map/Mountain.h"

Mountain::Mountain()
{
    std::random_device rd;  //Will be used to obtain a seed for the random number engine
    _gen = std::mt19937(rd());
    _height = 40.f;
}

float   Mountain::boundedRand()
{
    std::uniform_real_distribution<> dis(-_height / 2.f, _height * 1.5f);
    return (static_cast<float>(dis(_gen)));
}

