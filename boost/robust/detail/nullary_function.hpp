/*! \file
* \brief Nullary functions.
*
* This file contains the base class nullary_function, an abstract base
* for function objects (functors).
*
* (C) Copyright Sascha Peilicke 2010.
*
* Distributed under the Boost Software License, Version 1.0. (See
* accompanying file LICENSE_1_0.txt or copy at
* http://www.boost.org/LICENSE_1_0.txt)
*
* 5 May 2010 - Initial Revision (Sascha Peilicke)
*
* http://github.com/saschpe/robust
*/

#ifndef BOOST_ROBUST_NULLARY_FUNCTION_HPP
#define BOOST_ROBUST_NULLARY_FUNCTION_HPP

// FIXES for broken compilers
#include <boost/config.hpp>


/// The namespace robust contains fault-tolerant data structures and utility classes.
namespace boost { namespace robust {

    /*! \brief Nullary function.
    *
    * A base class defining a common interface for nullary function objects.
    *
    * \remarks TODO.
    *
    * \see http://en.wikipedia.org/wiki/Function_object#In_C_and_C.2B.2B
    */
    struct nullary_function
    {
        /*! TODO.
        */
        virtual void operator()() {};
    };

    /*! \brief Empty nullary function.
    *
    * An empty default nullary function object instance which can be used as a
    * default value for functions that take a functor parameter.
    *
    * \see nullary_function
    */
    static nullary_function empty_nullary_function;

} } // namespace boost::robust


#endif // BOOST_ROBUST_NULLARY_FUNCTION_HPP
