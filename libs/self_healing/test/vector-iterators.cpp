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

#include <algorithm>
#include <cassert>
#include <functional>
#include <iostream>
#include <vector>

int main()
{
    std::cout << "testing class boost::self_healing::vector<> iterators" << std::endl;

    int arr[4] = {1, 2, 3, 4};

    std::cout << "initialize a vector from an array" << std::endl;
    boost::self_healing::vector<int> numbers(arr, arr + 4);

    std::cout << "randomly shuffle the elements" << std::endl;
    std::random_shuffle(numbers.begin(), numbers.end());
    std::cout << "numbers: " << numbers << std::endl;

    std::cout << "locate the largest element, O(n)" << std::endl;
    boost::self_healing::vector<int>::const_iterator largest = std::max_element(numbers.begin(), numbers.end());

    std::cout << "The largest number is " << *largest << "\n";
    std::cout << "It is located at index " << largest - numbers.begin() << "\n";

    std::cout << "sort the elements" << std::endl;
    std::sort( numbers.begin(), numbers.end() );
    std::cout << "numbers: " << numbers << std::endl;

    std::cout << "find the position of the number 5 in the vector, O(log n)" << std::endl;
    boost::self_healing::vector<int>::const_iterator five = std::lower_bound(numbers.begin(), numbers.end(), 5);

    std::cout << "The number 5 is located at index " << five - numbers.begin() << "\n";

    std::cout << "erase all the elements greater than 4" << std::endl;
    numbers.erase(std::remove_if(numbers.begin(), numbers.end(),
                  std::bind2nd(std::greater<int>(), 4)), numbers.end());

    std::cout << "print all the remaining numbers" << std::endl;
    for (boost::self_healing::vector<int>::const_iterator it = numbers.begin(); it != numbers.end(); ++it) {
        std::cout << *it << ",";
    }
    std::cout << std::endl;


    std::cout << "testing vector<>::const_iterator access patterns" << std::endl;
    boost::self_healing::vector<int, 2> v;
    boost::self_healing::vector<int, 2>::const_iterator cit = v.begin();
    std::cout << "cit      : " << cit << '\n';
    std::cout << "v.end()  : " << v.end() << '\n';
    std::cout << "cit + 1  : " << (cit + 1) << '\n';
    std::cout << "cit - 1  : " << (cit - 1) << '\n';
    std::cout << "cit += 2 : " << (cit += 2) << '\n';
    std::cout << "cit -= 2 : " << (cit -= 2) << '\n';
    std::cout << "cit++    : " << cit++ << '\n';
    std::cout << "++cit    : " << ++cit << '\n';
    std::cout << "cit--    : " << cit-- << '\n';
    std::cout << "--cit    : " << --cit << '\n';
    std::cout << std::endl;
    std::cout << "*(cit += 4): " << *(cit += 4) << std::endl;

    std::cout << "testing vector<>::const_iterator" << std::endl;
    for (boost::self_healing::vector<int, 2>::const_iterator it = v.begin(); it != v.end(); ++it) {
        std::cout << it << " -> "<< *it << '\n';
    }
    std::cout << std::endl;

    std::cout << "testing vector<>::const_reverse_iterator" << std::endl;
    for (boost::self_healing::vector<int, 2>::const_reverse_iterator it = v.rbegin(); it != v.rend(); ++it) {
        std::cout << *it << '\n';
    }
    std::cout << std::endl;


    return 0;
}
