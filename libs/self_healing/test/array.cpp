/* Test case for class checksummed array.
 *
 * (C) Copyright Sascha Peilicke 2010.
 *
 * Distributed under the Boost Software License, Version 1.0. (See
 * accompanying file LICENSE_1_0.txt or copy at
 * http://www.boost.org/LICENSE_1_0.txt)
 *
 * 27 April 2010 - Initial Revision (Sascha Peilicke)
 */

#include <boost/self_healing/array.hpp>

#include <iostream>
#include <cassert>
#include <memory>

using namespace boost::self_healing;
using namespace std;

static void empty_function() {}

int main()
{
    cout << "testing class array<>" << endl;

    array<int, 16> a;
    cout << "array 1: " << a << endl;

    int i = 3;
    array<int, 16>::reference ar(i, empty_function);
    cout << "testing array<>::reference constructor for " << ar << endl;
    array<int, 16>::const_reference car(i);
    cout << "testing array<>::const_reference constructor for " << car << endl;

    assert(a.at(0) == 0);
    assert(a.at(a.size() - 1) == 0);

    cout << "testing array<>::reference assignment" << endl;
    a.front() = 3;
    a.back() = 4;
    a[2] = 4;
    a.at(3) = 5;
    a.at(13) = a.at(12);

    cout << "testing array<>::reference comparison" << endl;
    assert(a.front() == 3);
    assert(a.front() != 5);
    assert(a.front() >= 2);
    cout << (a.front() <= 2) << endl;
    cout << (a.front() > 3) << endl;
    cout << (a.front() < 3) << endl;

    cout << "testing array<>::reference arithmetic operations" << endl;
    cout << a.front() + 2 << endl;
    cout << a.front() - 2 << endl;
    cout << a.front() * 2 << endl;
    cout << a.front() / 2 << endl;
    cout << a.front() % 2 << endl;

    cout << "testing array<>::reference arithmetic assignment operations" << endl;
    cout << a[0];
    cout << "a1[6] += 1: " << (a[6] += 1) << endl;
    cout << (a[7] -= 1) << endl;
    cout << (a[7] *= 1) << endl;
    cout << (a[7] /= 1) << endl;
    cout << (a[7] %= 1) << endl;

    cout << "testing array<>::reference increment and decrement operations" << endl;
    a[8]++;
    ++a[9];
    a[10]--;
    --a[11];

    cout << "array 1:  ";
    copy(a.begin(), a.end(), ostream_iterator<int>(cout, ","));
    cout << endl;

    array<int, 8> *pa = new array<int, 8>(3);
    cout << "pointer to array: " << *pa << endl;

    auto_ptr<array<int, 8> > apa(pa);
    cout << "auto_ptr to array size: " << apa->size() << endl;
    cout << "auto_ptr to array: " << *apa << endl;

    return 0;
}
