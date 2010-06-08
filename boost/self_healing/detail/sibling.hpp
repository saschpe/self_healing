/*! \file
* \brief Sibling.
*
* This file contains the class sibling.
*
* (C) Copyright Sascha Peilicke 2010.
*
* Distributed under the Boost Software License, Version 1.0. (See
* accompanying file LICENSE_1_0.txt or copy at
* http://www.boost.org/LICENSE_1_0.txt)
*
* 31 May 2010 - Initial Revision (Sascha Peilicke)
*/

#ifndef BOOST_SELF_HEALING_SIBLING_HPP
#define BOOST_SELF_HEALING_SIBLING_HPP

#include <boost/config.hpp>
#include <boost/detail/workaround.hpp>
#include <boost/throw_exception.hpp>

#ifdef BOOST_SELF_HEALING_DEBUG
#   include <iostream>
#endif
#include <stdexcept>


/// The namespace self_healing contains fault-tolerant data structures and utility classes.
namespace boost { namespace self_healing {

    /*! A class to express sibling relationship between class instances of the same type.
    */
    template <class T>
    class sibling
    {
    public:
        // type definitions
        typedef T         sibling_type;     //!< The type of the sibling class.
        typedef T *       sibling_pointer;  //!< Pointer to sibling class.

        /*! Default constructor.
        * \param next Pointer to the next sibling.
        * \param previous Pointer to the previous sibling.
        * \throws std::invalid_argument Thrown if a sibling pointer is invalid.
        */
        explicit sibling(sibling_pointer const next = 0, sibling_pointer const previous = 0) {
            set_next(next);
            set_previous(previous);
        }

        /*! Copy constructor.
        * \param other The other sibling instance to copy from.
        */
        sibling(const sibling &other) {
            set_next(other.next());
            set_previous(other.previous());
        }

        /*! Set a new next sibling.
        * \param next Pointer to the new next sibling.
        * \throws std::invalid_argument Thrown if parent pointer is invalid.
        */
        void set_next(sibling_pointer const next) {
            if (!next) {
                std::invalid_argument e("next sibling argument is null");
                boost::throw_exception(e);
            };
            m_next1 = next;
            m_next2 = next;
            m_next3 = next;
        }

        /*! Accessor to get the next sibling.
        * \return Pointer to the next sibling.
        */
        sibling_pointer next() const { check_siblings(); return m_next1; }

        /*! Set a new previous sibling.
        * \param next Pointer to the new previous sibling.
        */
        void set_previous(sibling_pointer const previous) {
            if (!previous) {
                std::invalid_argument e("previous sibling argument is null");
                boost::throw_exception(e);
            };
            m_previous1 = previous;
            m_previous2 = previous;
            m_previous3 = previous;
        }

        /*! Accessor to get the previous sibling.
        * \return Pointer to the previous sibling.
        */
        sibling_pointer previous() const { check_siblings(); return m_previous1; }

        /*! Validity check that tries to correct minor faults silently.
        * \param next Optional pointer to the next sibling to check against.
        * \param previous Optional pointer to the previous sibling to check against.
        * \return true, if the internal structure and data is valid.
        * \see check_siblings()
        */
        bool is_valid(sibling_pointer const next = 0, sibling_pointer const previous = 0) const {
#ifdef BOOST_SELF_HEALING_DEBUG
            std::cout << "boost::self_healing::sibling<P>::is_valid()" << std::endl;
#endif
            try {
                check_siblings(next, previous);
                return true;
            } catch (const std::runtime_error &e) {
#ifdef BOOST_SELF_HEALING_DEBUG
                std::cout << "boost::self_healing::sibling<P>::is_valid() caught runtime error: " << e.what() << std::endl;
#endif
                return false;
            };
        }

    private:
        /*! Checks the internal sibling pointers against the provided ones.
        * \throws std::invalid_argument Thrown if a sibling pointer is invalid.
        */
        void check_siblings(sibling_pointer const next = 0, sibling_pointer const previous = 0) const {
#ifdef BOOST_SELF_HEALING_DEBUG
            std::cout << "boost::self_healing::sibling<P>::check_siblings" << std::endl;
#endif
            if (next) {
                // If a valid next sibling pointer was given we simply check against
                // it and fix the internal pointer if needed.
                if (m_next1 != next) { const_cast<sibling_pointer &>(m_next1) = next; }
                if (m_next2 != next) { const_cast<sibling_pointer &>(m_next2) = next; }
                if (m_next3 != next) { const_cast<sibling_pointer &>(m_next3) = next; }
            } else {
                // check and repair size via TMR voting
                const bool equal_13 = m_next1 == m_next3;
                const bool equal_23 = m_next2 == m_next3;
                const bool equal_12 = m_next2 == m_next2;

                if (equal_12 && equal_13 && equal_23) {
                    // all fine
                } else if (equal_13) {
                    const_cast<sibling_pointer &>(m_next2) = m_next1; // fix m_next2 as the others are equal
                } else if (equal_23) {
                    const_cast<sibling_pointer &>(m_next1) = m_next2; // fix m_next1 as the others are equal
                } else if (equal_12) {
                    const_cast<sibling_pointer &>(m_next3) = m_next1; // fix m_next3 as the others are equal
                } else {
                    std::runtime_error e("parent error");
                    boost::throw_exception(e);
                }
            }

            if (previous) {
                // If a valid previous sibling pointer was given we simply check against
                // it and fix the internal pointer if needed.
                if (m_previous1 != previous) { const_cast<sibling_pointer &>(m_previous1) = previous; }
                if (m_previous2 != previous) { const_cast<sibling_pointer &>(m_previous2) = previous; }
                if (m_previous3 != previous) { const_cast<sibling_pointer &>(m_previous3) = previous; }
            } else {
                // check and repair size via TMR voting
                const bool equal_13 = m_previous1 == m_previous3;
                const bool equal_23 = m_previous2 == m_previous3;
                const bool equal_12 = m_previous2 == m_previous2;

                if (equal_12 && equal_13 && equal_23) {
                    // all fine
                } else if (equal_13) {
                    const_cast<sibling_pointer &>(m_previous2) = m_previous1; // fix m_previous2 as the others are equal
                } else if (equal_23) {
                    const_cast<sibling_pointer &>(m_previous1) = m_previous2; // fix m_previous1 as the others are equal
                } else if (equal_12) {
                    const_cast<sibling_pointer &>(m_previous3) = m_previous1; // fix m_previous3 as the others are equal
                } else {
                    std::runtime_error e("parent error");
                    boost::throw_exception(e);
                }
            }
        }

        sibling_pointer m_next1;
        sibling_pointer m_previous1;
        sibling_pointer m_next2;
        sibling_pointer m_previous2;
        sibling_pointer m_next3;
        sibling_pointer m_previous3;
    };

} } // namespace boost::self_healing


#endif // BOOST_SELF_HEALING_SIBLING_HPP


