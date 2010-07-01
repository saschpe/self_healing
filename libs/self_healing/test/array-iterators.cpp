/* Test case for class checksummed array.
 *
 * (C) Copyright Sascha Peilicke 2010.
 *
 * Distributed under the Boost Software License, Version 1.0. (See
 * accompanying file LICENSE_1_0.txt or copy at
 * http://www.boost.org/LICENSE_1_0.txt)
 *
 * 14 May 2010 - Initial Revision (Sascha Peilicke)
 */

#include <boost/self_healing/array.hpp>

#include <algorithm>
#include <cassert>
#include <functional>
#include <iostream>

using namespace boost::self_healing;
using namespace std;

int main()
{
    cout << "testing class vector<> iterators" << endl;

    array<int, 16> a;

    cout << "array 1: " << a << endl;

    cout << "testing iterators" << endl;
    for (array<int, 16>::iterator it = a.begin(); it != a.end(); it++) {
        *it += 1;
    }

    cout << "testing iterator value assignment" << endl;
    array<int, 16>::iterator tmp_it = a.begin();
    *tmp_it = 99;

    cout << "it + value: " << tmp_it + 1 << endl;

    array<int, 16>::reverse_iterator tmp_rit = a.rbegin();
    //TODO: fix this, the functor is not called here !!!
    *tmp_rit = 99;

    const int* cit = a.begin();
    cout << cit << endl;

    cout << "randomly shuffle the elements" << endl;
    //random_shuffle(a.begin(), a.end());
    cout << "numbers: " << a << endl;

    cout << "locate the largest element, O(n)" << endl;
    array<int, 16>::iterator largest = max_element(a.begin(), a.end());

    array<int, 16>::const_iterator tmp_cit = a.begin();
    cout << "testing array<>::const_iterator " << tmp_cit << endl;

    cout << "testing array<>::const_reverse_iterator" << endl;
    array<int, 16>::const_reverse_iterator tmp_crit = a.rbegin();

    cout << "array 1:  ";
    for (reverse_iterator<array<int, 16>::iterator> rit = a.rbegin(); rit != a.rend(); rit++) {
        int i = *rit;
         cout << i << ",";
    }
    cout << endl;

    cout << "array 1: " << a << endl;
    return 0;
}
