/*! \file
* \brief Vector.
*
* This file contains the class vector, an self-healing STL vector with
* checksumming. This allows to monitor the validity of the vector's content.
*
* (C) Copyright Sascha Peilicke 2010.
*
* Distributed under the Boost Software License, Version 1.0. (See
* accompanying file LICENSE_1_0.txt or copy at
* http://www.boost.org/LICENSE_1_0.txt)
*
* 23 April 2010 - Initial Revision (Sascha Peilicke)
*/

#ifndef BOOST_SELF_HEALING_VECTOR_HPP
#define BOOST_SELF_HEALING_VECTOR_HPP

#include <boost/detail/workaround.hpp>

#ifdef BOOST_SELF_HEALING_DEBUG
#include <iostream>
#endif
#include <stdexcept>
#include <vector>
#include <boost/swap.hpp>

// Handles broken standard libraries better than <iterator>
#include <boost/detail/iterator.hpp>
#include <boost/throw_exception.hpp>

// FIXES for broken compilers
#include <boost/config.hpp>

#include "./detail/reference_wrapper.hpp"
#include "./detail/vector_chunk.hpp"
#include "./detail/utility.hpp"


/// The namespace self_healing contains fault-tolerant data structures and utility classes.
namespace boost { namespace self_healing {

    /*! \brief Self-healing vector.
    *
    * TODO.
    *
    * \param T The data type of the stored values.
    * \param N The capacity of the internal chunks.
    * \remarks The chunk size should be chosen based on CPU cache size.
    * \see std:vector, vector_chunk
    */
    template <class T, std::size_t N = 64>
    class vector
    {
    public:
        // type definitions
        typedef T                       value_type;        //!< The type of elements stored in the <code>vector</code>.
        class                           iterator;          //!< Forward declaration of class iterator.
        class                           const_iterator;    //!< Forward declaration of class const_iterator.
        typedef T *                     pointer;           //!< A pointer to an element.
        typedef const T *               const_pointer;     //!< A const pointer to an element.
        typedef reference_wrapper<T>    reference;         //!< A reference to an element.
        typedef const T &               const_reference;   //!< A const reference to an element.
        typedef std::size_t             size_type;         //!< An unsigned integral type that can represent any non-negative value of the container's distance type.
        typedef std::ptrdiff_t          difference_type;   //!< A signed integral type used to represent the distance between two iterators.

    private:
        // private type definitions
        typedef vector_chunk<T, N>      vector_chunk_type;                              //!< A vector chunk.
        typedef vector_chunk<T, N> *    vector_chunk_pointer;                           //!< A pointer to vector chunk.
        static const size_type          vector_chunk_size = sizeof(vector_chunk_type);  //!< The size of a vector chunk.
    public:

#if 0
        /*! \brief A (random access) iterator used to iterate through the <code>vector</code>.
        *
        * A safe iterator that calls a functor if the value at the current
        * position is changed. Checksumms are also updated correctly if the
        * iterator is dereferenced.
        */
        class iterator : public std::iterator<std::random_access_iterator_tag, value_type>
        {
            friend class vector<value_type, N>;

            /*! Private constructor.
            * \param rhs TODO.
            */
            explicit iterator(pointer rhs)
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
        class const_iterator : public std::iterator<std::random_access_iterator_tag, value_type>
        {
            friend class vector<T, N>;

            /*! Private constructor.
            * \param rhs The chunk to initialize the iterator with.
            */
            explicit const_iterator(chunk *rhs, const typename checksummed_array<T, N>::iterator &it)
                : m_chunk(rhs), m_it(it), m_begin(rhs->elements.begin()), m_end(rhs->elements.end()) {}

        public:
            /*! Copy constructor.
            * \param other The other iterator instance to copy from.anged.
            */
            const_iterator(const const_iterator &other)
                : m_chunk(other.m_chunk), m_it(other.m_it), m_begin(other.m_begin), m_end(other.m_end) {}

            const_iterator& operator=(const const_iterator &rhs) { m_chunk = rhs.m_chunk; m_it = rhs.m_it; return *this; }

            const_iterator operator+(difference_type n) const {
                typename checksummed_array<T, N>::const_iterator tmp_new = m_it + n;

                if (tmp_new >= m_begin && tmp_new < m_end) {
                    return const_iterator(m_chunk, tmp_new);
                } else if (tmp_new < m_begin) {
                    chunk *previous_chunk;
                    /*do {
                        previous_chunk = m_chunk->previous();
                        m_begin = previous_chunk->elements.begin();
                        m_end = previous_chunk->elements.end();
                        //tmp_new += N;
                    } while (tmp_new < m_begin);*/
                    //return const_iterator(previous_chunk, tmp_new);
                } else {
                    chunk *next_chunk;
                    /*do {
                        next_chunk = m_chunk->next();
                        m_begin = next_chunk->elements.begin();
                        m_end = next_chunk->elements.end();
                        tmp_new -= N;
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
            typename checksummed_array<T, N>::const_iterator m_it;     //!< Iterator of the current chunk
            typename checksummed_array<T, N>::const_iterator m_begin;  //!<
            typename checksummed_array<T, N>::const_iterator m_end;
        };

#endif

        //TODO: Implement iterators!
        class iterator : public std::iterator<std::random_access_iterator_tag, value_type> {};
        class const_iterator : public std::iterator<std::random_access_iterator_tag, value_type> {};

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
        typedef std::reverse_iterator<iterator, value_type> reverse_iterator;
        typedef std:: reverse_iterator<const_iterator, value_type> const_reverse_iterator;
#endif

        /*! Default constructor.
        */
        explicit vector()
            : m_head(NULL), m_size(0), m_chunks(0), m_tail(NULL) {}

        /*! Constructor to initialize vector with a custom size and an optional fill value.
        * \param n Custom initial vector size.
        * \param x Optional value to fill the vector with.
        */
        vector(size_type n, const_reference x = value_type())
            : m_head(NULL), m_size(0), m_chunks(0), m_tail(NULL) {
            assign(n, x);
        }

        /*! Constructor to initialize vector with a custom value range supplied by an iterator.
        * \param first Begin of value range.
        * \param last End of value range.
        */
        template <class InputIterator>
        vector(InputIterator first, InputIterator last)
            : m_head(NULL), m_size(0), m_chunks(0), m_tail(NULL) {
            assign(first, last);
        }

        /*! Copy constructor to copy from a <code>boost::self_healing::vector</code>.
        * \param rhs The other <code>boost::self_healing::vector</code> to copy from.
        */
        vector(const boost::self_healing::vector<value_type, N> &rhs)
            : m_head(NULL), m_size(0), m_chunks(0), m_tail(NULL) {
            assign(rhs.begin(), rhs.end());
        }

        /*! Copy constructor to copy from a <code>std::vector</code>.
        * \param rhs The other <code>std::vector</code> to copy from.
        */
        vector(const std::vector<value_type> &rhs)
            : m_head(NULL), m_size(0), m_chunks(0), m_tail(NULL) {
            assign(rhs.begin(), rhs.end());
        }

        /*! Destructor.
        */
        ~vector() {
            check_head_and_tail_pointers();
            delete[] m_head;
        }

        vector<value_type, N>& operator=(const vector<value_type, N> &rhs) {
            assign(rhs.begin(), rhs.end());
        };

        template <class InputIterator>
        void assign(InputIterator first, InputIterator last) {
            check_head_and_tail_pointers();
            reserve(last - first);
            //TODO: Implement when iterators are available
        }
        template <class Size, class TT>
        void assign(Size n, const TT &x = TT()) {
            check_head_and_tail_pointers();
            reserve(n);
            //TODO: Implement first!
        }

        // iterator support
        //TODO: Fix iterator construction.
        iterator begin() { check_head_and_tail_pointers(); check_chunks(); return iterator(m_head); }
        const_iterator begin() const { check_head_and_tail_pointers(); check_chunks(); return const_iterator(m_head, m_head->elements.begin()); }
        iterator end() { check_head_and_tail_pointers(); check_chunks(); return iterator(m_tail + vector_chunk_size); }
        const_iterator end() const { check_head_and_tail_pointers(); check_chunks(); return const_iterator(m_tail, m_tail->elements.end()); }

        // reverse iterator support
        reverse_iterator rbegin() { return reverse_iterator(end()); }
        const_reverse_iterator rbegin() const { return const_reverse_iterator(end()); }
        reverse_iterator rend() { return reverse_iterator(begin()); }
        const_reverse_iterator rend() const { return const_reverse_iterator(begin()); }

        // capacity
        size_type size() const { check_size(); return m_size; }
        bool empty() const { return size() == 0; }
        size_type max_size() const { return std::numeric_limits<difference_type>::max(); }
        size_type capacity() const { check_chunks(); return m_chunks * vector_chunk_type::size(); }

        void resize(size_type new_size, value_type item = T()) {
            check_size();
            if (new_size > size()) {
                if (new_size > capacity()) {
                    reserve(new_size);
                }
                insert(end(), new_size - size(), item);
            } else {
                // Invalidate values after 'new_size', capacity remains the same.
                iterator e = end();
                erase(e - (size() - new_size), e);
            }
        }

        /*! Reserve storage capacity.
        * \param new_capacity The storage capacity to reserve.
        * \throws std::length_error
        */
        void reserve(size_type new_capacity) {
            if (capacity() < new_capacity) {
                if (new_capacity > max_size()) {
                    std::length_error e("unable to reserve capacity: " + to_string(new_capacity));
                    boost::throw_exception(e);
                }
                //TODO: Do something actually
                check_head_and_tail_pointers();
            }
        }

        // operator[]
        reference operator[](size_type i) {

        }
        const_reference operator[](size_type i) const {

        }

        // at() with range check
        reference at(size_type i) { rangecheck(i); return operator[](i); }
        const_reference at(size_type i) const { rangecheck(i); return operator[](i); }

        // front() and back()
        reference front() { return operator[](0); }
        const_reference front() const { return operator[](0); }
        reference back() { check_size(); return operator[](m_size - 1); }
        const_reference back() const { check_size(); return operator[](m_size - 1); }

        // modifiers
        iterator insert(iterator it);
        iterator insert(iterator it, const_reference value);
        void insert(iterator it, size_type, const_reference value) {
            //TODO:
        }
        template <class InputIterator>
        void insert(iterator, InputIterator, InputIterator) {
            //TODO:
        }
        void push_back(const_reference value) {
            //TODO:
        }
        iterator erase(iterator);
        iterator erase(iterator, iterator);
        void pop_back() {
            //TODO:
        }
        void clear() {
            //TODO:
            if (!empty()) {

            }
        }

        void swap(vector<value_type> &rhs) {
            boost::swap(m_head, rhs.m_head);
            boost::swap(m_tail, rhs.m_tail);
            boost::swap(m_chunks, rhs.m_chunks);
            boost::swap(m_size, rhs.m_size);
            check_head_and_tail_pointers();
            check_chunks();
            check_size();
        }

        /*! Check index validity against size.
        * \param index The index to check.
        * \throws std::out_of_range Thrown if index is out of range.
        */
        void rangecheck(size_type index) const {
#ifdef BOOST_SELF_HEALING_DEBUG
            std::cout << "boost::self_healing::vector<T, N>::rangecheck(" << index << ")" << std::endl;
#endif
            if (index >= size()) {
                std::out_of_range e("index out of range");
                boost::throw_exception(e);
            }
        }

        /*! Validity check that tries to correct minor faults silently.
        * \return true, if the internal structure and data is valid
        * \see check_size
        */
        bool is_valid() const {
#ifdef BOOST_SELF_HEALING_DEBUG
            std::cout << "boost::self_healing::vector<T, N>::is_valid()" << std::endl;
#endif
            try {
                // check all parts of the data structure
                check_head_and_tail_pointers();
                check_chunks();
                check_size();
                for (int i = 0; i < m_chunks; i++) {
                    // make sure that the head pointer is valid during the loop
                    check_head_and_tail_pointers();
                    // compute address of next chunk
                    vector_chunk_pointer chunk = m_head + i * vector_chunk_size;
                    chunk->is_valid(this);
                }
                return true;
            } catch (const std::runtime_error &e) {
#ifdef BOOST_SELF_HEALING_DEBUG
                std::cout << "boost::self_healing::vector<T, N>::is_valid() caught runtime error: " << e.what() << std::endl;
#endif
                return false;
            };
        }

    private:
        void check_head_and_tail_pointers() const {
            //TODO: Check and repair head and tail pointers
#ifdef BOOST_SELF_HEALING_DEBUG
            std::cout << "boost::self_healing::vector<T, N>::check_head_and_tail_pointers()" << std::endl;
#endif
            if (m_head == NULL && m_tail == NULL) {
                // Both are NULL, this means nothing seems to be alloced yet
                // We're fine if the other values reflect that
                if (m_size == 0 && m_chunks == 0) {
                    return;
                }
                /*check_chunks();
                check_size();*/
                //TODO: Maybe do a simpler check here instead
            } else if (m_head == NULL) {
                // Only head is null, could be error with tail or head

            } else if (m_tail == NULL) {
                // Only tail is null, could be error with tail or head

            } else {
                // Both are non-NULL, further checks

            }
            std::runtime_error e("head or tail error");
            boost::throw_exception(e);
        }

        void check_chunks() const {
#ifdef BOOST_SELF_HEALING_DEBUG
            std::cout << "boost::self_healing::vector<T, N>::check_chunks()" << std::endl;
#endif
            //TODO: Check head and tail pointer
            //check_head_and_tail_pointers();
            size_type chunk_count = ((m_tail + vector_chunk_size) - m_head) / vector_chunk_size;

            if (m_chunks != chunk_count) {
                //TODO: Repair chunk count

                std::runtime_error e("chunk count error");
                boost::throw_exception(e);
            }
        }

        void check_size() const {
#ifdef BOOST_SELF_HEALING_DEBUG
            std::cout << "boost::self_healing::vector<T, N>::check_size()" << std::endl;
#endif
            //TODO: check and repair size
            if (m_size > capacity()) {
                std::runtime_error e("size is bigger than capacity");
                boost::throw_exception(e);
            } else {
                //TODO: See if we can do other checks/fixes
            }
        }

        vector_chunk_pointer m_head;    //!< Pointer to the first chunk.
        size_type m_size;               //!< Counts how much elements are stored currently in all chunks.
        size_type m_chunks;             //!< Chunk counter.
        vector_chunk_pointer m_tail;    //!< Pointer to the last chunk.
    };

    // comparisons
    template <class T, std::size_t N>
    inline bool operator==(const vector<T, N> &x, const vector<T, N> &y) {
        return std::equal(x.begin(), x.end(), y.begin());
    }
    template <class T, std::size_t N>
    inline bool operator<(const vector<T, N> &x, const vector<T, N> &y) {
        return std::lexicographical_compare(x.begin(), x.end(), y.begin(), y.end());
    }
    template <class T, std::size_t N>
    inline bool operator!=(const vector<T, N> &x, const vector<T, N> &y) {
        return !(x == y);
    }
    template <class T, std::size_t N>
    inline bool operator>(const vector<T, N> &x, const vector<T, N> &y) {
        return y < x;
    }
    template <class T, std::size_t N>
    inline bool operator<=(const vector<T, N> &x, const vector<T, N> &y) {
        return !(y < x);
    }
    template <class T, std::size_t N>
    inline bool operator>=(const vector<T, N>& x, const vector<T, N> &y) {
        return !(x < y);
    }

    /*! Global swap function.
    */
    template<class T>
    inline void swap(vector<T> &x, vector<T> &y) {
        x.swap(y);
    }

} } // namespace boost::self_healing


/*! Overload for operator << of std::ostream to print a vector.
*/
template <class T, std::size_t N>
std::ostream &operator<<(std::ostream &os, const boost::self_healing::vector<T, N> &vector)
{
    os << "[";
    for (std::size_t i = 0; i < vector.size(); i++) {
        os << (i == 0 ? "" : ",") << vector[i];
    }
    return os << "]";
}


#endif // BOOST_SELF_HEALING_VECTOR_HPP
