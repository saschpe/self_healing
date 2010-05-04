/* Test case for class array.
 *
 * The original author site is at: http://saschpe.wordpress.com/
 *
 * (C) Copyright Sascha Peilicke 2010.
 *
 * Distributed under the Boost Software License, Version 1.0. (See
 * accompanying file LICENSE_1_0.txt or copy at
 * http://www.boost.org/LICENSE_1_0.txt)
 *
 * 27 April 2010 - Initial Revision (Sascha Peilicke)
 *
 * May 4th, 2010
 */

#include <robust/array.hpp>

#include <iostream>
#include <cassert>

int main()
{
    robust::array<int, 64> a1;
    std::cout << "array 1: " << a1 << std::endl;

    assert(a1.at(0) == 0);
    assert(a1.at(a1.size() - 1) == 0);

    a1.front() = 3;
    a1.back() = 4;
    a1[2] = 4;
    a1.at(3) = 5;

    robust::array<int, 1> a2(42);
    std::cout << "array 2: " << a2 << std::endl;
    assert(a2.at(0) == 42);

    return 0;
}
