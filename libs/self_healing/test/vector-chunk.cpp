/* Test case for class chunk.
 *
 * (C) Copyright Sascha Peilicke 2010.
 *
 * Distributed under the Boost Software License, Version 1.0. (See
 * accompanying file LICENSE_1_0.txt or copy at
 * http://www.boost.org/LICENSE_1_0.txt)
 *
 * 28 April 2010 - Initial Revision (Sascha Peilicke)
 */

#include <boost/self_healing/detail/vector_chunk.hpp>

#include <iostream>

int main()
{
    std::cout << "testing class boost::self_healing::chunk<>" << std::endl;

    // this parent is invalid, thus checks should fail
    boost::self_healing::vector<int, 4> *parent = NULL;
    boost::self_healing::vector_chunk<int, 4> c(parent, 43);

    std::cout << "invalid parent: " << &parent << std::endl;
    std::cout << "chunk is valid: " << c.is_valid() << std::endl;
    std::cout << "chunk parent: " << c.parent() << std::endl;
    std::cout << "chunk is valid (with parent " << parent << "): " << c.is_valid(parent) << std::endl;

    // this parent is valid, altough useless, it's a dangling pointer
    // the is_valid() check can be tricked by that because it does not
    // dereferenced the parent, it onl compares addresses.
    //NOTE: dereferencing dangling pointers is generally unsafe.
    std::cout << "setting a valid parent" << std::endl;
    boost::self_healing::vector<int, 4> *another_parent;
    c.set_parent(another_parent);

    std::cout << "valid parent: " << &another_parent << std::endl;
    std::cout << "chunk is valid: " << c.is_valid() << std::endl;
    std::cout << "chunk parent: " << c.parent() << std::endl;
    std::cout << "chunk is valid (with parent " << another_parent << "): " << c.is_valid(another_parent) << std::endl;

    std::cout << "chunk: " << c << std::endl;
    return 0;
}

