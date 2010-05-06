/*! \file
* \brief Functor.
*
* This file contains the abstract base class functor, an abstract base for
* function objects (functors).
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

#ifndef BOOST_ROBUST_FUNCTOR_HPP
#define BOOST_ROBUST_FUNCTOR_HPP

// FIXES for broken compilers
#include <boost/config.hpp>


/// The namespace robust contains fault-tolerant data structures and utility classes.
namespace boost { namespace robust {

    /*! \brief Functor.
    *
    * An abstract functor base class defining a common interface for function objects.
    *
    * \remarks TODO.
    *
    * \see http://en.wikipedia.org/wiki/Function_object#In_C_and_C.2B.2B
    */
    class functor
    {
    public:
        /*! TODO.
        */
        virtual void operator()() = 0;
    };

    /*! \brief Empty functor.
    *
    * An empty default functor instance which can be used as a default value for
    * functions that take a functor parameter.
    *
    * \see Functor: functor
    */
    static class void_functor : public functor
    {
    public:
        /*! TODO.
        */
        void operator()() {}
    } void_functor;

} } // namespace boost::robust


#endif // BOOST_ROBUST_FUNCTOR_HPP
