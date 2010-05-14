/* Test case for class reference.
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

#include <boost/self_healing/reference.hpp>

#include <iostream>

int main()
{
    int i = 3;
    boost::self_healing::reference<int> rr(i);
    int j = 1 + *rr + 3;
    int k = 1 + rr + 3;

    std::cout << "sizeof(self_healing::reference<int>): " << sizeof(rr) << std::endl;
    return 0;
}
