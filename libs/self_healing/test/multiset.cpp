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

    return 0;
}
