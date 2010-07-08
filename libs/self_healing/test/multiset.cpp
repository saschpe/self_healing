/* Test case for class multiset.
 *
 * (C) Copyright Sascha Peilicke 2010.
 *
 * Distributed under the Boost Software License, Version 1.0. (See
 * accompanying file LICENSE_1_0.txt or copy at
 * http://www.boost.org/LICENSE_1_0.txt)
 *
 * 31 May 2010 - Initial Revision (Sascha Peilicke)
 */

#include <boost/self_healing/multiset.hpp>

#include <iostream>

using namespace std;

int main()
{
    cout << "testing class boost::self_healing::multiset<>" << endl;

    boost::self_healing::multiset<float> set;

    const int N = 10;
    int a[N] = {4, 1, 1, 1, 1, 1, 0, 5, 1, 0};
    int b[N] = {4, 4, 2, 4, 2, 4, 0, 1, 5, 5};

    boost::self_healing::multiset<int> A(a, a + N);
    boost::self_healing::multiset<int> B(b, b + N);
    boost::self_healing::multiset<int> C;

    cout << "Set A: ";
    copy(A.begin(), A.end(), ostream_iterator<int>(cout, " "));
    cout << endl;
    cout << "Set B: ";
    copy(B.begin(), B.end(), ostream_iterator<int>(cout, " "));
    cout << endl;

    cout << "Union: ";
    set_union(A.begin(), A.end(), B.begin(), B.end(), ostream_iterator<int>(cout, " "));
    cout << endl;

    cout << "Intersection: ";
    set_intersection(A.begin(), A.end(), B.begin(), B.end(), ostream_iterator<int>(cout, " "));
    cout << endl;

    set_difference(A.begin(), A.end(), B.begin(), B.end(), inserter(C, C.begin()));
    cout << "Set C (difference of A and B): ";
    copy(C.begin(), C.end(), ostream_iterator<int>(cout, " "));
    cout << endl;

    // Create a multiset of integers.
    boost::self_healing::multiset<int>  si;

    for (boost::self_healing::multiset<int>::value_type j = 0; j < 2; j++) {
        for (boost::self_healing::multiset<int>::value_type i = 0; i < 10; ++i) {
            // Insert values with a hint.
            si.insert(si.begin (), i);
        }
    }
    // Print out the multiset.
    cout << si << std::endl;

    // Make another multiset and an empty multiset.
    boost::self_healing::multiset<int> si2, result;
    for (boost::self_healing::multiset<int>::value_type i = 0; i < 10; i++) {
        si2.insert(i + 5);
    }

    cout << si2 << endl;

    // Try a couple of set algorithms.
    std::set_union(si.begin(), si.end(), si2.begin(), si2.end(), inserter(result, result.begin()));

    cout << "Union:\n" << result << endl;

    result.erase(result.begin(), result.end());

    set_intersection(si.begin(), si.end(), si2.begin(), si2.end(), inserter(result, result.begin()));

    cout << "Intersection:\n" << result << endl;

    return 0;
}
