//
//  tests.cpp
//  AudioSearchCLI
//
//  Created by Michael on 4/10/17.
//  Copyright © 2017 Michael Donovan. All rights reserved.
//

#include "libs/catch.hpp"

#include <vector>
#include "Distance.h"

using namespace Distance;
TEST_CASE ("DTW can be calculated with or without a locality constraint")
{
    vector<double> a = {1, 2, 3, 4, 5, 6, 7, 8, 9};
    vector<double> b = {5, 7, 2, 4, 5};
    double expected = 20.;
    REQUIRE (DTW (a, b) == expected);
    REQUIRE (DTW (a, b, 1) == expected);
    REQUIRE (DTW (a, b, .5) == expected);
    REQUIRE (DTW (a, b, .25) == expected);
    REQUIRE (DTW (a, b, .1) == expected);
}
