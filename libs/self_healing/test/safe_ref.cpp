/* Test case for class safe_ref.
 *
 * (C) Copyright Sascha Peilicke 2010.
 *
 * Distributed under the Boost Software License, Version 1.0. (See
 * accompanying file LICENSE_1_0.txt or copy at
 * http://www.boost.org/LICENSE_1_0.txt)
 *
 * 14 May 2010 - Initial Revision (Sascha Peilicke)
 */

#include <boost/self_healing/detail/safe_ref.hpp>

#include <iostream>

static void empty_function() {}

int main()
{
    std::cout << "testing class boost::self_healing::safe_ref<>" << std::endl;

    int i = 3;
    boost::self_healing::safe_ref<int> rr(i, empty_function);

    std::cout << "sizeof(self_healing::safe_ref<int>): " << sizeof(rr) << std::endl;

    int j = 1 + *rr + 3;
    std::cout << "1 + *rr + 3: " << j << std::endl;

    int k = 1 + rr + 3;
    std::cout << "1 + rr + 3: " << k << std::endl;

    return 0;
}
