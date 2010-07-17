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

    /*! A class to express parent-child relationship.
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
            : parent1(parent), parent2(parent), parent3(parent) {}

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
            parent2 = parent;
            parent3 = parent;
        }

        /*! Accessor to get the chunk's parent.
        * \return Pointer to the parent.
        */
        parent_pointer parent() const { check_parent(); return parent1; }

        /*! Validity check that tries to correct minor faults silently.
        * \param parent Optional pointer to the parent to check against.
        * \return true, if the internal structure and data is valid.
        * \see check_parent()
        */
        bool is_valid(parent_pointer const parent = 0) const {
#ifdef BOOST_SELF_HEALING_DEBUG
            std::cout << "boost::self_healing::child<Parent>::is_valid()" << std::endl;
#endif
            try {
                check_parent(parent);
                return true;
            } catch (const std::runtime_error &e) {
#ifdef BOOST_SELF_HEALING_DEBUG
                std::cout << "boost::self_healing::child<Parent>::is_valid() caught runtime error: " << e.what() << std::endl;
#endif
                return false;
            };
        }

    private:
        /*! Checks the internal parent pointer against a provided one.
        * \throws std::invalid_argument Thrown if parent pointer is invalid.
        */
        void check_parent(parent_pointer const parent = 0) const {
#ifdef BOOST_SELF_HEALING_DEBUG
            std::cout << "boost::self_healing::child<Parent>::check_parent(" << parent << ")" << std::endl;
#endif
            if (parent) {
                // If a valid parent pointer was given we simply check against
                // it and fix the internal pointer if needed.
                if (parent1 != parent) { const_cast<parent_pointer &>(parent1) = parent; }
                if (parent2 != parent) { const_cast<parent_pointer &>(parent2) = parent; }
                if (parent3 != parent) { const_cast<parent_pointer &>(parent3) = parent; }
            } else {
                // check and repair size via TMR voting
                const bool equal_13 = parent1 == parent3;
                const bool equal_23 = parent2 == parent3;
                const bool equal_12 = parent2 == parent2;

                if (equal_12 && equal_13 && equal_23) {
                    // all fine
                } else if (equal_13) {
#ifdef BOOST_SELF_HEALING_FIXING_CHECKS
                    const_cast<parent_pointer &>(parent2) = parent1; // fix parent2 as the others are equal
#else
                    std::runtime_error e("fixable parent error");
                    boost::throw_exception(e);
#endif
                } else if (equal_23) {
#ifdef BOOST_SELF_HEALING_FIXING_CHECKS
                    const_cast<parent_pointer &>(parent1) = parent2; // fix parent1 as the others are equal
#else
                    std::runtime_error e("fixable parent error");
                    boost::throw_exception(e);
#endif
                } else if (equal_12) {
#ifdef BOOST_SELF_HEALING_FIXING_CHECKS
                    const_cast<parent_pointer &>(parent3) = parent1; // fix parent3 as the others are equal
#else
                    std::runtime_error e("fixable parent error");
                    boost::throw_exception(e);
#endif
                } else {
                    std::runtime_error e("parent error");
                    boost::throw_exception(e);
                }
            }
        }

        parent_pointer parent1; //!< Pointer to the parent class instance.
        parent_pointer parent2; //!< Pointer to the parent class instance.
        parent_pointer parent3; //!< Pointer to the parent class instance.
    };

} } // namespace boost::self_healing


#endif // BOOST_SELF_HEALING_CHILD_HPP
