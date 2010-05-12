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

#include "checksummed_array.hpp"
#include "reference.hpp"


/// The namespace robust contains fault-tolerant data structures and utility classes.
namespace boost { namespace robust {

    /*! \brief Vector.
    *
    * TODO.
    *
    * \param A
    * \param CS The size of the internal chunks.
    * \param T The data type of the stored values.
    *
    * \remarks The chunk size should be chosen based on CPU cache size.
    *
    * \see checksummed_array
    */
    template <class T, std::size_t CS = 64, class A = std::allocator<T> >
    class vector
    {
        struct chunk;                                   //!< Private forward declaration of struct chunk.

    public:
        // type definitions
        typedef T                    value_type;        //!< The type of elements stored in the <code>vector</code>.
        class                        iterator;          //!< Forward declaration of class iterator.
        class                        const_iterator;    //!< Forward declaration of class const_iterator.
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

        /*! \brief A (random access) iterator used to iterate through the <code>vector</code>.
        *
        * A safe iterator that calls a functor if the value at the current
        * position is changed. Checksumms are also updated correctly if the
        * iterator is dereferenced.
        */
        class iterator : public std::iterator<std::random_access_iterator_tag, T>
        {
            friend class vector<T, CS, A>;

            /*! Private constructor.
            * \param rhs TODO.
            */
            explicit iterator(T *rhs)
                : m_p(rhs) {}

        public:
            /*! Copy constructor.
            * \param other The other iterator instance to copy from.
            */
            iterator(const iterator &other)
                : m_p(other.m_p) {}

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

            reference operator*() const { return reference(*m_p); }
            operator const_iterator() const { return m_p; }

        private:
            T *m_p; //!< Internal pointer to the current position in the vector.
        };

        /*! A const (random access) iterator used to iterate through the <code>vector</code>.
        */
        class const_iterator : public std::iterator<std::random_access_iterator_tag, T>
        {
            friend class vector<T, CS, A>;

            /*! Private constructor.
            * \param rhs The chunk to initialize the iterator with.
            */
            explicit const_iterator(chunk *rhs, const typename checksummed_array<T, CS>::iterator &it)
                : m_chunk(rhs), m_it(it), m_begin(rhs->elements.begin()), m_end(rhs->elements.end()) {}

        public:
            /*! Copy constructor.
            * \param other The other iterator instance to copy from.anged.
            */
            const_iterator(const const_iterator &other)
                : m_chunk(other.m_chunk), m_it(other.m_it), m_begin(other.m_begin), m_end(other.m_end) {}

            const_iterator& operator=(const const_iterator &rhs) { m_chunk = rhs.m_chunk; m_it = rhs.m_it; return *this; }

            const_iterator operator+(difference_type n) const {
                typename checksummed_array<T, CS>::const_iterator tmp_new = m_it + n;

                if (tmp_new >= m_begin && tmp_new < m_end) {
                    return const_iterator(m_chunk, tmp_new);
                } else if (tmp_new < m_begin) {
                    chunk *previous_chunk;
                    /*do {
                        previous_chunk = m_chunk->previous();
                        m_begin = previous_chunk->elements.begin();
                        m_end = previous_chunk->elements.end();
                        //tmp_new += CS;
                    } while (tmp_new < m_begin);*/
                    //return const_iterator(previous_chunk, tmp_new);
                } else {
                    chunk *next_chunk;
                    /*do {
                        next_chunk = m_chunk->next();
                        m_begin = next_chunk->elements.begin();
                        m_end = next_chunk->elements.end();
                        tmp_new -= CS;
                    } while (tmp_new >= m_end);*/
                    //return const_iterator(next_chunk, tmp_new);
                }
            }

            //const_iterator operator-(difference_type n) const { return from_current(m_index - n); }
            /*difference_type operator+(const const_iterator &rhs) const { return 0; }
            difference_type operator-(const const_iterator &rhs) const { return 0; }*/

            /*const_iterator& operator+=(difference_type n) { *this = from_current(m_index + n); return *this; }
            const_iterator& operator-=(difference_type n) { *this = from_current(m_index - n); return *this; }
            const_iterator& operator++() { *this = from_current(m_index + 1); return *this; }
            const_iterator& operator++(int) { *this = from_current(m_index + 1); return *this; }
            const_iterator& operator--() { *this = from_current(m_index - 1); return *this; }
            const_iterator& operator--(int) { *this = from_current(m_index - 1); return *this; }*/

            bool operator==(const const_iterator& rhs) const { return m_chunk == rhs.m_chunk && m_it == rhs.m_it; }
            bool operator!=(const const_iterator& rhs) const { return m_chunk != rhs.m_chunk || m_it != rhs.m_it; }

            const_reference operator*() const { return *m_it; }

            /*! Overload for operator<<() of std::ostream to print a reference.
            */
            friend std::ostream &operator<<(std::ostream &os, const const_iterator &it) {
                return os << it.m_it;
            }

        private:
            struct chunk *m_chunk;                                      //!< Internal pointer to the current chunk.
            typename checksummed_array<T, CS>::const_iterator m_it;     //!< Iterator of the current chunk
            typename checksummed_array<T, CS>::const_iterator m_begin;  //!<
            typename checksummed_array<T, CS>::const_iterator m_end;
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

        explicit vector(const A & = A())
            : m_head(new chunk(this)), m_tail(m_head), m_chunks(1), m_size(0), m_capacity(CS) {}

        /*explicit vector(size_type, const A & = A());
        vector(size_type, const T &, const A & = A());*/

        /*! Copy constructor.
        * \param other The other vector to copy from.
        */
        //vector(const vector<T, CS, A> &other);
        //vector(const vector<T>);

        /*explicit vector(size_type);

        template <class InputIterator>
        vector(InputIterator, InputIterator, const A & = A());*/

        ~vector() {
            delete[] m_head;
        }

        /*vector<T, CS, A>& operator=(const vector<T, CS, A> &);

        template <class InputIterator>
        void assign(InputIterator first, InputIterator last);
        template <class Size, class TT>
        void assign(Size n);
        template <class Size, class TT>
        void assign(Size n, const TT&);
        allocator_type get_allocator() const;*/

        // iterator support
        //iterator begin() { return iterator(m_chunks); }
        const_iterator begin() const { return const_iterator(m_head, m_head->elements.begin()); }
        //iterator end() { return iterator(m_chunks + N); }
        const_iterator end() const { return const_iterator(m_tail, m_tail->elements.end()); }

        // reverse iterator support
        //reverse_iterator rbegin() { return reverse_iterator(end()); }
        const_reverse_iterator rbegin() const { return const_reverse_iterator(end()); }
        //reverse_iterator rend() { return reverse_iterator(begin()); }
        const_reverse_iterator rend() const { return const_reverse_iterator(begin()); }

        // Capacity
        size_type size() const { return m_size; }
        //size_type max_size() const {}
        /*void resize(size_type);
        void resize(size_type, T);*/
        size_type capacity() const { return m_capacity; }
        bool empty() const { return m_size == 0; }
        //void reserve(size_type);

        // Element Access
        /*reference operator[](size_type);
        const_reference operator[](size_type) const;
        reference at(size_type);
        const_reference at(size_type) const;
        reference front();
        const_reference front() const;
        reference back();
        const_reference back() const;*/

        // Modifiers
        /*void push_back(const T &);
        void pop_back();*/
        /*iterator insert(iterator);
        iterator insert(iterator, const T &);
        void insert(iterator, size_type, const T &);
        template <class InputIterator>
        void insert(iterator, InputIterator, InputIterator);
        iterator erase(iterator);
        iterator erase(iterator, iterator);*/
        //void swap(vector<T> &);

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
        * \see vector, checksummed_array
        */
        struct chunk
        {
            explicit chunk(vector<T, CS, A> *parent)
                : parent(parent) {}

            bool is_valid(vector<T, CS, A> *parent) {
                if (this->parent != parent) {
                    this->parent = parent;
                }
                return elements.is_valid();
            }

            chunk *next() const {
                /*if (this >= parent->m_tail) {
                    std::out_of_range e("vector<>: index out of range");
                    boost::throw_exception(e);
                }*/
                return const_cast<chunk *>(this + sizeof(chunk));
            }
            chunk *previous() const {
                /*if (this <= parent->m_head) {
                    std::out_of_range e("vector<>: index out of range");
                    boost::throw_exception(e);
                }*/
                return const_cast<chunk *>(this - sizeof(chunk));
            }

            bool is_head() const { return this == parent->m_head; }
            bool is_tail() const { return this == parent->m_tail; }

            vector<T, CS, A> *parent;           //!< Pointer to the paernt vector instance.
            checksummed_array<T, CS> elements;  //!< The actual data store.
        };

        chunk *m_head;          //!< Internal pointer to the first chunk.
        chunk *m_tail;          //!< Internal pointer to the last chunk.

        size_type m_chunks;     //!< Chunk counter.
        size_type m_size;       //!< Counts how much elements are stored currently in all chunks.
        size_type m_capacity;   //!< Counts how much elements can be stored in all chunks.
    };


    //TODO: Add vector<T, 0, A>

    // comparisons
    template<class T, std::size_t CS, class A>
    inline bool operator==(const vector<T, CS, A> &x, const vector<T, CS, A> &y) {
        return std::equal(x.begin(), x.end(), y.begin());
    }
    template<class T, std::size_t CS, class A>
    inline bool operator<(const vector<T, CS, A> &x, const vector<T, CS, A> &y) {
        return std::lexicographical_compare(x.begin(), x.end(), y.begin(), y.end());
    }
    template<class T, std::size_t CS, class A>
    inline bool operator!=(const vector<T, CS, A> &x, const vector<T, CS, A> &y) {
        return !(x == y);
    }
    template<class T, std::size_t CS, class A>
    inline bool operator>(const vector<T, CS, A> &x, const vector<T, CS, A> &y) {
        return y < x;
    }
    template<class T, std::size_t CS, class A>
    inline bool operator<=(const vector<T, CS, A> &x, const vector<T, CS, A> &y) {
        return !(y < x);
    }
    template<class T, std::size_t CS, class A>
    inline bool operator>=(const vector<T, CS, A>& x, const vector<T, CS, A> &y) {
        return !(x < y);
    }

    /*! Global swap().
    */
    template<class T, std::size_t CS, class A>
    inline void swap (vector<T, CS, A> &x, vector<T, CS, A> &y) {
        x.swap(y);
    }

} } // namespace boost::robust


/*! Overload for operator<<() of std::ostream to print a vector.
*/
template <class T, std::size_t CS, class A>
std::ostream &operator<<(std::ostream &os, const boost::robust::vector<T, CS, A> &vector)
{
    os << "[";
    for (std::size_t i = 0; i < vector.size(); i++) {
        os << (i == 0 ? "" : ",") << vector[i];
    }
    return os << "]";
}


#endif // BOOST_ROBUST_VECTOR_HPP
