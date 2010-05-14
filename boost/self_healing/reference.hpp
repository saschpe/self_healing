/*! \file
* \brief Reference.
*
* This files contains the class reference, which defines a reference to a value
* that calls a function object on value updates.
*
* (C) Copyright Sascha Peilicke 2010.
*
* Distributed under the Boost Software License, Version 1.0. (See
* accompanying file LICENSE_1_0.txt or copy at
* http://www.boost.org/LICENSE_1_0.txt)
*
* 5 May 2010 - Initial Revision (Sascha Peilicke)
*/

#ifndef BOOST_SELF_HEALING_REFERENCE_HPP
#define BOOST_SELF_HEALING_REFERENCE_HPP

#include <ostream>

// FIXES for broken compilers
#include <boost/config.hpp>

#include "./detail/nullary_function.hpp"


/// The namespace self_healing contains fault-tolerant data structures and utility classes.
namespace boost { namespace self_healing {

    /*! \brief Reference
    *
    * A reference to a value which optionally takes a function object that is
    * called every time the value changes.
    *
    * \param T The data type of the stored value.
    *
    * \remarks TODO.
    *
    * \see nullary_function, empty_nullary_function
    */
    template <class T>
    class reference
    {
    public:
        /*! Constructor.
        * \param value The value to initialize the reference with.
        * \param update The function to apply if the value is changed.
        */
        explicit reference(T &value, nullary_function &update = empty_nullary_function)
            : value(value), update(update) {}

        /*! Copy constructor.
        * \param other The other reference instance to copy from.
        */
        reference(const reference &other)
            : value(other.value), update(other.update) {}

        reference& operator=(const T &rhs) { value = rhs; update(); return *this; }
        reference& operator=(const reference &rhs) { value = rhs.value; update = rhs.update; return *this; }

        T operator+(const T &rhs) const { return value + rhs; }
        T operator-(const T &rhs) const { return value - rhs; }
        T operator*(const T &rhs) const { return value * rhs; }
        T operator/(const T &rhs) const { return value / rhs; }
        T operator%(const T &rhs) const { return value % rhs; }

        reference& operator+=(const T &rhs) { value += rhs; update(); return *this; }
        reference& operator-=(const T &rhs) { value -= rhs; update(); return *this; }
        reference& operator*=(const T &rhs) { value *= rhs; update(); return *this; }
        reference& operator/=(const T &rhs) { value /= rhs; update(); return *this; }
        reference& operator%=(const T &rhs) { value %= rhs; update(); return *this; }

        reference& operator++() { ++value; update(); return *this; }
        reference& operator++(int) { value++; update(); return *this; }
        reference& operator--() { --value; update(); return *this; }
        reference& operator--(int) { value--; update(); return *this; }

        bool operator==(const T &rhs) const { return value == rhs; }
        bool operator!=(const T &rhs) const { return value != rhs; }
        bool operator>(const T &rhs) const { return value > rhs; }
        bool operator>=(const T &rhs) const { return value >= rhs; }
        bool operator<(const T &rhs) const { return value < rhs; }
        bool operator<=(const T &rhs) const { return value <= rhs; }

        T& operator*() const { return value; }
        operator T&() const { return value; }

        /*! Overload for operator<<() of std::ostream to print an iterator
        */
        friend std::ostream &operator<<(std::ostream &os, const reference<T> &ref) { return os << ref.value; }

    private:
        T &value;
        nullary_function &update;
    };

} } // namespace boost::self_healing


#endif // BOOST_SELF_HEALING_REFERENCE_HPP
