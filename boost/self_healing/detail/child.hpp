/*! \file
* \brief Child.
*
* This file contains the class child.
*
* (C) Copyright Sascha Peilicke 2010.
*
* Distributed under the Boost Software License, Version 1.0. (See
* accompanying file LICENSE_1_0.txt or copy at
* http://www.boost.org/LICENSE_1_0.txt)
*
* 23 April 2010 - Initial Revision (Sascha Peilicke)
*/

#ifndef BOOST_SELF_HEALING_CHILD_HPP
#define BOOST_SELF_HEALING_CHILD_HPP

#include <boost/detail/workaround.hpp>

#include <stdexcept>
#include <boost/throw_exception.hpp>

// FIXES for broken compilers
#include <boost/config.hpp>


/// The namespace self_healing contains fault-tolerant data structures and utility classes.
namespace boost { namespace self_healing {

    /*! A class to express parent-child relationship.
    * \param P The type of the parent data structure.
    * \see checksummed_array
    */
    template <class P>
    class child
    {
    public:
        // type definitions
        typedef P         parent_type;      //!< The type of the parent class.
        typedef P *       parent_pointer;   //!< Pointer to parent class.

        /*! Constructor.
        * \param parent The parent.
        * \throws std::invalid_argument Thrown if parent pointer is invalid.
        */
        explicit child(parent_pointer const parent)
            : m_parent(parent) {
            if (!parent) {
                std::invalid_argument e("parent is NULL");
                boost::throw_exception(e);
            }
        }

        /*! Set a new parent.
        * \param parent Pointer to the new parent.
        * \throws std::invalid_argument Thrown if parent pointer is invalid.
        */
        void setParent(parent_pointer const parent) { check_parent(parent); }

        /*! Accessor to get the chunk's parent.
        * \return Pointer to the parent.
        */
        parent_pointer parent() const { return m_parent; }

        /*! Validity check that tries to correct minor faults silently.
        * \param parent An optional pointer to the parent to check against.
        * \return true, if the internal structure and data is valid.
        * \see check_parent()
        */
        virtual bool is_valid(parent_pointer const parent = NULL) const {
            try {
                check_parent(parent);
                return true;
            } catch (const std::runtime_error &) {
                return false;
            };
        }

    private:
        /*! TODO.
        * \throws std::invalid_argument Thrown if parent pointer is invalid.
        */
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
                    std::runtime_error e("parent is NULL");
                    boost::throw_exception(e);
                }
            }
        }

        parent_pointer m_parent;    //!< Pointer to the parent class instance.
    };

} } // namespace boost::self_healing


#endif // BOOST_SELF_HEALING_CHILD_HPP

