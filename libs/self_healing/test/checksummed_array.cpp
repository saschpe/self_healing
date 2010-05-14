/* Test case for class checksummed_array.
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
 */

#include <boost/self_healing/checksummed_array.hpp>

#include <iostream>
#include <cassert>
#include <memory>

int main()
{
    std::cout << "testing class boost::self_healing::checksummed_array<>" << std::endl;

    boost::self_healing::checksummed_array<int, 16> a;
    std::cout << "checksummed_array 1: " << a << std::endl;

    assert(a.at(0) == 0);
    assert(a.at(a.size() - 1) == 0);

    std::cout << "testing checksummed_array<>::reference assignment" << std::endl;
    a.front() = 3;
    a.back() = 4;
    a[2] = 4;
    a.at(3) = 5;
    a.at(13) = a.at(12);

    std::cout << "testing checksummed_array<>::reference comparison" << std::endl;
    assert(a.front() == 3);
    assert(a.front() != 5);
    assert(a.front() >= 2);
    std::cout << (a.front() <= 2) << std::endl;
    std::cout << (a.front() > 3) << std::endl;
    std::cout << (a.front() < 3) << std::endl;

    std::cout << "testing checksummed_array<>::reference arithmetic operations" << std::endl;
    std::cout << a.front() + 2 << std::endl;
    std::cout << a.front() - 2 << std::endl;
    std::cout << a.front() * 2 << std::endl;
    std::cout << a.front() / 2 << std::endl;
    std::cout << a.front() % 2 << std::endl;

    std::cout << "testing checksummed_array<>::reference arithmetic assignment operations" << std::endl;
    std::cout << a[0];
    std::cout << "a1[6] += 1: " << (a[6] += 1) << std::endl;
    std::cout << (a[7] -= 1) << std::endl;
    std::cout << (a[7] *= 1) << std::endl;
    std::cout << (a[7] /= 1) << std::endl;
    std::cout << (a[7] %= 1) << std::endl;

    std::cout << "testing checksummed_array<>::reference increment and decrement operations" << std::endl;
    a[8]++;
    ++a[9];
    a[10]--;
    --a[11];

    std::cout << "checksummed_array 1:  ";
    std::copy(a.begin(), a.end(), std::ostream_iterator<int>(std::cout, ","));
    std::cout << std::endl;

    boost::self_healing::checksummed_array<int, 8> *pa = new boost::self_healing::checksummed_array<int, 8>(3);
    std::cout << "pointer to checksummed_array: " << *pa << std::endl;

    std::auto_ptr<boost::self_healing::checksummed_array<int, 8> > apa(pa);
    std::cout << "auto_ptr to checksummed_array size: " << apa->size() << std::endl;
    std::cout << "auto_ptr to checksummed_array: " << *apa << std::endl;

    return 0;
}
