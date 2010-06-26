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

#include "child.hpp"
#include "btree_node.hpp"
#include "sibling.hpp"
#include "../checksummed_array.hpp"

#include <boost/throw_exception.hpp>

#ifdef BOOST_SELF_HEALING_DEBUG
#   include <iostream>
#endif
#include <stdexcept>


/// The namespace self_healing contains fault-tolerant data structures and utility classes.
namespace boost { namespace self_healing {

    template <class T, std::size_t L, std::size_t CS>
    class btree_node; // forward declaration to break circular dependency

    /*! \brief TODO.
    *
    * TODO.
    *
    * \param T The data type of the stored values.
    * \param L Optional amount of children of nodes.
    * \param CS Optional (chunk) storage capacity of leaves.
    * \throws std::invalid_argument Thrown if parent pointer is invalid.
    * \see child, sibling, checksummed_array
    */
    template <class T, std::size_t L = 8, std::size_t CS = 64>
    class btree_leaf : public child<btree_node<T, L, CS> >, public sibling<btree_leaf<T, L, CS> >, public checksummed_array<T, CS>
    {
    public:
        // type definitions
        typedef T                       value_type;         //!< The type of elements stored in the <code>checksummed_array</code>.
        typedef const T &               const_reference;    //!< A const reference to an element.
        typedef btree_node<T, L, CS>    parent_type;        //!< The type of the parent.
        typedef btree_node<T, L, CS> *  parent_pointer;     //!< Pointer to parent objects.
        typedef btree_leaf<T, L, CS>    sibling_type;       //!< The type of the siblings.
        typedef btree_leaf<T, L, CS> *  sibling_pointer;    //!< Pointer to sibling objects.

        /*! Default constructor.
        * \param parent The parent B-tree.
        * \param next Pointer to the next leaf sibling.
        * \param previous Pointer to the previous leaf sibling.
        * \param value An initial value that is set for all elements.
        */
        explicit btree_leaf(parent_pointer const parent = 0, sibling_pointer const next = 0, sibling_pointer const previous = 0, const_reference value = 0)
            : child<parent_type>(parent), sibling<sibling_type>(next, previous), checksummed_array<value_type, CS>(value) {}

        /*! Validity check that tries to correct minor faults silently.
        * \param parent Optional pointer to the parent to check against.
        * \param next Optional pointer to the next sibling to check against.
        * \param previous Optional pointer to the previous sibling to check against.
        * \return true, if the internal structure and data is valid.
        */
        bool is_valid(parent_pointer const parent = 0, sibling_pointer const next = 0, sibling_pointer const previous = 0) const {
#ifdef BOOST_SELF_HEALING_DEBUG
            std::cout << "boost::self_healing::btree_leaf<T, CS>::is_valid()" << std::endl;
#endif
            // call the is_valid methods of all base classes.
            return child<parent_type>::is_valid(parent) &&
                   sibling<sibling_type>::is_valid(next, previous) &&
                   checksummed_array<value_type, CS>::is_valid();
        }
    };

} } // namespace boost::self_healing


#endif // BOOST_SELF_HEALING_BTREE_LEAF_HPP
