/*! \file
* \brief Vector chunk.
*
* This file contains the class vector_chunk.
*
* (C) Copyright Sascha Peilicke 2010.
*
* Distributed under the Boost Software License, Version 1.0. (See
* accompanying file LICENSE_1_0.txt or copy at
* http://www.boost.org/LICENSE_1_0.txt)
*
* 23 April 2010 - Initial Revision (Sascha Peilicke)
*/

#ifndef BOOST_SELF_HEALING_VECTOR_CHUNK_HPP
#define BOOST_SELF_HEALING_VECTOR_CHUNK_HPP

#include <boost/detail/workaround.hpp>

#ifdef BOOST_SELF_HEALING_DEBUG
#include <iostream>
#endif

// FIXES for broken compilers
#include <boost/config.hpp>

#include "child.hpp"
#include "../checksummed_array.hpp"


/// The namespace self_healing contains fault-tolerant data structures and utility classes.
namespace boost { namespace self_healing {

    template <class T, std::size_t N>
    class vector;

    /*! \brief Vector element storage chunk.
    *
    * A chunk is a checksummed_array and a child and part of the self-healing
    * vector storage mechanism
    *
    * \param T The data type of the stored values.
    * \param N The capacity of the chunk.
    * \throws std::invalid_argument Thrown if parent pointer is invalid.
    * \see checksummed_array
    */
    template <class T, std::size_t N = 64>
    class vector_chunk : public child<vector<T, N> >, public checksummed_array<T, N>
    {
    public:
        // type definitions
        typedef T         value_type;       //!< The type of elements stored in the <code>checksummed_array</code>.
        typedef const T & const_reference;  //!< A const reference to an element.

        /*! Default constructor.
        * \param parent The parent.
        * \param value An initial value that is set for all elements.
        */
        explicit vector_chunk(vector<value_type, N> *const parent = NULL, const_reference value = 0)
            : child<vector<value_type, N> >(parent), checksummed_array<value_type, N>(value) {}

        /*! Validity check that tries to correct minor faults silently.
        * \param parent An optional pointer to the parent to check against.
        * \return true, if the internal structure and data is valid.
        * \see check_parent()
        */
        bool is_valid(vector<value_type, N> *const parent = NULL) const {
#ifdef BOOST_SELF_HEALING_DEBUG
            std::cout << "boost::self_healing::vector_chunk<T, N>::is_valid()" << std::endl;
#endif
            // call the is_valid methods of all base classes.
            return child<vector<value_type, N> >::is_valid(parent) &&
                   checksummed_array<value_type, N>::is_valid();
        }
    };

} } // namespace boost::self_healing


#endif // BOOST_SELF_HEALING_VECTOR_CHUNK_HPP
