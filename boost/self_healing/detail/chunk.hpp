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
    * \throws std::invalid_argument Thrown if parent pointer is invalid.
    * \see checksummed_array
    */
    template <class T, class P, std::size_t N = 64>
    class chunk : public checksummed_array<T, N>
    {
    public:
        // type definitions
        typedef T         value_type;       //!< The type of elements stored in the <code>checksummed_array</code>.
        typedef const T & const_reference;  //!< A const reference to an element.
        typedef P         parent_type;      //!<
        typedef P *       parent_pointer;   //!<

        /*! Constructor.
        * \param parent The parent.
        * \param value An initial value that is set for all elements.
        */
        explicit chunk(parent_pointer const parent, const_reference value = 0)
            : checksummed_array<value_type, N>(value), m_parent(parent) {
            if (!parent) {
                std::invalid_argument e("parent is NULL");
                boost::throw_exception(e);
            }
        }

        /*! Validity check that tries to correct minor faults silently.
        * \param parent An optional pointer to the parent to check against.
        * \return true, if the internal structure and data is valid.
        * \see check_parent()
        */
        bool is_valid(parent_pointer const parent = NULL) const {
            try {
                check_parent(parent);
                return static_cast<const checksummed_array<value_type, N> *>(this)->is_valid();
            } catch (const parent_error &e) {
                return false;
            };
        }

        /*! Set a new parent.
        * \param parent Pointer to the new parent.
        */
        void setParent(parent_pointer const parent) { check_parent(parent); }

        /*! Accessor to get the chunk's parent.
        * \return Pointer to the parent.
        */
        parent_pointer parent() const { return m_parent; }

    private:
        void check_parent(parent_pointer const parent) const {
            if (parent) {
                // If a valid parent pointer was given we simply check against
                // it and fix the internal pointer if needed.
                if (m_parent != parent) {
                    const_cast<parent_pointer &>(m_parent) = parent;
                }
            } else {
                // No pointer was given to check against, so we can do only limited checks
                if (!m_parent) {
                    parent_error e("parent is NULL");
                    boost::throw_exception(e);
                }
            }
        }

        parent_pointer m_parent;    //!< Pointer to the parent class instance.
    };

} } // namespace boost::self_healing


#endif // BOOST_SELF_HEALING_CHUNK_HPP
