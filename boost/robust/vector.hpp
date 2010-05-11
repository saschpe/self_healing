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

#include <stdexcept>
#include <boost/swap.hpp>

// Handles broken standard libraries better than <iterator>
#include <boost/detail/iterator.hpp>
#include <boost/throw_exception.hpp>

// FIXES for broken compilers
#include <boost/config.hpp>

#include "./detail/nullary_function.hpp"
#include "reference.hpp"
#include "checksummed_array.hpp"


/// The namespace robust contains fault-tolerant data structures and utility classes.
namespace boost { namespace robust {

    /*! \brief Vector.
    *
    * TODO.
    *
    * \param T
    * \param A
    *
    * \remarks TODO:
    *
    * \see TODO.
    */
    template <class T, class A = std::allocator<T> >
    class vector
    {
    public:
        // type definitions
        typedef T                    value_type;        //!< The type of elements stored in the <code>vector</code>.
        //typedef T *                iterator;
        //typedef const T *          const_iterator;
        //typedef robust::pointer<T>   pointer;           //!< A pointer to the element.
        typedef const T *            const_pointer;     //!< A const pointer to the element.
        typedef robust::reference<T> reference;         //!< A reference to an element.
        typedef const T &            const_reference;   //!< A const reference to an element.

        /*! \brief The size type.
        *
        * An unsigned integral type that can represent any non-negative value of the container's distance type.
        */
        typedef std::size_t          size_type;

        /*! \brief The distance type.
        *
        * A signed integral type used to represent the distance between two iterators.
        */
        typedef std::ptrdiff_t       difference_type;
        typedef A                    allocator_type;    //!< The type of an allocator used in the <code>circular_buffer</code>.
        class const_iterator;

        /*! \brief A (random access) iterator used to iterate through the <code>vector</code>.
        *
        * A safe iterator that calls a functor if the value at the current
        * position is changed. Checksumms are also updated correctly if the
        * iterator is dereferenced.
        *
        * \see TODO.
        */
        class iterator : public std::iterator<std::random_access_iterator_tag, T>
        {
        public:
            /*! Constructor.
            * \param rhs TODO.
            * \param functor The function object to apply if the value is changed.
            */
            explicit iterator(T *rhs, nullary_function &functor = empty_nullary_function)
                : m_p(rhs), m_functor(functor) {}

            /*! Copy constructor.
            * \param other The other iterator instance to copy from.
            * \param functor The function object to apply if the value is changed.
            */
            iterator(const iterator &other, nullary_function &functor = empty_nullary_function)
                : m_p(other.m_p), m_functor(functor) {}

            //iterator& operator=(T *rhs) { *m_p = rhs; m_functor(); ++m_p; return *this; }
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
            T *m_p;                         //!< Internal pointer to the current position in the vector.
            nullary_function &m_functor;    //!< Internal reference to the functor to apply.
        };

        /*! A const (random access) iterator used to iterate through the <code>checksummed_array</code>.
        */
        class const_iterator : public std::const_iterator<std::random_access_iterator_tag, T>
        {
        public:
            /*! Constructor.
            * \param rhs TODO.
            */
            explicit iterator(chunk *rhs)
                : m_p(rhs) {}

            /*! Copy constructor.
            * \param other The other iterator instance to copy from.anged.
            */
            iterator(const iterator &other)
                : m_p(other.m_p) {}

        private:
            chunk *m_p;
        };

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


        explicit vector(const A & = A());
        explicit vector(size_type, const A & = A());
        vector(size_type, const T &, const A & = A());

        /*! Copy constructor.
        * \param other The other vector to copy from.
        */
        vector(const vector<T, A> &other);

        explicit vector(size_type);
        vector(const vector<T>);

        template <class InputIterator>
        vector(InputIterator, InputIterator, const A& = A());
        ~vector();

        vector<T, A>& operator=(const vector<T, A>&);


        template <class InputIterator>
        void assign(InputIterator first, InputIterator last);
        template <class Size, class TT>
        void assign(Size n);
        template <class Size, class TT>
        void assign(Size n, const TT&);
        allocator_type get_allocator() const;

        // iterator support
        //TODO: Fix those
        iterator begin() { return iterator(m_chunks, m_func); }
        const_iterator begin() const { return const_iterator(m_chunks); }
        iterator end() { return iterator(m_elements + N, m_func); }
        const_iterator end() const { return const_iterator(m_chunks + N); }

        // reverse iterator support
        reverse_iterator rbegin() { return reverse_iterator(end()); }
        const_reverse_iterator rbegin() const { return const_reverse_iterator(end()); }
        reverse_iterator rend() { return reverse_iterator(begin()); }
        const_reverse_iterator rend() const { return const_reverse_iterator(begin()); }

        // Capacity
        size_type size() const;
        size_type max_size() const {

        }

        void resize(size_type);
        void resize(size_type, T);
        size_type capacity() const {

        }
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
            return false;
        }

    private:
        /*! \brief Element storage chunk.
        *
        * TODO.
        *
        * \remarks The chunk size should be chosen based on CPU cache size.
        *
        * \see checksummed_array
        */
        template <class Size = 64>
        class chunk
        {
        public:
            chunk(vector<T, A> *parent)
                : m_parent(parent) {}

            bool is_valid(vector<T, A> *parent) {
                if (*m_parent != parent) {
                    m_parent = parent;
                }
                return m_elements.is_valid();
            }

            vector<T, A> *parent() const { return m_parent; }

        private:
            vector<T, A> *m_parent;                 //!< Pointer to the paernt vector instance.
            checksummed_array<T, Size> m_elements;  //!< The actual data store.
        };

        chunk *m_head;          //!< Internal pointer to the first chunk.
        chunk *m_tail;          //!< Internal pointer to the last chunk.

        size_type m_chunks;     //!< Chunk counter.
        size_type m_size;       //!< Counts how much elements are stored currently in all chunks.
        size_type m_capacity;   //!< Counts how much elements can be stored in all chunks.
    };

    // comparisons
    template<class T, class A>
    inline bool operator==(const vector<T> &x, const vector<T> &y) {
        return std::equal(x.begin(), x.end(), y.begin());
    }
    template<class T, class A>
    inline bool operator<(const vector<T> &x, const vector<T> &y) {
        return std::lexicographical_compare(x.begin(), x.end(), y.begin(), y.end());
    }
    template<class T, class A>
    inline bool operator!=(const vector<T> &x, const vector<T> &y) {
        return !(x == y);
    }
    template<class T, class A>
    inline bool operator>(const vector<T> &x, const vector<T> &y) {
        return y < x;
    }
    template<class T, class A>
    inline bool operator<=(const vector<T> &x, const vector<T> &y) {
        return !(y < x);
    }
    template<class T, class A>
    inline bool operator>=(const vector<T>& x, const vector<T> &y) {
        return !(x < y);
    }

    /*! Global swap().
    */
    template<class T, class A>
    inline void swap (vector<T, A> &x, vector<T, A> &y) {
        x.swap(y);
    }

} } // namespace boost::robust


/*! Overload for operator<<() of std::ostream to print a vector.
*/
template <class T, class A>
std::ostream &operator<<(std::ostream &os, const boost::robust::vector<T, A> &vector)
{
    os << "[";
    for (std::size_t i = 0; i < vector.size(); i++) {
        os << (i == 0 ? "" : ",") << vector[i];
    }
    return os << "]";
}


#endif // BOOST_ROBUST_VECTOR_HPP
