/*! \file
* \brief B-Tree leaf.
*
* This file contains the class tree_leaf.
*
* (C) Copyright Sascha Peilicke 2010.
*
* Distributed under the Boost Software License, Version 1.0. (See
* accompanying file LICENSE_1_0.txt or copy at
* http://www.boost.org/LICENSE_1_0.txt)
*
* 23 April 2010 - Initial Revision (Sascha Peilicke)
*/

#ifndef BOOST_SELF_HEALING_TREE_LEAF_HPP
#define BOOST_SELF_HEALING_TREE_LEAF_HPP

#include <boost/detail/workaround.hpp>

// FIXES for broken compilers
#include <boost/config.hpp>

#include "./chunk.hpp"


/// The namespace self_healing contains fault-tolerant data structures and utility classes.
namespace boost { namespace self_healing {

    /*! \brief Element storage chunk.
    *
    * A chunk is a checksummed_array with a pointer to it's parent and to be
    * used as part of other data structures.
    *
    * \param T The data type of the stored values.
    * \param P The type of the parent data structure.
    * \param N The size of the chunk.
    * \throws std::invalid_argument Thrown if parent pointer is invalid.
    * \see checksummed_array
    */
    template <class T, std::size_t N = 64>
    class tree_leaf : public chunk<T, tree, N>
    {
    public:
        // type definitions
        typedef T         value_type;       //!< The type of elements stored in the <code>checksummed_array</code>.
        typedef const T & const_reference;  //!< A const reference to an element.

    private:
        tree_leaf *prev_sibling;
        tree_leaf *next_sibling;
    };

} } // namespace boost::self_healing


#endif // BOOST_SELF_HEALING_TREE_LEAF_HPP
