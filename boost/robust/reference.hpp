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
*
* http://github.com/saschpe/robust
*/

#ifndef BOOST_ROBUST_REFERENCE_HPP
#define BOOST_ROBUST_REFERENCE_HPP

#include <ostream>

// FIXES for broken compilers
#include <boost/config.hpp>

#include "./detail/nullary_function.hpp"


/// The namespace robust contains fault-tolerant data structures and utility classes.
namespace boost { namespace robust {

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
        * \param value TODO.
        * \param functor TODO.
        */
        reference(T &value, nullary_function &functor = empty_nullary_function)
            : m_value(value), m_functor(functor) {}

        reference& operator=(const T &rhs) { m_value = rhs; m_functor(); return *this; }
        reference& operator=(const reference &rhs) { m_value = rhs.m_value; m_functor(); return *this; }

        T operator+(const T &rhs) const { return m_value + rhs; }
        T operator-(const T &rhs) const { return m_value - rhs; }
        T operator*(const T &rhs) const { return m_value * rhs; }
        T operator/(const T &rhs) const { return m_value / rhs; }
        T operator%(const T &rhs) const { return m_value % rhs; }

        reference& operator+=(const T &rhs) { m_value += rhs; m_functor(); return *this; }
        reference& operator-=(const T &rhs) { m_value -= rhs; m_functor(); return *this; }
        reference& operator*=(const T &rhs) { m_value *= rhs; m_functor(); return *this; }
        reference& operator/=(const T &rhs) { m_value /= rhs; m_functor(); return *this; }
        reference& operator%=(const T &rhs) { m_value %= rhs; m_functor(); return *this; }

        reference& operator++() { ++m_value; m_functor(); return *this; }
        reference& operator++(int) { m_value++; m_functor(); return *this; }
        reference& operator--() { --m_value; m_functor(); return *this; }
        reference& operator--(int) { m_value--; m_functor(); return *this; }

        bool operator==(const T &rhs) const { return m_value == rhs; }
        bool operator!=(const T &rhs) const { return m_value != rhs; }
        bool operator>(const T &rhs) const { return m_value > rhs; }
        bool operator>=(const T &rhs) const { return m_value >= rhs; }
        bool operator<(const T &rhs) const { return m_value < rhs; }
        bool operator<=(const T &rhs) const { return m_value <= rhs; }

        T& operator*() const { return m_value; }
        operator T&() const { return m_value; }

        friend std::ostream &operator<<(std::ostream &os, const reference<T> &ref) {
            return os << ref.m_value;
        }

    private:
        T &m_value;
        nullary_function &m_functor;
    };

} } // namespace boost::robust


#endif // BOOST_ROBUST_REFERENCE_HPP
