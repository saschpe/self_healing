/*! \file
* \brief Reference.
*
* This file contains the class reference_wrapper, which defines a wrapper of a
* reference to a value that calls a function object on value updates.
*
* (C) Copyright Sascha Peilicke 2010.
*
* Distributed under the Boost Software License, Version 1.0. (See
* accompanying file LICENSE_1_0.txt or copy at
* http://www.boost.org/LICENSE_1_0.txt)
*
* 5 May 2010 - Initial Revision (Sascha Peilicke)
*/

#ifndef BOOST_SELF_HEALING_REFERENCE_WRAPPER_HPP
#define BOOST_SELF_HEALING_REFERENCE_WRAPPER_HPP

#include <boost/config.hpp>
#include <boost/function.hpp>

#include <ostream>


/// The namespace self_healing contains fault-tolerant data structures and utility classes.
namespace boost { namespace self_healing {

    /*! \brief Reference wrapper.
    *
    * A class wrapping a reference to a value which optionally takes a function
    * object that is called every time the value changes.
    *
    * \param T The data type of the stored value.
    */
    template <class T>
    class reference_wrapper
    {
    public:
        // type definitions
        typedef T                    value_type;        //!< The type of elements stored in the <code>checksummed_array</code>.
        typedef T &                  reference;
        typedef const T &            const_reference;   //!< A const reference to an element.

        /*! Constructor.
        * \param value The value to initialize the reference with.
        * \param update The function to apply if the value is changed.
        */
        explicit reference_wrapper(reference value, boost::function<void (void)> update)
            : value(value), update(update) {}

        /*! Copy constructor.
        * \param other The other reference instance to copy from.
        */
        reference_wrapper(const reference_wrapper &other)
            : value(other.value), update(other.update) {}

        reference_wrapper& operator=(const_reference rhs) { value = rhs; update(); return *this; }
        reference_wrapper& operator=(const reference_wrapper &rhs) { value = rhs.value; update = rhs.update; return *this; }

        value_type operator+(const_reference rhs) const { return value + rhs; }
        value_type operator-(const_reference rhs) const { return value - rhs; }
        value_type operator*(const_reference rhs) const { return value * rhs; }
        value_type operator/(const_reference rhs) const { return value / rhs; }
        value_type operator%(const_reference rhs) const { return value % rhs; }

        reference_wrapper& operator+=(const_reference rhs) { value += rhs; update(); return *this; }
        reference_wrapper& operator-=(const_reference rhs) { value -= rhs; update(); return *this; }
        reference_wrapper& operator*=(const_reference rhs) { value *= rhs; update(); return *this; }
        reference_wrapper& operator/=(const_reference rhs) { value /= rhs; update(); return *this; }
        reference_wrapper& operator%=(const_reference rhs) { value %= rhs; update(); return *this; }

        reference_wrapper& operator++() { ++value; update(); return *this; }
        reference_wrapper& operator++(int) { value++; update(); return *this; }
        reference_wrapper& operator--() { --value; update(); return *this; }
        reference_wrapper& operator--(int) { value--; update(); return *this; }

        bool operator==(const_reference rhs) const { return value == rhs; }
        bool operator!=(const_reference rhs) const { return value != rhs; }
        bool operator>(const_reference rhs) const { return value > rhs; }
        bool operator>=(const_reference rhs) const { return value >= rhs; }
        bool operator<(const_reference rhs) const { return value < rhs; }
        bool operator<=(const_reference rhs) const { return value <= rhs; }

        reference operator*() const { return value; }
        operator reference() const { return value; }

        /*! Overload for operator<<() of std::ostream to print a reference_wrapper.
        */
        friend std::ostream &operator<<(std::ostream &os, const reference_wrapper<value_type> &ref) { return os << ref.value; }

    private:
        reference value;
        boost::function<void (void)> update;
    };

} } // namespace boost::self_healing


#endif // BOOST_SELF_HEALING_REFERENCE_WRAPPER_HPP
