/*! \file
* \brief Child.
*
* This file contains the class child.
*
* (C) Copyright Sascha Peilicke 2010.
*
* Distributed under the Boost Software License, Version 1.0. (See
* accompanying file LICENSE_1_0.txt or copy at
* http://www.boost.org/LICENSE_1_0.txt)
*
* 23 April 2010 - Initial Revision (Sascha Peilicke)
*/

#ifndef BOOST_SELF_HEALING_CHILD_HPP
#define BOOST_SELF_HEALING_CHILD_HPP

#include <boost/throw_exception.hpp>

#ifdef BOOST_SELF_HEALING_DEBUG
#   include <iostream>
#endif
#include <stdexcept>


/// The namespace self_healing contains fault-tolerant data structures and utility classes.
namespace boost { namespace self_healing {

    /*! A class to express parent-child relationship. This class is not
    * inherently fault-tolerant, but meant to be part of other classes.
    * \param Parent The type of the parent data structure.
    * \see array
    */
    template <class Parent>
    class child
    {
    public:
        // type definitions
        typedef Parent   parent_type;    //!< The type of the parent class.
        typedef Parent * parent_pointer; //!< Pointer to parent class.

        /*! Default constructor.
        * \param parent The parent.
        * \throws std::invalid_argument Thrown if parent pointer is invalid.
        */
        explicit child(parent_pointer const parent = 0)
            : parent1(parent) {}

        /*! Set a new parent.
        * \param parent Pointer to the new parent.
        * \throws std::invalid_argument Thrown if parent pointer is invalid.
        */
        void set_parent(parent_pointer const parent) {
            if (!parent) {
                std::invalid_argument e("parent argument is null");
                boost::throw_exception(e);
            };
            parent1 = parent;
        }

        /*! Accessor to get the chunk's parent.
        * \return Pointer to the parent.
        */
        parent_pointer parent() const { return parent1; }

        /*! Validity check that needs a parent to compare with.
        * \param parent Pointer to the parent to check against.
        * \return true, if the internal structure and data is valid.
        */
        bool is_valid(parent_pointer const parent = 0) const {
#ifdef BOOST_SELF_HEALING_DEBUG
            std::cout << "boost::self_healing::child<Parent>::is_valid()" << std::endl;
#endif
            if (parent) {
                // If a valid parent pointer was given we simply check against
                // it and fix the internal pointer if needed.
                if (parent != parent1) { const_cast<parent_pointer &>(parent1) = parent; }
                return true;
            } else {
                return parent1 != 0;
            }
        }

    private:
        parent_pointer parent1; //!< Pointer to the parent class instance.
    };

} } // namespace boost::self_healing


#endif // BOOST_SELF_HEALING_CHILD_HPP
