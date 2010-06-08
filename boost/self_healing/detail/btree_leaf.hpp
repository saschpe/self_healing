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

    template <class T, std::size_t N = 64>
    class btree_node;

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
    class btree_leaf : public child<btree_node<T, N> >, public sibling<btree_leaf<T, N> >, public checksummed_array<T, N>
    {
    public:
        // type definitions
        typedef T                   value_type;         //!< The type of elements stored in the <code>checksummed_array</code>.
        typedef const T &           const_reference;    //!< A const reference to an element.
        typedef btree_node<T, N>    parent_type;        //!< The type of the parent.
        typedef btree_node<T, N> *  parent_pointer;     //!< Pointer to parent objects.
        typedef btree_leaf<T, N>    sibling_type;       //!< The type of the siblings.
        typedef btree_leaf<T, N> *  sibling_pointer;    //!< Pointer to sibling objects.

        /*! Default constructor.
        * \param parent The parent B-tree.
        * \param next Pointer to the next leaf sibling.
        * \param previous Pointer to the previous leaf sibling.
        * \param value An initial value that is set for all elements.
        */
        explicit btree_leaf(parent_pointer const parent = NULL, sibling_pointer next = NULL, sibling_pointer const previous = NULL, const_reference value = 0)
            : child<parent_type>(parent), sibling<sibling_type>(next, previous), checksummed_array<value_type, N>(value) {}

        /*! Validity check that tries to correct minor faults silently.
        * \param parent Optional pointer to the parent to check against.
        * \param next Optional pointer to the next sibling to check against.
        * \param previous Optional pointer to the previous sibling to check against.
        * \return true, if the internal structure and data is valid.
        * \see check_parent()
        */
        bool is_valid(parent_pointer const parent = NULL, sibling_pointer next = NULL, sibling_pointer const previous = NULL) const {
#ifdef BOOST_SELF_HEALING_DEBUG
            std::cout << "boost::self_healing::btree_leaf<T, N>::is_valid()" << std::endl;
#endif
            // call the is_valid methods of all base classes.
            return child<parent_type>::is_valid(parent) &&
                   sibling<sibling_type>::is_valid(next, previous) &&
                   checksummed_array<value_type, N>::is_valid();
        }
    };

} } // namespace boost::self_healing


#endif // BOOST_SELF_HEALING_BTREE_LEAF_HPP
