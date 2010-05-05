/* The following code declares class reference,
 * a reference to a value that calls a functor on value updates.
 *
 * The original author site is at: http://saschpe.wordpress.com/
 *
 * (C) Copyright Sascha Peilicke 2010.
 *
 * Distributed under the Boost Software License, Version 1.0. (See
 * accompanying file LICENSE_1_0.txt or copy at
 * http://www.boost.org/LICENSE_1_0.txt)
 *
 * 5 May 2010 - Initial Revision (Sascha Peilicke)
 *
 * May 5th, 2010
 */
#ifndef BOOST_ROBUST_REFERENCE_HPP
#define BOOST_ROBUST_REFERENCE_HPP

#include <ostream>

// FIXES for broken compilers
#include <boost/config.hpp>

#include <robust/functor.hpp>


namespace robust {

    /**
     * Reference to a value which optionally takes a functor that is called
     * every time the value changes.
     */
    template <class T>
    class reference
    {
    public:
        reference(T& value, robust::functor &functor = void_functor)
            : m_value(value), m_functor(functor) {}

        reference& operator=(const T& rhs) { m_value = rhs; m_functor(); return *this; }
        reference& operator=(const reference &rhs) { m_value = rhs.m_value; m_functor(); return *this; }

        T operator+(const T& rhs) const { return m_value + rhs; }
        T operator-(const T& rhs) const { return m_value - rhs; }
        T operator*(const T& rhs) const { return m_value * rhs; }
        T operator/(const T& rhs) const { return m_value / rhs; }
        T operator%(const T& rhs) const { return m_value % rhs; }

        reference& operator+=(const T& rhs) { m_value += rhs; m_functor(); return *this; }
        reference& operator-=(const T& rhs) { m_value -= rhs; m_functor(); return *this; }
        reference& operator*=(const T& rhs) { m_value *= rhs; m_functor(); return *this; }
        reference& operator/=(const T& rhs) { m_value /= rhs; m_functor(); return *this; }
        reference& operator%=(const T& rhs) { m_value %= rhs; m_functor(); return *this; }

        reference& operator++() { ++m_value; m_functor(); return *this; }
        reference& operator++(int) { m_value++; m_functor(); return *this; }
        reference& operator--() { --m_value; m_functor(); return *this; }
        reference& operator--(int) { m_value--; m_functor(); return *this; }

        bool operator==(const T& rhs) const { return m_value == rhs; }
        bool operator!=(const T& rhs) const { return m_value != rhs; }
        bool operator>(const T& rhs) const { return m_value > rhs; }
        bool operator>=(const T& rhs) const { return m_value >= rhs; }
        bool operator<(const T& rhs) const { return m_value < rhs; }
        bool operator<=(const T& rhs) const { return m_value <= rhs; }

        T& operator*() const { return m_value; }
        operator const T&() const { return m_value; }

        friend std::ostream &operator<<(std::ostream &os, const robust::reference<T> &ref) {
            return os << ref.m_value;
        }

    private:
        T& m_value;
        robust::functor &m_functor;
    };

} // namespace robust


#endif /* BOOST_ROBUST_REFERENCE_HPP */
