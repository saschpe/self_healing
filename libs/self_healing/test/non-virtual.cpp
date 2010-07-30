/* Test case for all classes to check that they don't have a VPTR.
 *
 * (C) Copyright Sascha Peilicke 2010.
 *
 * Distributed under the Boost Software License, Version 1.0. (See
 * accompanying file LICENSE_1_0.txt or copy at
 * http://www.boost.org/LICENSE_1_0.txt)
 *
 * 30 Jul 2010 - Initial Revision (Sascha Peilicke)
 */

#include <boost/self_healing/array.hpp>
#include <boost/self_healing/detail/child.hpp>
#include <boost/self_healing/detail/safe_ref.hpp>
#include <boost/self_healing/detail/sibling.hpp>
#include <boost/self_healing/detail/sized.hpp>
#include <boost/self_healing/vector.hpp>

#include <iostream>

using namespace std;
using namespace boost;
using namespace boost::self_healing;

int main()
{
    cout << "testing class boost::self_healing::array<>" << endl;
    assert(sizeof(array<char,8>) == 8 * sizeof(array<char, 8>::value_type) +
                                    2 * sizeof(array<char, 8>::checksum_type));

    cout << "testing class boost::self_healing::child<>" << endl;
    assert(sizeof(child<char>) == sizeof(child<char>::parent_pointer));

    cout << "testing class boost::self_healing::safe_ref<>" << endl;
    //assert(sizeof(safe_ref<char>) == sizeof(safe_ref<char>::reference) +
    //                                 sizeof(safe_ref<char>::function_type));

    cout << "testing class boost::self_healing::child<>" << endl;
    assert(sizeof(sibling<char>) == 2 * sizeof(sibling<char>::sibling_pointer));

    cout << "testing class boost::self_healing::sized" << endl;
    assert(sizeof(sized) == 3 * sizeof(sized::size_type));

    cout << "testing class boost::self_healing::vector<>" << endl;
    // all pointers are of the same size, thus vector::chunk_pointer == vector::pointer
    assert(sizeof(self_healing::vector<char>) == 2 * sizeof(self_healing::vector<char>::pointer) +
                                                     sizeof(self_healing::vector<char>::size_type) +
                                                     sizeof(sized));

    //TODO: add multiset

    return 0;
}
