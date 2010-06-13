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

#include "child.hpp"
#include "../checksummed_array.hpp"

#include <boost/config.hpp>
#include <boost/detail/workaround.hpp>

#ifdef BOOST_SELF_HEALING_DEBUG
#   include <iostream>
#endif


/// The namespace self_healing contains fault-tolerant data structures and utility classes.
namespace boost { namespace self_healing {

    template <class T, std::size_t CS>
    class vector; // forward declaration

    /*! \brief Vector element storage chunk.
    *
    * A chunk is a checksummed_array and a child and part of the self-healing
    * vector storage mechanism
    *
    * \param T The data type of the stored values.
    * \param CS Optional storage capacity of the chunk.
    * \throws std::invalid_argument Thrown if parent pointer is invalid.
    * \see child, checksummed_array
    */
    template <class T, std::size_t CS = 64>
    class vector_chunk : public child<vector<T, CS> >, public checksummed_array<T, CS>
    {
    public:
        // type definitions
        typedef T               value_type;         //!< The type of elements stored in the <code>checksummed_array</code>.
        typedef const T &       const_reference;    //!< A const reference to an element.
        typedef vector<T, CS>   parent_type;
        typedef vector<T, CS> * parent_pointer;

        /*! Default constructor.
        * \param parent The parent vector.
        * \param value An initial value that is set for all elements.
        */
        explicit vector_chunk(parent_pointer const parent = 0, const_reference value = 0)
            : child<parent_type>(parent), checksummed_array<value_type, CS>(value) {}

        /*! Validity check that tries to correct minor faults silently.
        * \param parent An optional pointer to the parent to check against.
        * \return true, if the internal structure and data is valid.
        * \see check_parent()
        */
        bool is_valid(parent_pointer const parent = 0) const {
#ifdef BOOST_SELF_HEALING_DEBUG
            std::cout << "boost::self_healing::vector_chunk<T, CS>::is_valid()" << std::endl;
#endif
            // call the is_valid methods of all base classes.
            return child<parent_type>::is_valid(parent) &&
                   checksummed_array<value_type, CS>::is_valid();
        }
    };

} } // namespace boost::self_healing


#endif // BOOST_SELF_HEALING_VECTOR_CHUNK_HPP
