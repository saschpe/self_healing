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
 *
 * May 4th, 2010
 */

#include <boost/robust/checksummed_array.hpp>

#include <iostream>
#include <cassert>
#include <memory>

int main()
{
    boost::robust::checksummed_array<int, 16> a;
    std::cout << "checksummed_array 1: " << a << std::endl;

    assert(a.at(0) == 0);
    assert(a.at(a.size() - 1) == 0);

    int i = 3;
    //TODO: move robust::reference<> test code into own test case
    boost::robust::reference<int> rr(i);
    int j = 1 + *rr + 3;
    int k = 1 + rr + 3;
    boost::robust::checksummed_array<int, 16>::reference ar(i);
    boost::robust::checksummed_array<int, 16>::const_reference car(i);

    std::cout << "sizeof(robust::reference<int>): " << sizeof(rr) << std::endl;

    std::cout << "testing class robust::checksummed_array<>::reference assignment" << std::endl;
    a.front() = 3;
    a.back() = 4;
    a[2] = 4;
    a.at(3) = 5;
    a.at(13) = a.at(12);

    std::cout << "testing class robust::checksummed_array<>::reference comparison" << std::endl;
    assert(a.front() == 3);
    assert(a.front() != 5);
    assert(a.front() >= 2);
    std::cout << (a.front() <= 2) << std::endl;
    std::cout << (a.front() > 3) << std::endl;
    std::cout << (a.front() < 3) << std::endl;

    std::cout << "testing class robust::checksummed_array<>::reference arithmetic operations" << std::endl;
    std::cout << a.front() + 2 << std::endl;
    std::cout << a.front() - 2 << std::endl;
    std::cout << a.front() * 2 << std::endl;
    std::cout << a.front() / 2 << std::endl;
    std::cout << a.front() % 2 << std::endl;

    std::cout << "testing class robust::checksummed_array<>::reference arithmetic assignment operations" << std::endl;
    std::cout << a[0];
    std::cout << "a1[6] += 1: " << (a[6] += 1) << std::endl;
    std::cout << (a[7] -= 1) << std::endl;
    std::cout << (a[7] *= 1) << std::endl;
    std::cout << (a[7] /= 1) << std::endl;
    std::cout << (a[7] %= 1) << std::endl;

    std::cout << "testing class robust::checksummed_array<>::reference increment and decrement operations" << std::endl;
    a[8]++;
    ++a[9];
    a[10]--;
    --a[11];

    std::cout << "testing iterators" << std::endl;
    std::cout << "checksummed_array 1:  ";
    std::copy(a.begin(), a.end(), std::ostream_iterator<int>(std::cout, ","));
    std::cout << std::endl;

    for (boost::robust::checksummed_array<int, 16>::iterator it = a.begin(); it != a.end(); it++) {
        *it += 1;
    }

    std::cout << "testint iterator value assignment" << std::endl;
    boost::robust::checksummed_array<int, 16>::iterator tmp_it = a.begin();
    *tmp_it = 99;

    boost::robust::checksummed_array<int, 16>::reverse_iterator tmp_rit = a.rbegin();
    //TODO: fix this, the functor is not called here !!!
    *tmp_rit = 99;

    std::cout << "testing const iterators" << std::endl;
    const int* cit = a.begin();
    boost::robust::checksummed_array<int, 16>::const_iterator tmp_cit = a.begin();
    boost::robust::checksummed_array<int, 16>::const_reverse_iterator tmp_crit = a.rbegin();

    std::cout << "checksummed_array 1:  ";
    for (std::reverse_iterator<boost::robust::checksummed_array<int, 16>::iterator> rit = a.rbegin(); rit != a.rend(); rit++) {
        int i = *rit;
        std:: cout << i << ",";
    }
    std::cout << std::endl;

    std::cout << "checksummed_array 1: " << a << std::endl;

    boost::robust::checksummed_array<int, 8> *pa = new boost::robust::checksummed_array<int, 8>(3);
    std::cout << "pointer to checksummed_array: " << *pa << std::endl;

    std::auto_ptr<boost::robust::checksummed_array<int, 8> > apa(pa);
    std::cout << "auto_ptr to checksummed_array size: " << apa->size() << std::endl;
    std::cout << "auto_ptr to checksummed_array: " << *apa << std::endl;

    return 0;
}
