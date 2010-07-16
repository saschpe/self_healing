/* Test case for class multiset-node.
 *
 * (C) Copyright Sascha Peilicke 2010.
 *
 * Distributed under the Boost Software License, Version 1.0. (See
 * accompanying file LICENSE_1_0.txt or copy at
 * http://www.boost.org/LICENSE_1_0.txt)
 *
 * 31 May 2010 - Initial Revision (Sascha Peilicke)
 */

#include <boost/self_healing/detail/multiset_node.hpp>

#include <iostream>

int main()
{
    std::cout << "testing class boost::self_healing::multiset_node<>" << std::endl;

    boost::self_healing::multiset_node<int, 2> parent;
    boost::self_healing::multiset_node<int, 2> child(&parent);

    return 0;
}
