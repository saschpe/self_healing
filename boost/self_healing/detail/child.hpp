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

#include <boost/config.hpp>
#include <boost/detail/workaround.hpp>
#include <boost/throw_exception.hpp>

#ifdef BOOST_SELF_HEALING_DEBUG
#   include <iostream>
#endif
#include <stdexcept>


/// The namespace self_healing contains fault-tolerant data structures and utility classes.
namespace boost { namespace self_healing {

    /*! A class to express parent-child relationship.
    * \param P The type of the parent data structure.
    * \see checksummed_array
    */
    template <class P>
    class child
    {
    public:
        // type definitions
        typedef P         parent_type;      //!< The type of the parent class.
        typedef P *       parent_pointer;   //!< Pointer to parent class.

        /*! Default constructor.
        * \param parent The parent.
        * \throws std::invalid_argument Thrown if parent pointer is invalid.
        */
        explicit child(parent_pointer const parent = 0) {
            set_parent(parent);
        }

        /*! Copy constructor.
        * \param other The other child instance to copy from.
        */
        child(const child &other) {
            set_parent(other.parent());
        }

        /*! Set a new parent.
        * \param parent Pointer to the new parent.
        * \throws std::invalid_argument Thrown if parent pointer is invalid.
        */
        void set_parent(parent_pointer const parent) {
            if (!parent) {
                std::invalid_argument e("parent argument is null");
                boost::throw_exception(e);
            };
            m_parent1 = parent;
            m_parent2 = parent;
            m_parent3 = parent;
        }

        /*! Accessor to get the chunk's parent.
        * \return Pointer to the parent.
        */
        parent_pointer parent() const { check_parent(); return m_parent1; }

        /*! Validity check that tries to correct minor faults silently.
        * \param parent Optional pointer to the parent to check against.
        * \return true, if the internal structure and data is valid.
        * \see check_parent()
        */
        bool is_valid(parent_pointer const parent = 0) const {
#ifdef BOOST_SELF_HEALING_DEBUG
            std::cout << "boost::self_healing::child<P>::is_valid()" << std::endl;
#endif
            try {
                check_parent(parent);
                return true;
            } catch (const std::runtime_error &e) {
#ifdef BOOST_SELF_HEALING_DEBUG
                std::cout << "boost::self_healing::child<P>::is_valid() caught runtime error: " << e.what() << std::endl;
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
            std::cout << "boost::self_healing::child<P>::check_parent(" << parent << ")" << std::endl;
#endif
            if (parent) {
                // If a valid parent pointer was given we simply check against
                // it and fix the internal pointer if needed.
                if (m_parent1 != parent) { const_cast<parent_pointer &>(m_parent1) = parent; }
                if (m_parent2 != parent) { const_cast<parent_pointer &>(m_parent2) = parent; }
                if (m_parent3 != parent) { const_cast<parent_pointer &>(m_parent3) = parent; }
            } else {
                // check and repair size via TMR voting
                const bool equal_13 = m_parent1 == m_parent3;
                const bool equal_23 = m_parent2 == m_parent3;
                const bool equal_12 = m_parent2 == m_parent2;

                if (equal_12 && equal_13 && equal_23) {
                    // all fine
                } else if (equal_13) {
                    const_cast<parent_pointer &>(m_parent2) = m_parent1; // fix m_parent2 as the others are equal
                } else if (equal_23) {
                    const_cast<parent_pointer &>(m_parent1) = m_parent2; // fix m_parent1 as the others are equal
                } else if (equal_12) {
                    const_cast<parent_pointer &>(m_parent3) = m_parent1; // fix m_parent3 as the others are equal
                } else {
                    std::runtime_error e("parent error");
                    boost::throw_exception(e);
                }
            }
        }

        parent_pointer m_parent1;   //!< Pointer to the parent class instance.
        parent_pointer m_parent2;   //!< Pointer to the parent class instance.
        parent_pointer m_parent3;   //!< Pointer to the parent class instance.
    };

} } // namespace boost::self_healing


#endif // BOOST_SELF_HEALING_CHILD_HPP

