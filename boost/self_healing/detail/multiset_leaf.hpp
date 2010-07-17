/*! \file
* \brief Multiset internal B-tree leaf.
*
* This file contains the class multiset_leaf.
*
* (C) Copyright Sascha Peilicke 2010.
*
* Distributed under the Boost Software License, Version 1.0. (See
* accompanying file LICENSE_1_0.txt or copy at
* http://www.boost.org/LICENSE_1_0.txt)
*
* 31 May 2010 - Initial Revision (Sascha Peilicke)
*/

#ifndef BOOST_SELF_HEALING_MULTISET_LEAF_HPP
#define BOOST_SELF_HEALING_MULTISET_LEAF_HPP

#include "child.hpp"
#include "multiset_node.hpp"
#include "sibling.hpp"
#include "../array.hpp"

#include <boost/throw_exception.hpp>

#ifdef BOOST_SELF_HEALING_DEBUG
#   include <iostream>
#endif
#include <stdexcept>


/// The namespace self_healing contains fault-tolerant data structures and utility classes.
namespace boost { namespace self_healing {

    template <class T, std::size_t Leaves, std::size_t Size>
    class multiset_node; // forward declaration to break circular dependency

    /*! \brief TODO.
    *
    * TODO.
    *
    * \param T The data type of the stored values.
    * \param Leaves Optional amount of children of nodes.
    * \param Size Optional storage capacity of leaves.
    * \throws std::invalid_argument Thrown if parent pointer is invalid.
    * \see child, sibling, array
    */
    template <class T, std::size_t Leaves = 8, std::size_t Size = 64>
    class multiset_leaf : public child<multiset_node<T, Leaves, Size> >, public sibling<multiset_leaf<T, Leaves, Size> >, public array<T, Size>
    {
    public:
        // type definitions
        typedef T                                value_type;      //!< The type of elements stored in the <code>array</code>.
        typedef const T &                        const_reference; //!< A const reference to an element.
        typedef multiset_node<T, Leaves, Size>   parent_type;     //!< The type of the parent.
        typedef multiset_node<T, Leaves, Size> * parent_pointer;  //!< Pointer to parent objects.
        typedef multiset_leaf<T, Leaves, Size>   sibling_type;    //!< The type of the siblings.
        typedef multiset_leaf<T, Leaves, Size> * sibling_pointer; //!< Pointer to sibling objects.

        /*! Default constructor.
        * \param parent The parent B-tree.
        * \param next Pointer to the next leaf sibling.
        * \param previous Pointer to the previous leaf sibling.
        * \param value An initial value that is set for all elements.
        */
        explicit multiset_leaf(parent_pointer const parent = 0, sibling_pointer const next = 0, sibling_pointer const previous = 0, const_reference value = 0)
            : child<parent_type>(parent), sibling<sibling_type>(next, previous), array<value_type, Size>(value) {}

        /*! Validity check that tries to correct minor faults silently.
        * \param parent Optional pointer to the parent to check against.
        * \param next Optional pointer to the next sibling to check against.
        * \param previous Optional pointer to the previous sibling to check against.
        * \return true, if the internal structure and data is valid.
        */
        bool is_valid(parent_pointer const parent = 0, sibling_pointer const next = 0, sibling_pointer const previous = 0) const {
#ifdef BOOST_SELF_HEALING_DEBUG
            std::cout << "boost::self_healing::multiset_leaf<T, Leaves, Size>::is_valid()" << std::endl;
#endif
            // call the is_valid methods of all base classes.
            return child<parent_type>::is_valid(parent) &&
                   sibling<sibling_type>::is_valid(next, previous) &&
                   array<value_type, Size>::is_valid();
        }
    };

} } // namespace boost::self_healing


#endif // BOOST_SELF_HEALING_MULTISET_LEAF_HPP
