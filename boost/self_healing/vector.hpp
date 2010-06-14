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

#include "detail/safe_ref.hpp"
#include "detail/vector_chunk.hpp"
#include "detail/utility.hpp"

#include <boost/config.hpp>
#include <boost/detail/iterator.hpp>
#include <boost/detail/workaround.hpp>
#include <boost/swap.hpp>
#include <boost/throw_exception.hpp>

#include <algorithm>
#include <cmath>
#ifdef BOOST_SELF_HEALING_DEBUG
#   include <iostream>
#endif
#include <stdexcept>
#include <vector>


/// The namespace self_healing contains fault-tolerant data structures and utility classes.
namespace boost { namespace self_healing {

    /*! \brief Self-healing vector.
    *
    * TODO.
    *
    * \param T The data type of the stored values.
    * \param CS Optional storage capacity of the internal chunks.
    * \remarks The chunk size should be chosen based on CPU cache size.
    * \see std:vector, vector_chunk<T, CS>
    */
    template <class T, std::size_t CS = 64>
    class vector
    {
        // private type definitions
        typedef vector<T, CS>         vector_type;
        typedef vector<T, CS> *       vector_pointer;
        typedef vector_chunk<T, CS>   vector_chunk_type;    //!< A vector chunk.
        typedef vector_chunk<T, CS> * vector_chunk_pointer; //!< A pointer to vector chunk.

        static const std::size_t      vector_chunk_size = sizeof(vector_chunk_type); //!< The size of a vector chunk.

    public:
        // type definitions
        typedef T               value_type;         //!< The type of elements stored in the <code>vector</code>.
        class                   iterator;           //!< Forward declaration of class iterator.
        class                   const_iterator;     //!< Forward declaration of class const_iterator.
        typedef T *             pointer;            //!< A pointer to an element.
        typedef const T *       const_pointer;      //!< A const pointer to an element.
        typedef safe_ref<T>     reference;          //!< A reference to an element.
        typedef const T &       const_reference;    //!< A const reference to an element.
        typedef std::size_t     size_type;          //!< An unsigned integral type that can represent any non-negative value of the container's distance type.
        typedef std::ptrdiff_t  difference_type;    //!< A signed integral type used to represent the distance between two iterators.

    public:
        /*! \brief A (random access) iterator used to iterate through the <code>vector</code>.
        *
        * A safe iterator that calls a functor if the value at the current
        * position is changed. Checksumms are also updated correctly if the
        * iterator is dereferenced.
        */
        class iterator : public child<vector_type>, public std::iterator<std::random_access_iterator_tag, value_type>
        {
            friend class vector;

            /*! Private constructor.
            * \param parent The vector parent.
            * \param index The index to start with.
            */
            explicit iterator(vector_pointer const parent, size_type index)
                : child<vector_type>(parent), m_i(index) {}

        public:
            /*! Default constructor.
            * \remarks This constructor does not build a valid iterator instance, it is just
            *          there to satisfy some bad STL algorithms.
            */
            iterator()
                : child<vector_type>(0), m_i(-1) {}

            /*! Copy constructor.
            * \param other The other iterator instance to copy from.
            */
            iterator(const iterator &other)
                : child<vector_type>(other.parent()), m_i(other.m_i) {}

            iterator& operator=(const iterator &rhs) { m_i = rhs.m_i; set_parent(rhs.parent()); return *this; }

            iterator operator+(difference_type n) const { return iterator(child<vector_type>::parent(), m_i + n); }
            iterator operator-(difference_type n) const { return iterator(child<vector_type>::parent(), m_i - n); }
            difference_type operator+(const iterator &rhs) const { return m_i + rhs.m_i; }
            difference_type operator-(const iterator &rhs) const { return m_i - rhs.m_i; }

            iterator& operator+=(difference_type n) { m_i += n; return *this; }
            iterator& operator-=(difference_type n) { m_i -= n; return *this; }
            iterator& operator++() { ++m_i; return *this; }
            iterator& operator++(int) { m_i++; return *this; }
            iterator& operator--() { --m_i; return *this; }
            iterator& operator--(int) { m_i--; return *this; }

            // comparison
            bool operator==(const iterator& other) const { return m_i == other.m_i; }
            bool operator!=(const iterator& other) const { return m_i != other.m_i; }
            bool operator>(const iterator &other) const { return m_i > other.m_i; }
            bool operator>=(const iterator &other) const { return m_i >= other.m_i; }
            bool operator<(const iterator &other) const { return m_i < other.m_i; }
            bool operator<=(const iterator &other) const { return m_i <= other.m_i; }

            reference operator*() const { return child<vector_type>::parent()->at(m_i); }
            operator const_iterator() const { return const_iterator(child<vector_type>::parent(), m_i); }

            /*! Overload for operator<<() of std::ostream to print an iterator.
            */
            friend std::ostream &operator<<(std::ostream &os, const iterator &it) { return os << it.m_i; }

        private:
            size_type m_i;                          //!< Internal index to the current element.
        };

        /*! A const (random access) iterator used to iterate through the <code>vector</code>.
        */
        class const_iterator : public child<vector_type>, public std::iterator<std::random_access_iterator_tag, value_type>
        {
            friend class vector;

            /*! Private constructor.
            * \param parent The vector parent.
            * \param index The index to start with.
            */
            explicit const_iterator(vector_pointer const parent, size_type index)
                : child<vector_type>(parent), m_i(index) {}

        public:
            /*! Copy constructor.
            * \param other The other const_iterator instance to copy from.
            */
            const_iterator(const const_iterator &other)
                : child<vector_type>(other.parent()), m_i(other.m_i) {}

            const_iterator& operator=(const const_iterator &rhs) { m_i = rhs.m_i; set_parent(rhs.parent()); return *this; }

            const_iterator operator+(difference_type n) const { return const_iterator(child<vector_type>::parent(), m_i + n); }
            const_iterator operator-(difference_type n) const { return const_iterator(child<vector_type>::parent(), m_i - n); }
            difference_type operator+(const const_iterator &rhs) const { return m_i + rhs.m_i; }
            difference_type operator-(const const_iterator &rhs) const { return m_i - rhs.m_i; }

            const_iterator& operator+=(difference_type n) { m_i += n; return *this; }
            const_iterator& operator-=(difference_type n) { m_i -= n; return *this; }
            const_iterator& operator++() { ++m_i; return *this; }
            const_iterator& operator++(int) { m_i++; return *this; }
            const_iterator& operator--() { --m_i; return *this; }
            const_iterator& operator--(int) { m_i--; return *this; }

            // comparison
            bool operator==(const const_iterator& other) const { return m_i == other.m_i; }
            bool operator!=(const const_iterator& other) const { return m_i != other.m_i; }
            bool operator>(const const_iterator &other) const { return m_i > other.m_i; }
            bool operator>=(const const_iterator &other) const { return m_i >= other.m_i; }
            bool operator<(const const_iterator &other) const { return m_i < other.m_i; }
            bool operator<=(const const_iterator &other) const { return m_i <= other.m_i; }

            const_reference operator*() const { return child<vector_type>::parent()->at(m_i); }

            /*! Overload for operator<<() of std::ostream to print a const_iterator.
            */
            friend std::ostream &operator<<(std::ostream &os, const const_iterator &it) { return os << it.m_i; }

        private:
            size_type m_i;  //!< Internal index to the current element.
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
        typedef std::reverse_iterator<iterator, value_type> reverse_iterator;
        typedef std::reverse_iterator<const_iterator, value_type> const_reverse_iterator;
#endif


        /*! Default constructor.
        */
        explicit vector()
            : m_head(0), m_size1(0), m_chunks(0), m_size2(0), m_tail(0), m_size3(0) {}

        /*! Constructor to initialize vector with a custom size and an optional fill value.
        * \param n Custom initial vector size.
        * \param x Optional value to fill the vector with.
        */
        vector(size_type n, const_reference x = value_type())
            : m_head(0), m_size1(0), m_chunks(0), m_size2(0), m_tail(0), m_size3(0) {
            assign(n, x);
        }

        /*! Constructor to initialize vector with a custom value range supplied by an iterator.
        * \param first Begin of value range.
        * \param last End of value range.
        */
        template <class InputIterator>
        vector(InputIterator first, InputIterator last)
            : m_head(0), m_size1(0), m_chunks(0), m_size2(0), m_tail(0), m_size3(0) {
            assign(first, last);
        }

        /*! Copy constructor to copy from a <code>boost::self_healing::vector</code>.
        * \param rhs The other <code>boost::self_healing::vector</code> to copy from.
        */
        vector(const vector_type &rhs)
            : m_head(0), m_size1(0), m_chunks(0), m_size2(0), m_tail(0), m_size3(0) {
            assign(rhs.begin(), rhs.end());
        }

        /*! Copy constructor to copy from a <code>std::vector</code>.
        * \param rhs The other <code>std::vector</code> to copy from.
        */
        vector(const std::vector<value_type> &rhs)
            : m_head(0), m_size1(0), m_chunks(0), m_size2(0), m_tail(0), m_size3(0) {
            assign(rhs.begin(), rhs.end());
        }

        /*! Destructor.
        */
        ~vector() {
            check_storage();
            delete[] m_head;
        }

        vector_type& operator=(const vector_type &rhs) {
            assign(rhs.begin(), rhs.end());
        };

        template <class InputIterator>
        void assign(InputIterator first, InputIterator last) {
            check_storage();
            resize(last - first);
            size_type i = 0;
            for (InputIterator it = first; it != last; it++) {
                m_head[i / CS][i % CS] = *it;
                i++;
            }
            m_size1 = i;
            m_size2 = i;
            m_size3 = i;
        }
        template <class Size, class TT>
        void assign(Size n, const TT &x = TT()) {
            check_storage();
            resize(n);
            size_type i = 0;
            for (; i < n; i++) {
                m_head[i / CS][i % CS] = x;
            }
            m_size1 = i;
            m_size2 = i;
            m_size3 = i;
        }

        // iterator support
        iterator begin() { check_storage(); return iterator(this, 0); }
        const_iterator begin() const { check_storage(); return const_iterator(this, 0); }
        iterator end() { check_storage(); check_size(); return iterator(this, size()); }
        const_iterator end() const { check_storage(); check_size(); return const_iterator(this, size()); }

        // reverse iterator support
        reverse_iterator rbegin() { return reverse_iterator(end()); }
        const_reverse_iterator rbegin() const { return const_reverse_iterator(end()); }
        reverse_iterator rend() { return reverse_iterator(begin()); }
        const_reverse_iterator rend() const { return const_reverse_iterator(begin()); }

        // capacity
        size_type size() const { check_size(); return m_size1; }
        bool empty() const { return size() == 0; }
        size_type max_size() const {
            // determin how much chunks fit into memory and thus how much elements we can have
            const long int max_elems = std::numeric_limits<size_type>::max() / vector_chunk_size * CS;
            return std::min(max_elems, std::numeric_limits<difference_type>::max());
        }
        size_type capacity() const { check_storage(); return m_chunks * vector_chunk_type::size(); }

        void resize(size_type new_size, value_type item = T()) {
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
                // check_storage(); // Already done in capacity() method call

                const vector_chunk_pointer old_head = m_head;
                const size_type new_chunk_count = std::ceil(new_capacity / CS);

                m_head = new vector_chunk_type[new_chunk_count];
                m_chunks = new_chunk_count;

                // copy values from old location to new
                for (size_type i = 0; i < size(); i++) {
                    m_head[i / CS][i % CS] = old_head[i / CS][i % CS];
                };

                delete[] old_head;
            }
        }

        // operator[]
        reference operator[](size_type i) { check_storage(); return m_head[i / CS][i % CS]; }
        const_reference operator[](size_type i) const { check_storage(); return m_head[i / CS][(i % CS)]; }

        // at() with range check
        reference at(size_type i) { rangecheck(i); return operator[](i); }
        const_reference at(size_type i) const { rangecheck(i); return operator[](i); }

        // front() and back()
        reference front() { check_storage(); return m_head.front(); }
        const_reference front() const { check_storage; return m_head.front(); }
        reference back() { check_storage(); return m_tail.back(); }
        const_reference back() const { check_storage(); return m_tail.back(); }

        // modifiers
        iterator insert(iterator it) {
            //TODO:
        }
        iterator insert(iterator it, const_reference value);
        void insert(iterator it, size_type, const_reference value) {
            //TODO:
        }
        template <class InputIterator>
        void insert(iterator, InputIterator, InputIterator) {
            //TODO:
        }

        iterator erase(iterator)  {
            //TODO:
        }
        iterator erase(iterator, iterator) {
            //TODO:
        }

        void push_back(const_reference value) {
            if (size() == capacity()) {
                //TODO:

            }
        }
        void pop_back() {
            check_size();
            m_size1--;
            m_size2--;
            m_size3--;
        }

        void clear() {
            if (!empty()) {
                erase(begin(), end());
            }
        }

        void swap(vector<value_type> &rhs) {
            boost::swap(m_head, rhs.m_head);
            boost::swap(m_size1, rhs.m_size1);
            boost::swap(m_tail, rhs.m_tail);
            boost::swap(m_size2, rhs.m_size2);
            boost::swap(m_chunks, rhs.m_chunks);
            boost::swap(m_size3, rhs.m_size3);
            check_storage();
            check_size();
        }

        /*! Check index validity against size.
        * \param index The index to check.
        * \throws std::out_of_range Thrown if index is out of range.
        */
        void rangecheck(size_type index) const {
#ifdef BOOST_SELF_HEALING_DEBUG
            std::cout << "boost::self_healing::vector<T, CS>::rangecheck(" << index << ")" << std::endl;
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
            std::cout << "boost::self_healing::vector<T, CS>::is_valid()" << std::endl;
#endif
            try {
                // check all parts of the data structure
                check_storage();
                check_size();
                for (int i = 0; i < m_chunks; i++) {
                    // compute address of next chunk
                    vector_chunk_pointer chunk = m_head + i * vector_chunk_size;
                    chunk->is_valid(this);
                }
                return true;
            } catch (const std::runtime_error &e) {
#ifdef BOOST_SELF_HEALING_DEBUG
                std::cout << "boost::self_healing::vector<T, CS>::is_valid() caught runtime error: " << e.what() << std::endl;
#endif
                return false;
            };
        }

    private:
        void check_storage() const {
            //TODO: Check and repair head and tail pointers
#ifdef BOOST_SELF_HEALING_DEBUG
            std::cout << "boost::self_healing::vector<T, CS>::check_storage()" << std::endl;
#endif
            if (m_head == 0 && m_tail == 0) {
                // Both are 0, this means nothing seems to be alloced yet
                // We're fine if the other values reflect that
                if (size() == 0 && m_chunks == 0) {
                    return;
                }
                /*check_size();*/
                //TODO: Maybe do a simpler check here instead
            } else if (m_head == 0) {
                // Only head is null, could be error with tail or head

            } else if (m_tail == 0) {
                // Only tail is null, could be error with tail or head

            } else {
                // Both are non-0, further checks

            }
            /*size_type chunk_count = ((m_tail + vector_chunk_size) - m_head) / vector_chunk_size;

            if (m_chunks != chunk_count) {
                //TODO: Repair chunk count

                std::runtime_error e("chunk count error");
                boost::throw_exception(e);
            }*/
            std::runtime_error e("head or tail error");
            boost::throw_exception(e);
        }

        void check_size() const {
#ifdef BOOST_SELF_HEALING_DEBUG
            std::cout << "boost::self_healing::vector<T, CS>::check_size()" << std::endl;
#endif
            // check and repair size via TMR voting
            const bool equal_13 = m_size1 == m_size3;
            const bool equal_23 = m_size2 == m_size3;
            const bool equal_12 = m_size1 == m_size2;

            if (equal_12 && equal_13 && equal_23) {
                // all fine
            } else if (equal_13) {
                const_cast<size_type &>(m_size2) = m_size1; // fix m_size1 as the others are equal
                // all fine
            } else if (equal_23) {
                const_cast<size_type &>(m_size1) = m_size2; // fix m_size1 as the others are equal
                // all fine
            } else if (equal_12) {
                const_cast<size_type &>(m_size3) = m_size1; // fix m_size3 as the others are equal
                // all fine
            } else {
                std::runtime_error e("size error");         // all three sizes differ
                boost::throw_exception(e);
            }

            if (m_size1 > capacity()) {
                std::runtime_error e("size is bigger than capacity");
                boost::throw_exception(e);
            }
        }

        vector_chunk_pointer m_head;    //!< Pointer to the first chunk of an array of vector_chunk instances.
        size_type m_size1;              //!< Counts how much elements are stored currently in all chunks.
        size_type m_chunks;             //!< Chunk counter.
        size_type m_size2;
        vector_chunk_pointer m_tail;    //!< Pointer to the last chunk in the array of vector_chunk instances.
        size_type m_size3;
    };

    // comparisons
    template <class T, std::size_t CS>
    inline bool operator==(const vector<T, CS> &x, const vector<T, CS> &y) {
        return std::equal(x.begin(), x.end(), y.begin());
    }
    template <class T, std::size_t CS>
    inline bool operator<(const vector<T, CS> &x, const vector<T, CS> &y) {
        return std::lexicographical_compare(x.begin(), x.end(), y.begin(), y.end());
    }
    template <class T, std::size_t CS>
    inline bool operator!=(const vector<T, CS> &x, const vector<T, CS> &y) {
        return !(x == y);
    }
    template <class T, std::size_t CS>
    inline bool operator>(const vector<T, CS> &x, const vector<T, CS> &y) {
        return y < x;
    }
    template <class T, std::size_t CS>
    inline bool operator<=(const vector<T, CS> &x, const vector<T, CS> &y) {
        return !(y < x);
    }
    template <class T, std::size_t CS>
    inline bool operator>=(const vector<T, CS> &x, const vector<T, CS> &y) {
        return !(x < y);
    }

    /*! Global swap function.
    */
    template<class T, std::size_t CS>
    inline void swap(vector<T, CS> &x, vector<T, CS> &y) {
        x.swap(y);
    }

} } // namespace boost::self_healing


/*! Overload for operator << of std::ostream to print a vector.
*/
template <class T, std::size_t CS>
std::ostream &operator<<(std::ostream &os, const boost::self_healing::vector<T, CS> &vector)
{
    os << "[";
    for (std::size_t i = 0; i < vector.size(); i++) {
        os << (i == 0 ? "" : ",") << vector[i];
    }
    return os << "]";
}


#endif // BOOST_SELF_HEALING_VECTOR_HPP
