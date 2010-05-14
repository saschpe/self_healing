/* Test case for class vector.
 *
 * The original author site is at: http://saschpe.wordpress.com/
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

int main()
{
    std::cout << "testing class boost::self_healing::vector<> fault injection" << std::endl;

    boost::self_healing::vector<short> v;

    return 0;
}
