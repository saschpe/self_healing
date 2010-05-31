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
        * \param next Pointer to the next sibling
        * \param previous Pointer to the previous sibling
        * \throws std::invalid_argument Thrown if a sibling pointer is invalid.
        */
        explicit sibling(sibling_pointer const next = NULL, sibling_pointer const previous = NULL)
            : m_next(next), m_previous(previous) {}

        /*! Copy constructor.
        * \param other The other sibling instance to copy from.
        */
        sibling(const sibling &other)
            : m_next(other.m_next), m_previous(other.m_prev) {}

        /*! Validity check that tries to correct minor faults silently.
        * \param next Pointer to the next sibling
        * \param previous Pointer to the previous sibling
        * \return true, if the internal structure and data is valid.
        * \see check_siblings()
        */
        bool is_valid(sibling_pointer const next = NULL, sibling_pointer const previous = NULL) const {
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
        /*! TODO.
        */
        void check_siblings(sibling_pointer const next = NULL, sibling_pointer const previous = NULL) const {
#ifdef BOOST_SELF_HEALING_DEBUG
            std::cout << "boost::self_healing::sibling<P>::check_siblings" << std::endl;
#endif
            //TODO:
        }

        sibling_pointer m_next;
        sibling_pointer m_previous;
    };

} } // namespace boost::self_healing


#endif // BOOST_SELF_HEALING_SIBLING_HPP


