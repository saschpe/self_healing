
// Copyright Sascha Peilicke <sasch.pe@gmx.de> 2010.
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
//  http://www.boost.org/LICENSE_1_0.txt)

#include "../array.h"

#include <iostream>
#include <cassert>

using namespace std;

int main()
{
    robust::chunk<int> c1;
    cout << "chunk 1: " << c1 << endl;
    assert(c1.at(0) == 0);
    assert(c1.at(c1.size() - 1) == 0);
    assert(c1.size() == sizeof(int) * 4);
    assert(c1.is_valid());

    robust::chunk<int, 1> c2(42);
    cout << "chunk 2: " << c2 << endl;
    assert(c2.at(0) == 42);
    assert(c2.size() == sizeof(int));
    assert(c2.is_valid());

    return 0;
}
