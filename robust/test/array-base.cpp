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
    robust::array<int, 16> a1;
    std::cout << "array 1: " << a1 << std::endl;

    assert(a1.at(0) == 0);
    assert(a1.at(a1.size() - 1) == 0);

    int i = 3;
    robust::array<int, 1>::reference r(i);

    // testing class robust::array<>::reference assignment
    a1.front() = 3;
    a1.back() = 4;
    a1[2] = 4;
    a1.at(3) = 5;

    // testing class robust::array<>::reference comparison
    assert(a1.front() == 3);
    assert(a1.front() != 5);
    assert(a1.front() >= 2);
    std::cout << (a1.front() <= 2) << std::endl;
    std::cout << (a1.front() > 3) << std::endl;
    std::cout << (a1.front() < 3) << std::endl;

    // testing class robust::array<>::reference arithmetic operations
    std::cout << a1.front() + 2 << std::endl;
    std::cout << a1.front() - 2 << std::endl;
    std::cout << a1.front() * 2 << std::endl;
    std::cout << a1.front() / 2 << std::endl;
    std::cout << a1.front() % 2 << std::endl;

    std::cout << a1[0];
    /*std::cout << "a1[6] += 1: " << (a1[6] += 1) << " and a1[6]: " << a1[6].value() << std::endl;
    std::cout << (a1[7] -= 1) << std::endl;
    std::cout << (a1[7] *= 1) << std::endl;
    std::cout << (a1[7] /= 1) << std::endl;
    std::cout << (a1[7] %= 1) << std::endl;*/

    // testing class robust::array<>::reference increment and decrement operations
    a1[8]++;
    ++a1[9];
    a1[10]--;
    --a1[11];

    // testing iterators
    std::cout << "array 1:  ";
    std::copy(a1.begin(), a1.end(), std::ostream_iterator<int>(std::cout, ","));
    std::cout << std::endl;

    for (robust::array<int, 16>::iterator it = a1.begin(); it != a1.end(); it++) {
        *it += 1;
    }

    std::cout << "array 1:  ";
    for (robust::array<int, 16>::const_reverse_iterator rit = a1.rbegin(); rit != a1.rend(); rit++) {
        std:: cout << *rit << ",";
    }
    std::cout << std::endl;

    std::cout << "array 1: " << a1 << std::endl;

    return 0;
}
