/*! \file
* \brief Chunk.
*
* This file contains the class chunk.
*
* (C) Copyright Sascha Peilicke 2010.
*
* Distributed under the Boost Software License, Version 1.0. (See
* accompanying file LICENSE_1_0.txt or copy at
* http://www.boost.org/LICENSE_1_0.txt)
*
* 23 April 2010 - Initial Revision (Sascha Peilicke)
*/

#ifndef BOOST_SELF_HEALING_CHUNK_HPP
#define BOOST_SELF_HEALING_CHUNK_HPP

#include <boost/detail/workaround.hpp>

// FIXES for broken compilers
#include <boost/config.hpp>

#include "../checksummed_array.hpp"


/// The namespace self_healing contains fault-tolerant data structures and utility classes.
namespace boost { namespace self_healing {

    /*! Exception that is thrown when a parent error happened.
    */
    class parent_error : public std::runtime_error
    {
    public:
        explicit parent_error(const std::string &what_arg)
            : std::runtime_error(what_arg) {}
    };

    /*! \brief Element storage chunk.
    *
    * A chunk is a checksummed_array with a pointer to it's parent and to be
    * used as part of other data structures.
    *
    * \param T The data type of the stored values.
    * \param P The type of the parent data structure.
    * \param N The size of the chunk.
    *
    * \see checksummed_array
    */
    template <class T, class P, std::size_t N = 64>
    class chunk : public checksummed_array<T, N>
    {
    public:
        explicit chunk(P * const parent, const T &value = 0)
            : checksummed_array<T, N>(value), m_parent(parent) {}

        virtual bool is_valid(P * const parent = NULL) const {
            try {
                check_and_repair_parent(parent);
                return static_cast<const checksummed_array<T, N> *>(this)->is_valid();
            } catch (const parent_error &e) {
                return false;
            };
        }

        void setParent(P * const parent) { m_parent = parent; }
        P *parent() const { return m_parent; }

    private:
        void check_and_repair_parent(P * const parent) const {
            if (parent) {
                if (m_parent != parent) {
                    const_cast<P *&>(m_parent) = parent;
                }
            }
        }

        P *m_parent;  //!< Pointer to the parent class instance.
    };

} } // namespace boost::self_healing


#endif // BOOST_SELF_HEALING_CHUNK_HPP
