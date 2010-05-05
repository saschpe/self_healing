/* The following code declares class functor,
 * an abstract base for function objects (functors).
 *
 * The original author site is at: http://saschpe.wordpress.com/
 *
 * (C) Copyright Sascha Peilicke 2010.
 *
 * Distributed under the Boost Software License, Version 1.0. (See
 * accompanying file LICENSE_1_0.txt or copy at
 * http://www.boost.org/LICENSE_1_0.txt)
 *
 * 5 May 2010 - Initial Revision (Sascha Peilicke)
 *
 * May 5th, 2010
 */
#ifndef BOOST_ROBUST_FUNCTOR_HPP
#define BOOST_ROBUST_FUNCTOR_HPP

// FIXES for broken compilers
#include <boost/config.hpp>


namespace robust {

    /**
     * Abstract functor base class.
     */
    class functor
    {
    public:
        virtual void operator()() = 0;
    };

    /**
     * Empty default functor instance suitable as a default value.
     */
    static class void_functor : public functor {
    public:
        void operator()() {}
    } void_functor;

} // namespace robust


#endif /* BOOST_ROBUST_FUNCTOR_HPP */
