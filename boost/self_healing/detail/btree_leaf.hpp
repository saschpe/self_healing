/*! \file
* \brief B-Tree leaf.
*
* This file contains the class btree_leaf.
*
* (C) Copyright Sascha Peilicke 2010.
*
* Distributed under the Boost Software License, Version 1.0. (See
* accompanying file LICENSE_1_0.txt or copy at
* http://www.boost.org/LICENSE_1_0.txt)
*
* 31 May 2010 - Initial Revision (Sascha Peilicke)
*/

#ifndef BOOST_SELF_HEALING_BTREE_LEAF_HPP
#define BOOST_SELF_HEALING_BTREE_LEAF_HPP

#include "./child.hpp"
#include "./sibling.hpp"
#include "../checksummed_array.hpp"

#include <boost/config.hpp>
#include <boost/detail/workaround.hpp>


/// The namespace self_healing contains fault-tolerant data structures and utility classes.
namespace boost { namespace self_healing {

    /*! \brief TODO.
    *
    * TODO.
    *
    * \param T The data type of the stored values.
    * \param N The size of the chunk.
    * \throws std::invalid_argument Thrown if parent pointer is invalid.
    * \see child, sibling, checksummed_array
    */
    template <class T, std::size_t N = 64>
    class btree_leaf : public child<btree<T, N> >, public sibling<btree_leaf>, public checksummed_array<T, N>
    {
    public:
        // type definitions
        typedef T         value_type;       //!< The type of elements stored in the <code>checksummed_array</code>.
        typedef const T & const_reference;  //!< A const reference to an element.

        /*! Default constructor.
        * \param parent The parent B-tree.
        * \param value An initial value that is set for all elements.
        */
        explicit btree_leaf(btree<value_type, N> *const parent = NULL, const_reference value = 0)
            : child<btree<value_type, N> >(parent), checksummed_array<value_type, N>(value) {}

        /*! Validity check that tries to correct minor faults silently.
        * \param parent An optional pointer to the parent to check against.
        * \return true, if the internal structure and data is valid.
        * \see check_parent()
        */
        bool is_valid(btree<value_type, N> *const parent = NULL) const {
#ifdef BOOST_SELF_HEALING_DEBUG
            std::cout << "boost::self_healing::btree_leaf<T, N>::is_valid()" << std::endl;
#endif
            // call the is_valid methods of all base classes.
            return child<btree_leaf<value_type, N> >::is_valid(parent) &&
                   sibling<btree_leaf>::is_valid() &&
                   checksummed_array<value_type, N>::is_valid();
        }
    };

} } // namespace boost::self_healing


#endif // BOOST_SELF_HEALING_BTREE_LEAF_HPP
