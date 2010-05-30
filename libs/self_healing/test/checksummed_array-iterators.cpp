/* Test case for class checksummed_array.
 *
 * (C) Copyright Sascha Peilicke 2010.
 *
 * Distributed under the Boost Software License, Version 1.0. (See
 * accompanying file LICENSE_1_0.txt or copy at
 * http://www.boost.org/LICENSE_1_0.txt)
 *
 * 14 May 2010 - Initial Revision (Sascha Peilicke)
 */

#include <boost/self_healing/checksummed_array.hpp>

#include <iostream>

int main()
{
    std::cout << "testing class boost::self_healing::vector<> iterators" << std::endl;

    boost::self_healing::checksummed_array<int, 16> a;

    std::cout << "checksummed_array 1: " << a << std::endl;

    std::cout << "testing iterators" << std::endl;
    for (boost::self_healing::checksummed_array<int, 16>::iterator it = a.begin(); it != a.end(); it++) {
        *it += 1;
    }

    std::cout << "testing iterator value assignment" << std::endl;
    boost::self_healing::checksummed_array<int, 16>::iterator tmp_it = a.begin();
    *tmp_it = 99;

    boost::self_healing::checksummed_array<int, 16>::reverse_iterator tmp_rit = a.rbegin();
    //TODO: fix this, the functor is not called here !!!
    *tmp_rit = 99;

    const int* cit = a.begin();
    std::cout << cit << std::endl;

    boost::self_healing::checksummed_array<int, 16>::const_iterator tmp_cit = a.begin();
    std::cout << "testing checksummed_array<>::const_iterator " << tmp_cit << std::endl;

    std::cout << "testing checksummed_array<>::const_reverse_iterator" << std::endl;
    boost::self_healing::checksummed_array<int, 16>::const_reverse_iterator tmp_crit = a.rbegin();

    std::cout << "checksummed_array 1:  ";
    for (std::reverse_iterator<boost::self_healing::checksummed_array<int, 16>::iterator> rit = a.rbegin(); rit != a.rend(); rit++) {
        int i = *rit;
        std:: cout << i << ",";
    }
    std::cout << std::endl;

    std::cout << "checksummed_array 1: " << a << std::endl;
    return 0;
}
