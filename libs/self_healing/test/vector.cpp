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

int main()
{
    std::cout << "testing class boost::self_healing::vector<>" << std::endl;

    int arr[4] = {1, 2, 3, 4};

    std::cout << "initialize a vector from an array" << std::endl;
    boost::self_healing::vector<int> numbers(arr, arr + 4);

    std::cout << "insert more numbers into the vector" << std::endl;
    numbers.push_back(5);
    numbers.push_back(6);
    numbers.push_back(7);
    numbers.push_back(8);
    std::cout << "the vector currently holds {1, 2, 3, 4, 5, 6, 7, 8}" << std::endl;
    std::cout << "numbers: " << numbers << std::endl;

    return 0;
}
