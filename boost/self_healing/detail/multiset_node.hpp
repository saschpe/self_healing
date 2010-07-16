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
    * \param Slots Amount of node children.
    * \throws std::invalid_argument Thrown if parent pointer is invalid.
    * \see child
    */
    template <class T, std::size_t Slots>
    class multiset_node : public child<multiset_node<T, Slots> >
                        , public array<multiset_node<T, Slots> *, Slots + 1>
    {
    public:
        // type definitions
        typedef T                         value_type;      //!< The type of elements stored in the <code>multiset_node</code>.
        typedef const T &                 const_reference; //!< A const reference to an element.
        typedef multiset_node<T, Slots>   parent_type;     //!< The type of the parent.
        typedef multiset_node<T, Slots> * parent_pointer;  //!< Pointer to parent objects.

    public:
        /*! Default constructor.
        * \param parent The parent B-tree.
        * \param value An initial value that is set for all elements.
        */
        explicit multiset_node(parent_pointer const parent = 0)
            : child<parent_type>(parent) {}

        /*! Validity check that tries to correct minor faults silently.
        * \param parent An optional pointer to the parent to check against.
        * \return true, if the internal structure and data is valid.
        * \see check_parent()
        */
        bool is_valid(parent_pointer const parent = 0) const {
#ifdef BOOST_SELF_HEALING_DEBUG
            std::cout << "boost::self_healing::multiset_node<T, Slots>::is_valid()" << std::endl;
#endif
            try {
                // check all parts of the data structure
                return child<parent_type>::is_valid(parent) &&
                       array<parent_pointer, Slots + 1>::is_valid();
            } catch (const std::runtime_error &e) {
#ifdef BOOST_SELF_HEALING_DEBUG
                std::cout << "boost::self_healing::multiset_node<T, Slots>::is_valid() caught runtime error: " << e.what() << std::endl;
#endif
                return false;
            };
        }
    };

} } // namespace boost::self_healing


#endif // BOOST_SELF_HEALING_MULTISET_LEAF_HPP
