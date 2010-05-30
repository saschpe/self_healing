/* Utility methods for unit-tests.
 *
 * This file contains several methods to simulate faults and errors.
 *
 * (C) Copyright Sascha Peilicke 2010.
 *
 * Distributed under the Boost Software License, Version 1.0. (See
 * accompanying file LICENSE_1_0.txt or copy at
 * http://www.boost.org/LICENSE_1_0.txt)
 *
 * 30 May 2010 - Initial Revision (Sascha Peilicke)
 */

#ifndef UTILITY_HPP
#define UTILITY_HPP

#include <boost/detail/workaround.hpp>

#include <boost/random.hpp>

// FIXES for broken compilers
#include <boost/config.hpp>


/// The namespace utility contains utility functions to provoke errors.
namespace utility {

    template <class DS>
    void flipped_bits(DS * const start, std::size_t size, std::size_t bit_count = 1)
    {
        //TODO:
    }

    template <class DS>
    void burst_error(DS * const start, std::size_t size, std::size_t burst_length)
    {
        //TODO:
    }

    /*! Function to change the value of a variable randomly.
    * \param var The variable to change.
    * \param var The variable to change.
    */
    template <class T>
    void value_change(T * const var)
    {
        // build a nice random number generator
        boost::mt19937 rng;
        boost::uniform_int<> dist(std::numeric_limits<T>::min(), std::numeric_limits<T>::max());
        boost::variate_generator<boost::mt19937 &, boost::uniform_int<> > dice(rng, dist);

        // apply random number to target
        *var = dice();
    }

} // namespace utility


#endif // UTILITY_HPP
