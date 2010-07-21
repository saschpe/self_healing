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

#include <boost/throw_exception.hpp>

#ifdef BOOST_SELF_HEALING_DEBUG
#   include <iostream>
#endif
#include <stdexcept>


/// The namespace self_healing contains fault-tolerant data structures and utility classes.
namespace boost { namespace self_healing {

    /*! A class to express sibling relationship between class instances of the
    * same type.  This class is not inherently fault-tolerant, but meant to be
    * part of other classes.
    */
    template <class T>
    class sibling
    {
    public:
        // type definitions
        typedef T   sibling_type;       //!< The type of the sibling class.
        typedef T * sibling_pointer;    //!< Pointer to sibling class.

        /*! Default constructor.
        * \param next Pointer to the next sibling.
        * \param previous Pointer to the previous sibling.
        * \throws std::invalid_argument Thrown if a sibling pointer is invalid.
        */
        explicit sibling(sibling_pointer const next = 0, sibling_pointer const previous = 0)
            : next1(next), previous1(previous) {}

        /*! Set a new next sibling.
        * \param next Pointer to the new next sibling.
        * \throws std::invalid_argument Thrown if parent pointer is invalid.
        */
        void set_next(sibling_pointer const next) {
            if (!next) {
                std::invalid_argument e("next sibling argument is null");
                boost::throw_exception(e);
            };
            next1 = next;
        }

        /*! Accessor to get the next sibling.
        * \return Pointer to the next sibling.
        */
        sibling_pointer next() const { return next1; }

        /*! Set a new previous sibling.
        * \param next Pointer to the new previous sibling.
        */
        void set_previous(sibling_pointer const previous) {
            if (!previous) {
                std::invalid_argument e("previous sibling argument is null");
                boost::throw_exception(e);
            };
            previous1 = previous;
        }

        /*! Accessor to get the previous sibling.
        * \return Pointer to the previous sibling.
        */
        sibling_pointer previous() const { return previous1; }

        /*! Validity check that needs a sibling pointers to compare with.
        * \param next Pointer to the next sibling to check against.
        * \param previous Pointer to the previous sibling to check against.
        * \return true, if the internal structure and data is valid.
        * \see check_siblings()
        */
        bool is_valid(sibling_pointer const next = 0, sibling_pointer const previous = 0) const {
#ifdef BOOST_SELF_HEALING_DEBUG
            std::cout << "boost::self_healing::sibling<P>::is_valid()" << std::endl;
#endif
            if (next) {
                // If a valid next sibling pointer was given we simply check
                // against it and fix the internal pointer if needed.
                if (next != next1) { const_cast<sibling_pointer &>(next1) = next; }
            } else {
                return next1 != 0;
            }
            if (previous) {
                // If a valid next sibling pointer was given we simply check
                // against it and fix the internal pointer if needed.
                if (previous != previous1) { const_cast<sibling_pointer &>(previous1) = previous; }
            } else {
                return next1 != 0;
            }
        }

    private:
        sibling_pointer next1;
        sibling_pointer previous1;
    };

} } // namespace boost::self_healing


#endif // BOOST_SELF_HEALING_SIBLING_HPP
