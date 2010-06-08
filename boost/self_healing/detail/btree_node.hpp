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

#include "./child.hpp"

#include <boost/config.hpp>
#include <boost/detail/workaround.hpp>
#include <boost/throw_exception.hpp>

#ifdef BOOST_SELF_HEALING_DEBUG
#   include <iostream>
#endif
#include <stdexcept>


/// The namespace self_healing contains fault-tolerant data structures and utility classes.
namespace boost { namespace self_healing {

    template <class T, std::size_t N = 64>
    class btree_leaf;   // forward declaration

    /*! \brief TODO.
    *
    * TODO.
    *
    * \param T The data type of the stored values.
    * \param N The size 
    * \throws std::invalid_argument Thrown if parent pointer is invalid.
    * \see child
    */
    template <class T, std::size_t N = 64>
    class btree_node : public child<btree_node<T, N> >
    {
    public:
        // type definitions
        typedef T                   value_type;         //!< The type of elements stored in the <code>checksummed_array</code>.
        typedef const T &           const_reference;    //!< A const reference to an element.
        typedef btree_node<T, N>    parent_type;        //!< The type of the parent.
        typedef btree_node<T, N> *  parent_pointer;     //!< Pointer to parent objects.

        /*! Default constructor.
        * \param parent The parent B-tree.
        * \param value An initial value that is set for all elements.
        */
        explicit btree_node(parent_pointer const parent = NULL)
            : child<parent_type>(parent) {}

        /*! Validity check that tries to correct minor faults silently.
        * \param parent An optional pointer to the parent to check against.
        * \return true, if the internal structure and data is valid.
        * \see check_parent()
        */
        bool is_valid(parent_pointer const parent = NULL) const {
#ifdef BOOST_SELF_HEALING_DEBUG
            std::cout << "boost::self_healing::btree_node<T, N>::is_valid()" << std::endl;
#endif
            try {
                // check all parts of the data structure
                check_children();
                return child<parent_type>::is_valid(parent);
            } catch (const std::runtime_error &e) {
#ifdef BOOST_SELF_HEALING_DEBUG
                std::cout << "boost::self_healing::btree_node<T, N>::is_valid() caught runtime error: " << e.what() << std::endl;
#endif
                return false;
            };
        }
    private:

        void check_children() const {
#ifdef BOOST_SELF_HEALING_DEBUG
            std::cout << "boost::self_healing::btree_node<T, N>::check_children" << std::endl;
#endif

        }

        /*! Children may be either nodes or leaves
        */
        union {
            btree_node<T, N> nodes;
            btree_leaf<T, N> leaves;
        } m_children;
    };

} } // namespace boost::self_healing


#endif // BOOST_SELF_HEALING_BTREE_LEAF_HPP
