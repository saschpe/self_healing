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

#include "utility.hpp"

using namespace std;

int main()
{
    cout << "testing class boost::self_healing::array<> fault injection" << endl;

    boost::self_healing::array<int, 8> a, ca;
    for (size_t i = 0; i < a.size(); i++) {
        cout << "a[" << i << "] = " << i << endl;
        a[i] = i;
    }
    cout << "array: " << a << endl;

    ca = a;
    utility::print_raw(&ca, sizeof(ca));
    utility::flip_bits(&ca, sizeof(ca), 1);
    utility::print_raw(&ca, sizeof(ca));
    cout << "1 flipped bit, is valid: " << ca.is_valid() << endl;

    utility::flip_bits(&(ca = a), sizeof(ca), 2);
    cout << "2 flipped bit, is valid: " << ca.is_valid() << endl;
    utility::flip_bits(&(ca = a), sizeof(ca), 3);
    cout << "3 flipped bit, is valid: " << ca.is_valid() << endl;

    utility::burst_flip_bits(&(ca = a), sizeof(ca), 2);
    cout << " 2 bit burst, is valid: " << ca.is_valid() << endl;
    utility::burst_flip_bits(&(ca = a), sizeof(ca), 3);
    cout << " 3 bit burst, is valid: " << ca.is_valid() << endl;
    utility::burst_flip_bits(&(ca = a), sizeof(ca), 5);
    cout << " 5 bit burst, is valid: " << ca.is_valid() << endl;
    utility::burst_flip_bits(&(ca = a), sizeof(ca), 17);
    cout << "17 bit burst, is valid: " << ca.is_valid() << endl;

    cout << "statistical test ...\n" << endl;
    size_t still_valid = 0;
    for (size_t i = 0; i < 1000; i++) {
        boost::self_healing::array<int, 64> tmp(23);
        utility::flip_bits(&tmp, sizeof(tmp), 1);
        if (tmp.is_valid()) {
            still_valid++;
        }
    }
    cout << still_valid << " out of 1000 array<int,64> are still valid" << endl;
    return 0;
}
