/*! \file
* \brief Sized.
*
* This file contains the class sized.
*
* (C) Copyright Sascha Peilicke 2010.
*
* Distributed under the Boost Software License, Version 1.0. (See
* accompanying file LICENSE_1_0.txt or copy at
* http://www.boost.org/LICENSE_1_0.txt)
*
* 16 July 2010 - Initial Revision (Sascha Peilicke)
*/

#ifndef BOOST_SELF_HEALING_SIZED_HPP
#define BOOST_SELF_HEALING_SIZED_HPP

#include <boost/throw_exception.hpp>

#ifdef BOOST_SELF_HEALING_DEBUG
#   include <iostream>
#endif
#include <stdexcept>


/// The namespace self_healing contains fault-tolerant data structures and utility classes.
namespace boost { namespace self_healing {

    /*! A fault-tolerant class to express size.
    */
    class sized
    {
    public:
        typedef std::size_t size_type; //!< An unsigned integral type that can represent any non-negative value of the container's distance type.

        sized()
            : size1(0), size2(0), size3(0) {}

        void set_size(size_type size) {
            size1 = size;
            size2 = size;
            size3 = size;
        }

        size_type size() const { check_size(); return size1; }

        /*! Validity check that tries to correct minor faults silently.
        * \return true, if the internal structure and data is valid
        * \see check_size
        */
        bool is_valid() const {
#ifdef BOOST_SELF_HEALING_DEBUG
            std::cout << "boost::self_healing::sized::is_valid()" << std::endl;
#endif
            try {
                // check all parts of the data structure
                check_size();
                return true;
            } catch (const std::runtime_error &e) {
#ifdef BOOST_SELF_HEALING_DEBUG
                std::cout << "boost::self_healing::sized::is_valid() caught runtime error: " << e.what() << std::endl;
#endif
                return false;
            };
        }
    private:
        void check_size() const {
#ifdef BOOST_SELF_HEALING_DEBUG
            std::cout << "boost::self_healing::vector<T, ChunkSize>::check_size()" << std::endl;
#endif
            // check and repair size via TMR voting
            const bool equal_13 = size1 == size3;
            const bool equal_23 = size2 == size3;
            const bool equal_12 = size1 == size2;

            if (equal_12 && equal_13 && equal_23) {
                // all fine
            } else if (equal_13) {
                const_cast<size_type &>(size2) = size1; // fix m_size1 as the others are equal
            } else if (equal_23) {
                const_cast<size_type &>(size1) = size2; // fix m_size1 as the others are equal
            } else if (equal_12) {
                const_cast<size_type &>(size3) = size1; // fix m_size3 as the others are equal
            } else {
                std::runtime_error e("size error"); // all three sizes differ
                boost::throw_exception(e);
            }

            /*if (size1 > capacity()) {
                std::runtime_error e("size is bigger than capacity");
                boost::throw_exception(e);
            }*/
        }

        size_type size1;            //!< Counts how much elements are stored currently in all chunks.
        size_type size2;
        size_type size3;
    };

} } // namespace boost::self_healing


#endif // BOOST_SELF_HEALING_SIZED_HPP
