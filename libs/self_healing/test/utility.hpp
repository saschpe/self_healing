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

    static const boost::rand48 s_rng;   //!< Fast number generator used by several functions.

    /*! Function to randomly flip bits in a given memory region.
    * \param obj Pointer to an object of type T.
    * \param bit_count Amount of bits to flip.
    */
    template <class T>
    void flip_bits(T * const obj, std::size_t bit_count = 1)
    {
        // a random number generator to select bits within 'obj'
        static const boost::uniform_int<> dist(0, sizeof(T) * 8 - 1);
        static boost::variate_generator<boost::rand48 &, boost::uniform_int<> > dice(s_rng, dist);

        for (std::size_t i = 0; i < bit_count; i++) {
            // determine the bit to flip and bit-manipulate the containing byte
            const std::size_t bit_to_flip = dice();
            const char &byte_to_modify = obj + bit_to_flip / 8;

            byte_to_modify ^= 1 << (bit_to_flip % 8);
        }
    }

    /*! Function to randomly flip bits in a consecutive region (burst).
    * \param Pointer to an object of type T.
    * \param burst_length Length of burst error.
    */
    template <class T>
    void burst_flip_bits(T * const obj, std::size_t burst_length = 5)
    {
        if (burst_length == 0) {
            return;
        }
        if (burst_length >= sizeof(T) * 8) {
            // if burst range is bigger than the bit-size of T set it to allowed maximum
            burst_length = sizeof(T) * 8;
        }

        // a random number generator to generate the starting bit position within 'obj'
        static const boost::uniform_int<> dist(0, sizeof(T) * 8 - 1 - burst_length);
        static boost::variate_generator<boost::rand48 &, boost::uniform_int<> > dice(s_rng, dist);
        const std::size_t start_pos = dice();

        for (std::size_t i = 0; i < burst_length; i++) {
            // determine containing byte and flip the current bit
            const char &byte_to_modify = obj + (start_pos + i) / 8;

            byte_to_modify ^= 1 << ((start_pos + i) % 8);
        }
    }

    /*! Function to change the value of a variable randomly.
    * \param var The variable to change.
    */
    template <class T>
    void change_value(T * const var)
    {
        static const boost::uniform_int<> dist(std::numeric_limits<T>::min(), std::numeric_limits<T>::max());
        static boost::variate_generator<boost::rand48 &, boost::uniform_int<> > dice(s_rng, dist);

        *var = dice(); // apply random number to target
    }

} // namespace utility


#endif // UTILITY_HPP
