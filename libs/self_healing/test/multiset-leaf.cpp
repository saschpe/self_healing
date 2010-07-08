/* Test case for class multiset-leaf.
 *
 * (C) Copyright Sascha Peilicke 2010.
 *
 * Distributed under the Boost Software License, Version 1.0. (See
 * accompanying file LICENSE_1_0.txt or copy at
 * http://www.boost.org/LICENSE_1_0.txt)
 *
 * 31 May 2010 - Initial Revision (Sascha Peilicke)
 */

#include <boost/self_healing/detail/multiset_leaf.hpp>

#include <iostream>

int main()
{
    std::cout << "testing class boost::self_healing::multiset_leaf<>" << std::endl;

    boost::self_healing::multiset_node<int, 3, 3> parent;
    boost::self_healing::multiset_leaf<int, 3, 3> leaf1(&parent);
    boost::self_healing::multiset_leaf<int, 3, 3> leaf2(&parent);
    boost::self_healing::multiset_leaf<int, 3, 3> leaf3(&parent);

    leaf1.set_next(&leaf2);
    leaf2.set_next(&leaf3);
    leaf2.set_previous(&leaf1);
    leaf3.set_previous(&leaf2);

    return 0;
}
