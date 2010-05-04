
// Copyright Sascha Peilicke <sasch.pe@gmx.de> 2010.
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
//  http://www.boost.org/LICENSE_1_0.txt)

#ifndef ROBUST_ARRAY_H
#define ROBUST_ARRAY_H

#include <boost/detail/workaround.hpp>

#if BOOST_WORKAROUND(BOOST_MSVC, >= 1400)
# pragma warning(push)
# pragma warning(disable:4996) // 'std::equal': Function call with parameters that may be unsafe
# pragma warning(disable:4510) // boost::array<T,N>' : default constructor could not be generated
# pragma warning(disable:4610) // warning C4610: class 'boost::array<T,N>' can never be instantiated - user defined constructor required
#endif

#include <cstddef>
#include <stdexcept>
#include <boost/assert.hpp>
#include <boost/swap.hpp>
#include <boost/crc.hpp>

// Handles broken standard libraries better than <iterator>
#include <boost/detail/iterator.hpp>
#include <boost/throw_exception.hpp>
#include <algorithm>

// FIXES for broken compilers
#include <boost/config.hpp>

namespace robust {

template <class T, std::size_t N = 64>
class array
{
public:
    // type definitions
    typedef T              value_type;
    typedef T*             iterator;
    typedef const T*       const_iterator;
    typedef T&             reference;
    typedef const T&       const_reference;
    typedef std::size_t    size_type;
    typedef std::ptrdiff_t difference_type;

    array(const T &value = 0, void *head = NULL, void *tail = NULL)
        : m_head(head), m_tail(tail)
    {
        for (size_type i = 0; i < N; i++) {
            m_elements[i] = value;
        }

        // compute and store initial CRC checksums
        boost::crc_32_type crc;
        crc.process_bytes(&m_elements, N * sizeof(T));
        m_crc1 = crc.checksum();
        m_crc2 = crc.checksum();
    }

    // iterator support
    iterator begin() { return m_elements; }
    const_iterator begin() const { return m_elements; }
    iterator end() { return m_elements + N; }
    const_iterator end() const { return m_elements + N; }

    // reverse iterator support
#if !defined(BOOST_NO_TEMPLATE_PARTIAL_SPECIALIZATION) && !defined(BOOST_MSVC_STD_ITERATOR) && !defined(BOOST_NO_STD_ITERATOR_TRAITS)
    typedef std::reverse_iterator<iterator> reverse_iterator;
    typedef std::reverse_iterator<const_iterator> const_reverse_iterator;
#elif defined(_MSC_VER) && (_MSC_VER == 1300) && defined(BOOST_DINKUMWARE_STDLIB) && (BOOST_DINKUMWARE_STDLIB == 310)
    // workaround for broken reverse_iterator in VC7
    typedef std::reverse_iterator<std::_Ptrit<value_type, difference_type, iterator,
                                              reference, iterator, reference> > reverse_iterator;
    typedef std::reverse_iterator<std::_Ptrit<value_type, difference_type, const_iterator,
                                              const_reference, iterator, reference> > const_reverse_iterator;
#elif defined(_RWSTD_NO_CLASS_PARTIAL_SPEC)
    typedef std::reverse_iterator<iterator, std::random_access_iterator_tag,
                                  value_type, reference, iterator, difference_type> reverse_iterator;
    typedef std::reverse_iterator<const_iterator, std::random_access_iterator_tag,
                                  value_type, const_reference, const_iterator, difference_type> const_reverse_iterator;
#else
    // workaround for broken reverse_iterator implementations
    typedef std::reverse_iterator<iterator,T> reverse_iterator;
    typedef std::reverse_iterator<const_iterator,T> const_reverse_iterator;
#endif
    reverse_iterator rbegin() { return reverse_iterator(end()); }
    const_reverse_iterator rbegin() const { return const_reverse_iterator(end()); }
    reverse_iterator rend() { return reverse_iterator(begin()); }
    const_reverse_iterator rend() const { return const_reverse_iterator(begin()); }

    // operator[]
    reference operator[](size_type i) {
        //TODO: assert( i < N && "out of range" );
        return m_elements[i];
    }
    const_reference operator[](size_type i) const {
        //TODO: assert( i < N && "out of range" );
        return m_elements[i];
    }

    // at() with range check
    reference at(size_type i) { rangecheck(i); return m_elements[i]; }
    const_reference at(size_type i) const { rangecheck(i); return m_elements[i]; }

    // front() and back()
    reference front() { return m_elements[0]; }
    const_reference front() const { return m_elements[0]; }
    reference back() { return m_elements[N - 1]; }
    const_reference back() const { return m_elements[N - 1]; }

    // size is constant
    static size_type size() { return N; }
    static bool empty() { return false; }
    static size_type max_size() { return N; }
    enum { static_size = N };

    // swap (note: linear complexity)
    void swap (array<T, N>& y) {
        for (size_type i = 0; i < N; ++i) {
            boost::swap(m_elements[i], y.m_elements[i]);
        }
        boost::swap(m_crc1, y.m_crc1);
        boost::swap(m_crc2, y.m_crc2);
    }

    //NOTE: those methods would bypass checksumming
    /*// direct access to data (read-only)
    const T* data() const { return m_elements; }
    T* data() { return m_elements; }

    // use array as C array (direct read/write access to data)
    T* c_array() { return m_elements; }*/

    // assignment with type conversion
    template <typename T2>
    array<T, N>& operator= (const array<T2, N>& rhs) {
        std::copy(rhs.begin(), rhs.end(), begin());
        return *this;
    }

    // assign one value to all elements
    void assign (const T& value) { fill(value); }    // A synonym for fill
    void fill   (const T& value) { std::fill_n(begin(), size(),value); }

    // check range (may be private because it is static)
    static void rangecheck (size_type i) {
        if (i >= size()) {
            std::out_of_range e("array<>: index out of range");
            boost::throw_exception(e);
        }
    }

    bool is_valid() {
        bool valid = true;
        valid &= check_head_pointer();
        valid &= check_tail_pointer();
        valid &= check_checksums();
        return valid;
    }

private:
    bool check_head_pointer() {
        bool success = true;
        // check if the circular reference on head is ok
        if (m_head != NULL) {
            //**m_head == &m_head;
        }
    }

    bool check_tail_pointer() {
        bool success = true;
        // check if the circular reference on tail is ok
        if (m_tail != NULL) {
            //success &= **m_tail == &m_tail;
        }
        return success;
    }

    unsigned int check_checksums() {
        //TODO: check crcs
        return m_crc1;
    }

    void *m_head;
    unsigned int m_crc1;            // first CRC-32 checksum
    T m_elements[N];                // fixed-size array of elements of type T
    unsigned int m_crc2;            // fecond CRC-32 checksum
    void *m_tail;
};

} // namespace robust

template <class T, std::size_t N>
std::ostream &operator<<(std::ostream &os, const robust::array<T, N> &array)
{
    os << "[";
    for (std::size_t i = 0; i < array.size(); i++) {
        os << (i == 0 ? "" : ",") << array.at(i);
    }
    os << "]";
    return os;
}

#endif // ROBUST_ARRAY_H
