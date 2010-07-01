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

int main()
{
    std::cout << "testing class boost::self_healing::array<> fault injection" << std::endl;

    boost::self_healing::array<int, 8> a, ca;
    for (std::size_t i = 0; i < a.size(); i++) {
        std::cout << "a[" << i << "] = " << i << std::endl;
        a[i] = i;
    }
    std::cout << "array: " << a << std::endl;

    ca = a;
    utility::print_raw(&ca, sizeof(ca));
    utility::flip_bits(&ca, sizeof(ca), 1);
    utility::print_raw(&ca, sizeof(ca));
    std::cout << "1 flipped bit, is valid: " << ca.is_valid() << std::endl;

    utility::flip_bits(&(ca = a), sizeof(ca), 2);
    std::cout << "2 flipped bit, is valid: " << ca.is_valid() << std::endl;
    utility::flip_bits(&(ca = a), sizeof(ca), 3);
    std::cout << "3 flipped bit, is valid: " << ca.is_valid() << std::endl;

    utility::burst_flip_bits(&(ca = a), sizeof(ca), 2);
    std::cout << " 2 bit burst, is valid: " << ca.is_valid() << std::endl;
    utility::burst_flip_bits(&(ca = a), sizeof(ca), 3);
    std::cout << " 3 bit burst, is valid: " << ca.is_valid() << std::endl;
    utility::burst_flip_bits(&(ca = a), sizeof(ca), 5);
    std::cout << " 5 bit burst, is valid: " << ca.is_valid() << std::endl;
    utility::burst_flip_bits(&(ca = a), sizeof(ca), 17);
    std::cout << "17 bit burst, is valid: " << ca.is_valid() << std::endl;

    return 0;
}
