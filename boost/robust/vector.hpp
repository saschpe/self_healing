/*! \file
* \brief Vector.
*
* This file contains the class vector, an robust STL vector with checksumming.
* This allows to monitor the validity of the vector's content.
*
* (C) Copyright Sascha Peilicke 2010.
*
* Distributed under the Boost Software License, Version 1.0. (See
* accompanying file LICENSE_1_0.txt or copy at
* http://www.boost.org/LICENSE_1_0.txt)
*
* 23 April 2010 - Initial Revision (Sascha Peilicke)
*
* http://github.com/saschpe/robust
*/

#ifndef BOOST_ROBUST_VECTOR_HPP
#define BOOST_ROBUST_VECTOR_HPP

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

// Handles broken standard libraries better than <iterator>
#include <boost/detail/iterator.hpp>
#include <boost/throw_exception.hpp>
#include <algorithm>

// FIXES for broken compilers
#include <boost/config.hpp>

#include "array.hpp"


/// The namespace robust contains fault-tolerant data structures and utility classes.
namespace boost { namespace robust {

    /*! \brief Vector.
    *
    * TODO.
    *
    * \param T
    * \param Allocator
    *
    * \remarks TODO:
    *
    * \see TODO.
    */
    template <class T, class Allocator = allocator<T> >
    class vector
    {
    public:
        // type definitions
        typedef T                    value_type;
        //typedef T *                iterator;      // replaced by safe class robust::vector<T, Allocator>::iterator
        //typedef const T *          const_iterator;// replaced by safe class robust::vector<T, Allocator>::const_iterator
        //typedef T &                reference;     // replaced by safe class robust::reference<T>
        typedef robust::reference<T> reference;
        typedef const T &            const_reference;
        typedef std::size_t          size_type;
        typedef std::ptrdiff_t       difference_type;

        /*! \brief Iterator.
        *
        * TODO.
        *
        * \see TODO.
        */
        class iterator : public std::iterator<std::random_access_iterator_tag, T>
        {
        public:
            /*! Constructor.
            * \param rhs TODO.
            */
            explicit iterator(T *rhs)
                : m_p(rhs) {}

            /*! Copy constructor.
            * \param rhs The other iterator instance to copy from.
            */
            iterator(const iterator &rhs)
                : m_p(rhs.m_p) {}

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
            T *m_p;                 //!< Internal pointer to the current position in the array.
            functor &m_functor;     //!< Internal reference to the functor to apply.
        };

        explicit vector(const Allocator & = Allocator());
        explicit vector(size_type, const Allocator & = Allocator());
        vector(size_type, const T &, const Allocator & = Allocator());

        /*! Copy constructor.
        * \param other The other vector to copy from.
        */
        vector(const vector<T, Allocator> &other);

        explicit vector(size_type);
        vector(const vector<T>);

        template <class InputIterator>
        vector(InputIterator, InputIterator, const Allocator& = Allocator());
        ~vector();

        vector<T, Allocator>& operator=(const vector<T, Allocator>&);


        template <class InputIterator>
        void assign(InputIterator first, InputIterator last);
        template <class Size, class TT>
        void assign(Size n);
        template <class Size, class TT>
        void assign(Size n, const TT&);
        allocator_type get_allocator() const;

        // Iterators
        iterator begin();
        const_iterator begin() const;
        iterator end();
        const_iterator end() const;
        reverse_iterator rbegin();
        const_reverse_iterator rbegin() const;
        reverse_iterator rend();
        const_reverse_iterator rend() const;

        // Capacity
        size_type size() const;
        size_type max_size() const;
        void resize(size_type);
        void resize(size_type, T);
        size_type capacity() const;
        bool empty() const;
        void reserve(size_type);

        // Element Access
        reference operator[](size_type);
        const_reference operator[](size_type) const;
        reference at(size_type);
        const_reference at(size_type) const;
        reference front();
        const_reference front() const;
        reference back();
        const_reference back() const;

        // Modifiers
        void push_back(const T &);
        void pop_back();
        /*iterator insert(iterator);
        iterator insert(iterator, const T &);
        void insert(iterator, size_type, const T &);
        template <class InputIterator>
        void insert(iterator, InputIterator, InputIterator);
        iterator erase(iterator);
        iterator erase(iterator, iterator);*/
        void swap(vector<T> &);

        /*! \brief Validity check.
        *
        * TODO.
        *
        * \return true, if the internal structure and data is valid
        */
        bool is_valid() {
        }

    private:
        /*! \brief Element storage chunk.
        *
        * TODO.
        *
        * \remarks The chunk size should be chosen based on CPU cache size.
        *
        * \see Array: array
        */
        template <class Size = 64>
        class chunk
        {
        public:
            chunk(vector<T, Allocator> *parent)
                : m_parent_head(parent->m_head), m_parent_tail(m_tail), m_elements(0) {}

            bool is_valid() const {
                //TODO: implement
                return false;
            }

        private:
            robust::vector<T, Allocator> *m_parent_head;
            robust::array<T, Size> m_elements;
            robust::vector<T, Allocator> *m_parent_tail;
        };

        chunk *m_head;          // Pointer to the first chunk
        chunk *m_tail;          // Pointer to the last chunk

        size_type m_chunks;     // Chunk count
        size_type m_size;       // How much elements are stored currently
        size_type m_capacity;   // How much elements can be stored
    };

    // comparisons
    template<class T, class Allocator>
    inline bool operator==(const vector<T> &x, const vector<T> &y) {
        return std::equal(x.begin(), x.end(), y.begin());
    }
    template<class T, class Allocator>
    inline bool operator<(const vector<T> &x, const vector<T> &y) {
        return std::lexicographical_compare(x.begin(), x.end(), y.begin(), y.end());
    }
    template<class T, class Allocator>
    inline bool operator!=(const vector<T> &x, const vector<T> &y) {
        return !(x == y);
    }
    template<class T, class Allocator>
    inline bool operator>(const vector<T> &x, const vector<T> &y) {
        return y < x;
    }
    template<class T, class Allocator>
    inline bool operator<=(const vector<T> &x, const vector<T> &y) {
        return !(y < x);
    }
    template<class T, class Allocator>
    inline bool operator>=(const vector<T>& x, const vector<T> &y) {
        return !(x < y);
    }

    /*! Global swap().
    */
    template<class T, class Allocator>
    inline void swap (vector<T, Allocator> &x, vector<T, Allocator> &y) {
        x.swap(y);
    }

} } // namespace boost::robust


/*! Overload for operator<<() of std::ostream to print a vector.
*/
template <class T, class Allocator>
std::ostream &operator<<(std::ostream &os, const boost::robust::vector<T, Allocator> &vector)
{
    os << "[";
    for (std::size_t i = 0; i < vector.size(); i++) {
        os << (i == 0 ? "" : ",") << vector[i];
    }
    return os << "]";
}


#if BOOST_WORKAROUND(BOOST_MSVC, >= 1400)
# pragma warning(pop)
#endif

#endif // BOOST_ROBUST_VECTOR_HPP
