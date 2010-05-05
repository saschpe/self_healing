
// Copyright Sascha Peilicke <sasch.pe@gmx.de> 2010.
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
//  http://www.boost.org/LICENSE_1_0.txt)

#include <robust/vector.hpp>

#include <iostream>
#include <cassert>
#include <vector>
#include <algorithm>
#include <functional>

int main()
{
    int arr[4] = {1, 2, 3, 4};

    std::cout << "initialize a vector from an array" << std::endl;
    robust::vector<int> numbers(arr, arr+4);

    std::cout << "insert more numbers into the vector" << std::endl;
    numbers.push_back(5);
    numbers.push_back(6);
    numbers.push_back(7);
    numbers.push_back(8);
    std::cout << "the vector currently holds {1, 2, 3, 4, 5, 6, 7, 8}" << std::endl;
    std::cout << "numbers: " << numbers << std::endl;

    std::cout << "randomly shuffle the elements" << std::endl;
    std::random_shuffle(numbers.begin(), numbers.end());
    std::cout << "numbers: " << numbers << std::endl;

    std::cout << "locate the largest element, O(n)" << std::endl;
    robust::vector<int>::const_iterator largest = std::max_element(numbers.begin(), numbers.end());

    std::cout << "The largest number is " << *largest << "\n";
    std::cout << "It is located at index " << largest - numbers.begin() << "\n";

    std::cout << "sort the elements" << std::endl;
    std::sort( numbers.begin(), numbers.end() );
    std::cout << "numbers: " << numbers << std::endl;

    std::cout << "find the position of the number 5 in the vector, O(log n)" << std::endl;
    robust::vector<int>::const_iterator five = std::lower_bound(numbers.begin(), numbers.end(), 5);

    std::cout << "The number 5 is located at index " << five - numbers.begin() << "\n";

    std::cout << "erase all the elements greater than 4" << std::endl;
    numbers.erase(std::remove_if(numbers.begin(), numbers.end(),
                  std::bind2nd(std::greater<int>(), 4)), numbers.end());

    std::cout << "print all the remaining numbers" << std::endl;
    for (robust::vector<int>::const_iterator it = numbers.begin(); it != numbers.end(); ++it) {
        std::cout << *it << ",";
    }
    std::cout << std::endl;

    return 0;
}
