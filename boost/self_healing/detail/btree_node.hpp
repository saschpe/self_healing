/*! \file
* \brief B-Tree node.
*
* This file contains the class btree_node.
*
* (C) Copyright Sascha Peilicke 2010.
*
* Distributed under the Boost Software License, Version 1.0. (See
* accompanying file LICENSE_1_0.txt or copy at
* http://www.boost.org/LICENSE_1_0.txt)
*
* 31 May 2010 - Initial Revision (Sascha Peilicke)
*/

#ifndef BOOST_SELF_HEALING_BTREE_NODE_HPP
#define BOOST_SELF_HEALING_BTREE_NODE_HPP

#include "child.hpp"
#include "btree_leaf.hpp"

#include <boost/throw_exception.hpp>
#include <boost/variant.hpp>

#ifdef BOOST_SELF_HEALING_DEBUG
#   include <iostream>
#endif
#include <stdexcept>


/// The namespace self_healing contains fault-tolerant data structures and utility classes.
namespace boost { namespace self_healing {

    template <class T, std::size_t L, std::size_t CS>
    class btree_leaf; // forward declaration

    /*! \brief TODO.
    *
    * TODO.
    *
    * \param T The data type of the stored values.
    * \param L Optional amount of children of nodes.
    * \param CS Optional (chunk) storage capacity of leaves.
    * \throws std::invalid_argument Thrown if parent pointer is invalid.
    * \see child
    */
    template <class T, std::size_t L = 8, std::size_t CS = 64>
    class btree_node : public child<btree_node<T, L, CS> >
    {
        // private type definitions
        typedef btree_node<T, L, CS>    node_type;

    public:
        // type definitions
        typedef T                       value_type;         //!< The type of elements stored in the <code>checksummed_array</code>.
        typedef const T &               const_reference;    //!< A const reference to an element.
        typedef btree_node<T, L, CS>    parent_type;        //!< The type of the parent.
        typedef btree_node<T, L, CS> *  parent_pointer;     //!< Pointer to parent objects.
        typedef btree_leaf<T, L, CS>    leaf_type;

    public:
        /*! Default constructor.
        * \param parent The parent B-tree.
        * \param value An initial value that is set for all elements.
        */
        explicit btree_node(parent_pointer const parent = 0)
            : child<parent_type>(parent) {}

        /*! Validity check that tries to correct minor faults silently.
        * \param parent An optional pointer to the parent to check against.
        * \return true, if the internal structure and data is valid.
        * \see check_parent()
        */
        bool is_valid(parent_pointer const parent = 0) const {
#ifdef BOOST_SELF_HEALING_DEBUG
            std::cout << "boost::self_healing::btree_node<T, CS>::is_valid()" << std::endl;
#endif
            try {
                // check all parts of the data structure
                check_children();
                return child<parent_type>::is_valid(parent);
            } catch (const std::runtime_error &e) {
#ifdef BOOST_SELF_HEALING_DEBUG
                std::cout << "boost::self_healing::btree_node<T, CS>::is_valid() caught runtime error: " << e.what() << std::endl;
#endif
                return false;
            };
        }

    private:
        void check_children() const {
#ifdef BOOST_SELF_HEALING_DEBUG
            std::cout << "boost::self_healing::btree_node<T, CS>::check_children" << std::endl;
#endif
            //TODO: implement
        }

        /*! Children may be either be nodes or leaves
        */
        //TODO: add node_type and fix it, check back guarantees of variant
        boost::variant<leaf_type> children;
    };

} } // namespace boost::self_healing


#endif // BOOST_SELF_HEALING_BTREE_LEAF_HPP
