/*! \file
* \brief Multiset internal B-tree node.
*
* This file contains the class multiset_node.
*
* (C) Copyright Sascha Peilicke 2010.
*
* Distributed under the Boost Software License, Version 1.0. (See
* accompanying file LICENSE_1_0.txt or copy at
* http://www.boost.org/LICENSE_1_0.txt)
*
* 31 May 2010 - Initial Revision (Sascha Peilicke)
*/

#ifndef BOOST_SELF_HEALING_MULTISET_NODE_HPP
#define BOOST_SELF_HEALING_MULTISET_NODE_HPP

#include "child.hpp"
#include "../array.hpp"

#include <boost/throw_exception.hpp>
#include <boost/variant.hpp>

#ifdef BOOST_SELF_HEALING_DEBUG
#   include <iostream>
#endif
#include <stdexcept>


/// The namespace self_healing contains fault-tolerant data structures and utility classes.
namespace boost { namespace self_healing {

    /*! \brief TODO.
    *
    * TODO.
    *
    * \param T The data type of the stored values.
    * \param Size Amount of node children.
    * \throws std::invalid_argument Thrown if parent pointer is invalid.
    * \see child
    */
    template <class T, std::size_t Size>
    class multiset_node : public child<multiset_node<T, Size> >
                        , public array<boost::variant<multiset_node<T, Size> *, T>, Size + 1>
    {
    public:
        // type definitions
        typedef T                         value_type;      //!< The type of elements stored in the <code>multiset_node</code>.
        typedef const T &                 const_reference; //!< A const reference to an element.
        typedef multiset_node<T, Size>   parent_type;      //!< The type of the parent.
        typedef multiset_node<T, Size> * parent_pointer;   //!< Pointer to parent objects.

        /*! Default constructor.
        * \param parent The parent B-tree.
        */
        explicit multiset_node(parent_pointer const parent = 0)
            : child<parent_type>(parent) {}

        bool is_full() const { return slot_use() == Size; }
        bool is_few() const { return slot_use() <= Size; }
        bool is_underflow() const { return slot_use() < Size; }

        /*! Validity check that tries to correct minor faults silently.
        * \param parent An optional pointer to the parent to check against.
        * \return true, if the internal structure and data is valid.
        * \see check_parent()
        */
        bool is_valid(parent_pointer const parent = 0) const {
#ifdef BOOST_SELF_HEALING_DEBUG
            std::cout << "boost::self_healing::multiset_node<T, Size>::is_valid()" << std::endl;
#endif
            // check all parts of the data structure
            return child<parent_type>::is_valid(parent) &&
                   array<boost::variant<multiset_node<T, Size> *, T>, Size + 1>::is_valid();
        }

    private:

        unsigned short level() const {
            return level1;
        }

        unsigned short slot_use() const {
            return slot_use1;
        }

        unsigned short level1;      //!< Level in the b-tree, if level == 0 -> leaf node
        unsigned short slot_use1;   //!< Number of key slotuse use, so number of valid children or data pointers

    };

    template <class T, std::size_t Size>
    class multiset_inner_node : public multiset_node<T, Size>
                              , public array<multiset_node<T, Size> *, Size + 1>
    {
    };

    template <class T, std::size_t Size>
    class multiset_leaf_node : public multiset_node<T, Size>
                             , public sibling<multiset_leaf_node<T, Size> >
                             , public array<T, Size>
    {
    };

} } // namespace boost::self_healing


#endif // BOOST_SELF_HEALING_MULTISET_LEAF_HPP
