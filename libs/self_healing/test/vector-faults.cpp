/* Test case for class vector.
 *
 * (C) Copyright Sascha Peilicke 2010.
 *
 * Distributed under the Boost Software License, Version 1.0. (See
 * accompanying file LICENSE_1_0.txt or copy at
 * http://www.boost.org/LICENSE_1_0.txt)
 *
 * 28 April 2010 - Initial Revision (Sascha Peilicke)
 */

#include <boost/self_healing/vector.hpp>

#include <iostream>

#include "utility.hpp"

using namespace std;

int main()
{
    cout << "testing class boost::self_healing::vector<> fault injection" << endl;

    boost::self_healing::vector<short> v;


    cout << "statistical test ...\n" << endl;
    size_t still_valid = 0;
    for (size_t i = 0; i < 1000; i++) {
        boost::self_healing::vector<int> tmp(23);
        utility::flip_bits(&tmp, sizeof(tmp), 1);
        if (tmp.is_valid()) {
            still_valid++;
        }
    }
    cout << still_valid << " out of 1000 vector<int> are still valid" << endl;

    return 0;
}
