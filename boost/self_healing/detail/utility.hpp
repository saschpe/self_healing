/*! \file
* \brief Various utility functions.
*
* This file contains the function to_string().
*
* (C) Copyright Sascha Peilicke 2010.
*
* Distributed under the Boost Software License, Version 1.0. (See
* accompanying file LICENSE_1_0.txt or copy at
* http://www.boost.org/LICENSE_1_0.txt)
*
* 5 May 2010 - Initial Revision (Sascha Peilicke)
*/

#ifndef BOOST_SELF_HEALING_UTILITY_HPP
#define BOOST_SELF_HEALING_UTILITY_HPP

#include <boost/config.hpp>

#include <sstream>

#ifdef BOOST_HAS_UNISTD_H
#include <unistd.h>
#endif


/// The namespace self_healing contains fault-tolerant data structures and utility classes.
namespace boost { namespace self_healing {

    /*! Converts any type into a std::string.
    * \param value The value to convert.
    * \return The value as a string.
    */
    template <class T>
    inline std::string to_string(const T &value)
    {
        std::stringstream ss;
        ss << value;
        return ss.str();
    }

    /*! Checks if a heap address is within allocated memory.
    *
    * Note: This function currently only works for UNIX-like operating systems.
    *
    * \param addr The address to check.
    */
    inline bool is_valid_heap_address(void *addr)
    {
#ifdef BOOST_HAS_UNISTD_H
        return addr < sbrk(0);
#else
        return false
#endif
    }

} } // namespace boost::self_healing


#endif // BOOST_SELF_HEALING_UTILITY_HPP

