/* Test case for class reference_wrapper.
 *
 * The original author site is at: http://saschpe.wordpress.com/
 *
 * (C) Copyright Sascha Peilicke 2010.
 *
 * Distributed under the Boost Software License, Version 1.0. (See
 * accompanying file LICENSE_1_0.txt or copy at
 * http://www.boost.org/LICENSE_1_0.txt)
 *
 * 14 May 2010 - Initial Revision (Sascha Peilicke)
 */

#include <boost/self_healing/detail/reference_wrapper.hpp>

#include <iostream>

int main()
{
    std::cout << "testing class boost::self_healing::reference_wrapper<>" << std::endl;

    int i = 3;
    boost::self_healing::reference_wrapper<int> rr(i);

    std::cout << "sizeof(self_healing::reference_wrapper<int>): " << sizeof(rr) << std::endl;

    int j = 1 + *rr + 3;
    std::cout << "1 + *rr + 3: " << j << std::endl;

    int k = 1 + rr + 3;
    std::cout << "1 + rr + 3: " << k << std::endl;

    return 0;
}
