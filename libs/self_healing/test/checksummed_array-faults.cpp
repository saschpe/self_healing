/* Test case for class checksummed_array.
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

#include "utility.hpp"

int main()
{
    std::cout << "testing class boost::self_healing::vector<> fault injection" << std::endl;

    boost::self_healing::checksummed_array<int, 64> a1;
    std::cout << "checksummed_array 1: " << a1 << std::endl;

    // Insert faults and check

    return 0;
}
