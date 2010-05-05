/*! \file
* \brief Array.
*
* This file contains the class array, an robust STL container (as wrapper) for
* arrays of constant size with checksumming. This allows to monitor the validity
* of the array's content.
*
* This class is based on boost::array by Nicolai M. Josuttis.
* See
*      http://www.boost.org/libs/array/
* for documentation.
*
* (C) Copyright Sascha Peilicke 2010.
*
* Distributed under the Boost Software License, Version 1.0. (See
* accompanying file LICENSE_1_0.txt or copy at
* http://www.boost.org/LICENSE_1_0.txt)
*
* 20 April 2010 - Initial Revision (Sascha Peilicke)
*
* http://github.com/saschpe/robust
*/

#ifndef BOOST_ROBUST_ARRAY_HPP
#define BOOST_ROBUST_ARRAY_HPP

#include <boost/detail/workaround.hpp>

#if BOOST_WORKAROUND(BOOST_MSVC, >= 1400)
# pragma warning(push)
# pragma warning(disable:4996) // 'std::equal': Function call with parameters that may be unsafe
# pragma warning(disable:4510) // boost::array<T,N>' : default constructor could not be generated
# pragma warning(disable:4610) // warning C4610: class 'boost::array<T,N>' can never be instantiated - user defined constructor required
#endif

#include <cstddef>
#include <iostream>
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

#include "reference.hpp"


/// The namespace robust contains fault-tolerant data structures and utility classes.
namespace boost { namespace robust {

    /*! \brief Array.
    *
    * A checksummed array of constant size.
    *
    * \param T The data type of the stored values.
    * \param N The size of the array.
    *
    * \remarks TODO.
    *
    * \see Functor: functor, void_functor
    */
    template <class T, std::size_t N>
    class array
    {
    public:
        // type definitions
        typedef T                    value_type;
        //typedef T*                 iterator;      // replaced by safe class robust::array<T, N>::iterator
        typedef const T*             const_iterator;
        //typedef T&                 reference;     // replaced by safe class robust::reference<T>
        typedef robust::reference<T> reference;
        typedef const T&             const_reference;
        typedef std::size_t          size_type;
        typedef std::ptrdiff_t       difference_type;

        /*! \brief Iterator.
        *
        * A safe iterator that calls a functor if the value at the current
        * position is changed. Checksumms are also updated correctly if the
        * iterator is dereferenced.
        *
        * \remarks TODO.
        *
        * \see std::iterator<>
        */
        class iterator : public std::iterator<std::random_access_iterator_tag, T>
        {
        public:
            explicit iterator(T *rhs, robust::functor &functor = void_functor)
                : m_p(rhs), m_functor(functor) {}
            iterator(const iterator &rhs, robust::functor &functor = void_functor)
                : m_p(rhs.m_p), m_functor(functor) {}

            //iterator& operator=(T* rhs) { *m_p = rhs; m_functor(); ++m_p; return *this; }
            iterator& operator=(const reference &rhs) { *m_p = rhs; m_functor(); ++m_p; return *this; }
            iterator& operator=(const iterator &rhs) { m_p = rhs.m_p; return *this; }

            iterator& operator+(difference_type n) const { return m_p + n; }
            iterator& operator-(difference_type n) const { return m_p - n; }
            difference_type operator+(const iterator &rhs) const { return m_p + rhs.m_p; }
            difference_type operator-(const iterator &rhs) const { return m_p - rhs.m_p; }

            iterator& operator+=(difference_type n) { m_p += n; return *this; }
            iterator& operator-=(difference_type n) { m_p -= n; return *this; }
            iterator& operator++() { ++m_p; return *this; }
            iterator& operator++(int) { m_p++; return *this; }
            iterator& operator--() { --m_p; return *this; }
            iterator& operator--(int) { m_p--; return *this; }

            bool operator==(const iterator& rhs) const { return m_p == rhs.m_p; }
            bool operator!=(const iterator& rhs) const { return m_p != rhs.m_p; }

            reference operator*() const { return reference(*m_p, m_functor); }
            operator const_iterator() const { return m_p; }

        private:
            T *m_p;
            robust::functor &m_functor;
        };

        /*! Contructor.
        * \param value An initial value that is set for all elements.
        */
        array(const T &value = 0)
            : m_func(this) { fill(value); }

        // iterator support
        iterator begin() { return iterator(m_elements, m_func); }
        const_iterator begin() const { return m_elements; }
        iterator end() { return iterator(m_elements + N, m_func); }
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
        typedef std::reverse_iterator<iterator, T> reverse_iterator;
        typedef std::reverse_iterator<const_iterator, T> const_reverse_iterator;
#endif
        reverse_iterator rbegin() { return reverse_iterator(end()); }
        const_reverse_iterator rbegin() const { return const_reverse_iterator(end()); }
        reverse_iterator rend() { return reverse_iterator(begin()); }
        const_reverse_iterator rend() const { return const_reverse_iterator(begin()); }

        // operator[] with range check
        reference operator[](size_type i) { rangecheck(i); return reference(m_elements[i], m_func); }
        const_reference operator[](size_type i) const { rangecheck(i); return m_elements[i]; }

        // at() with range check
        reference at(size_type i) { rangecheck(i); return reference(m_elements[i], m_func); }
        const_reference at(size_type i) const { rangecheck(i); return m_elements[i]; }

        // front() and back()
        reference front() { return reference(m_elements[0], m_func); }
        const_reference front() const { return m_elements[0]; }
        reference back() { return reference(m_elements[N - 1], m_func); }
        const_reference back() const { return m_elements[N - 1]; }

        // size is constant
        static size_type size() { return N; }
        static bool empty() { return false; }
        static size_type max_size() { return N; }
        enum { static_size = N };

        // swap (note: linear complexity)
        void swap(array<T, N> &y) {
            for (size_type i = 0; i < N; ++i) {
                boost::swap(m_elements[i], y.m_elements[i]);
            }
            boost::swap(m_crc1, y.m_crc1);
            boost::swap(m_crc2, y.m_crc2);
        }

        // direct access to data (read-only)
        const T* data() const { return m_elements; }
        //NOTE: those methods would bypass checksumming
        /*
        T* data() { return m_elements; }

        // use array as C array (direct read/write access to data)
        T* c_array() { return m_elements; }
        */

        // assignment with type conversion
        template <typename T2>
        array<T, N>& operator=(const array<T2, N> &rhs) {
            std::copy(rhs.begin(), rhs.end(), begin());
            update_checksums();
            return *this;
        }

        // assign one value to all elements
        void assign(const T &value) { fill(value); }    // A synonym for fill
        void fill(const T &value) {
            std::fill_n(begin(), size(),value);
            update_checksums();
        }

        // check range (may be private because it is static)
        static void rangecheck(size_type i) {
            if (i >= size()) {
                std::out_of_range e("array<>: index out of range");
                boost::throw_exception(e);
            }
        }

        /*! Validity check that tries to correct minor checksum faults silently.
         *
         * If one out of three checksums is wrong, this can be corrected.
         */
        bool is_valid() {
            boost::crc_32_type crc3;
            crc3.process_bytes(&m_elements, N * sizeof(T));
            const bool equal_13 = m_crc1 == crc3.checksum();
            const bool equal_23 = m_crc2 == crc3.checksum();
            const bool equal_12 = m_crc1 == m_crc2;

            if (equal_12 && equal_13 && equal_23) {
                return true;        // all fine
            }
            if (equal_13) {
                m_crc2 = m_crc1;    // fix m_crc2 as the others are equal
                return true;        // all fine
            }
            if (equal_23) {
                m_crc1 = m_crc2;    // fix m_crc1 as the others are equal
                return true;        // all fine
            }
            if (equal_12) {
                // The computed checksum over the content is not the same as
                // the stored onces, thus the content was maliciously changed
                // and the array is invalid.
                return false;
            }
            return false;           // checksum mismatch, fail
        }

    private:
        void update_checksums() {
            // compute and store CRC checksums
            std::cout << "robust::array<T, N>::update_checksums()" << std::endl;
            boost::crc_32_type crc;
            crc.process_bytes(&m_elements, N * sizeof(T));
            m_crc1 = crc.checksum();
            m_crc2 = m_crc1;
        }

        /*! \brief Update checksums functor.
         *
         * A private functor implementation that calls update_checksums() for a
         * given array instance if called itself.
         *
         * \see Functor: functor
         */
        class update_checksums_functor : public robust::functor
        {
        public:
            /*! Constructor.
            * \param parent The array instance that owns the functor instance.
            */
            update_checksums_functor(array<T, N> *parent)
                : m_parent(parent) {}

            void operator()() { m_parent->update_checksums(); }

        private:
            array<T, N> *m_parent;
        } m_func;                           /// functor instance to pass to robust::reference instances

        unsigned int m_crc1;                /// first checksum
        T m_elements[N];                    /// fixed-size array of elements of type T
        unsigned int m_crc2;                /// second checksum (backup)
    };

#if !defined(BOOST_NO_TEMPLATE_PARTIAL_SPECIALIZATION)
    /*! \brief Partial template specialization for the corner case of a zero sized array.
     */
    template<class T>
    class array<T, 0>
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

        // iterator support
        iterator begin() { return iterator(reinterpret_cast<T *>(this)); }
        const_iterator begin() const { return const_iterator(reinterpret_cast<const T *>(this)); }
        iterator end() { return begin(); }
        const_iterator end() const { return begin(); }

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
        typedef std::reverse_iterator<iterator, T> reverse_iterator;
        typedef std::reverse_iterator<const_iterator, T> const_reverse_iterator;
#endif

        reverse_iterator rbegin() { return reverse_iterator(end()); }
        const_reverse_iterator rbegin() const { return const_reverse_iterator(end()); }
        reverse_iterator rend() { return reverse_iterator(begin()); }
        const_reverse_iterator rend() const { return const_reverse_iterator(begin()); }

        // operator[]
        reference operator[](size_type /*i*/) { return failed_rangecheck(); }
        const_reference operator[](size_type /*i*/) const { return failed_rangecheck(); }

        // at() with range check
        reference at(size_type /*i*/) { return failed_rangecheck(); }
        const_reference at(size_type /*i*/) const { return failed_rangecheck(); }

        // front() and back()
        reference front() { return failed_rangecheck(); }
        const_reference front() const { return failed_rangecheck(); }

        reference back() { return failed_rangecheck(); }
        const_reference back() const { return failed_rangecheck(); }

        // size is constant
        static size_type size() { return 0; }
        static bool empty() { return true; }
        static size_type max_size() { return 0; }
        enum { static_size = 0 };

        void swap(array<T, 0> &/*y*/) {}

        // direct access to data (read-only)
        const T* data() const { return 0; }
        T* data() { return 0; }

        // use array as C array (direct read/write access to data)
        T* c_array() { return 0; }

        // assignment with type conversion
        template <typename T2>
        array<T, 0>& operator=(const array<T2, 0> &) { return *this; }

        // assign one value to all elements
        void assign(const T &value) { fill(value); }
        void fill(const T &/*value*/) {}

        // check range (may be private because it is static)
        static reference failed_rangecheck() {
            std::out_of_range e("attempt to access element of an empty array");
            boost::throw_exception(e);
#if defined(BOOST_NO_EXCEPTIONS) || !defined(BOOST_MSVC)
            //
            // We need to return something here to keep
            // some compilers happy: however we will never
            // actually get here....
            //
            static T placeholder;
            return placeholder;
#endif
        }
    };
#endif

    // comparisons
    template<class T, std::size_t N>
    bool operator==(const array<T, N> &x, const array<T, N> &y) {
        return std::equal(x.begin(), x.end(), y.begin());
    }
    template<class T, std::size_t N>
    bool operator<(const array<T, N> &x, const array<T, N> &y) {
        return std::lexicographical_compare(x.begin(), x.end(), y.begin(), y.end());
    }
    template<class T, std::size_t N>
    bool operator!=(const array<T, N> &x, const array<T, N> &y) {
        return !(x == y);
    }
    template<class T, std::size_t N>
    bool operator>(const array<T, N> &x, const array<T, N> &y) {
        return y < x;
    }
    template<class T, std::size_t N>
    bool operator<=(const array<T, N> &x, const array<T, N> &y) {
        return !(y < x);
    }
    template<class T, std::size_t N>
    bool operator>=(const array<T, N> &x, const array<T, N> &y) {
        return !(x < y);
    }

    /*! \brief Global swap().
    */
    template<class T, std::size_t N>
    inline void swap(array<T, N> &x, array<T, N> &y) {
        x.swap(y);
    }

} } // namespace boost::robust


template <class T, std::size_t N>
std::ostream &operator<<(std::ostream &os, const boost::robust::array<T, N> &array)
{
    os << "[";
    for (std::size_t i = 0; i < array.size(); i++) {
        os << (i == 0 ? "" : ",") << array[i];
    }
    return os << "]";
}


#if BOOST_WORKAROUND(BOOST_MSVC, >= 1400)
# pragma warning(pop)
#endif

#endif // BOOST_ROBUST_ARRAY_HPP
