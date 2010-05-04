/* The following code declares class vector,
 * an robust STL vector with checksumming. This allows to monitor the validity
 * of the vector's content.
 *
 * The original author site is at: http://saschpe.wordpress.com/
 *
 * (C) Copyright Sascha Peilicke 2010.
 *
 * Distributed under the Boost Software License, Version 1.0. (See
 * accompanying file LICENSE_1_0.txt or copy at
 * http://www.boost.org/LICENSE_1_0.txt)
 *
 * 21 April 2010 - Initial Revision (Sascha Peilicke)
 *
 * May 4th, 2010
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

#include "array.h"


namespace robust {

    /**
     *
     */
    template <class T, class Allocator = allocator<T> >
    class vector
    {
        template <class T>
        struct chunk {
            ::vector *parent_head;
            ::array<T, 64> elems;   //NOTE: the size should be chosen based on CPU cache
            ::vector *parent_tail;
        };

    public:

        /*explicit vector(const Allocator& = Allocator());
        explicit vector(size_type, const Allocator& = Allocator());
        vector(size_type, const T&, const Allocator& = Allocator());
        vector(const vector<T, Allocator>&);*/

        explicit vector(size_type);
        vector(const vector<T>);

        /*template <class InputIterator>
        vector(InputIterator, InputIterator, const Allocator& = Allocator());*/
        ~vector();

        //vector<T, Allocator>& operator=(const vector<T, Allocator>&);


        /*template <class InputIterator>
        void assign(InputIterator first, InputIterator last);
        template <class Size, class TT>
        void assign(Size n);
        template <class Size, class TT>
        void assign(Size n, const TT&);
        allocator_type get_allocator() const;*/

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

    private:
        chunk const *m_head;
        void *m_tail;
        size_type m_chunks;
        size_type m_size;
        size_type m_capacity;
    };

    // comparisons
    template<class T, class Allocator>
    inline bool operator== (const vector<T> &x, const vector<T> &y) {
        return std::equal(x.begin(), x.end(), y.begin());
    }
    template<class T, class Allocator>
    inline bool operator< (const vector<T> &x, const vector<T> &y) {
        return std::lexicographical_compare(x.begin(),x.end(),y.begin(),y.end());
    }
    template<class T, class Allocator>
    inline bool operator!= (const vector<T> &x, const vector<T> &y) {
        return !(x==y);
    }
    template<class T, class Allocator>
    inline bool operator> (const vector<T> &x, const vector<T> &y) {
        return y<x;
    }
    template<class T, class Allocator>
    inline bool operator<= (const vector<T> &x, const vector<T> &y) {
        return !(y<x);
    }
    template<class T, class Allocator>
    inline bool operator>= (const vector<T>& x, const vector<T> &y) {
        return !(x<y);
    }

    // global swap()
    template<class T, class Allocator>
    inline void swap (vector<T, Allocator> &x, vector<T, Allocator> &y) {
        x.swap(y);
    }

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


#if BOOST_WORKAROUND(BOOST_MSVC, >= 1400)
# pragma warning(pop)
#endif

#endif /* BOOST_ROBUST_VECTOR_HPP */
