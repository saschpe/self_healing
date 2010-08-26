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

#include "array.hpp"
#include "detail/child.hpp"
#include "detail/safe_ref.hpp"
#include "detail/sized.hpp"
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
    * \param ChunkSize Optional storage capacity of the internal chunks.
    * \remarks The chunk size should be chosen based on CPU cache size.
    * \see std:vector, chunk<T, ChunkSize>
    */
    template <class T, std::size_t ChunkSize = 64>
    class vector : private sized
    {
        // private type definitions
        typedef vector<T, ChunkSize>   vector_type;
        typedef vector<T, ChunkSize> * vector_pointer;
        typedef vector<T, ChunkSize> & vector_reference;

        /*! \brief Vector element storage chunk.
        *
        * A chunk is a checksummed array and a child and part of the self-healing
        * vector storage mechanism
        * \see child, array
        */
        struct chunk : public child<vector_type>, public array<T, ChunkSize>
        {
            explicit chunk(vector_pointer const parent = 0, const T &value = 0)
                : child<vector_type>(parent), array<T, ChunkSize>(value) {}

            bool is_valid(vector_pointer const parent = 0) const {
                return child<vector_type>::is_valid(parent) &&
                       array<T, ChunkSize>::is_valid();
            }
        };

        typedef chunk   chunk_type;    //!< A vector chunk.
        typedef chunk * chunk_pointer; //!< A pointer to vector chunk.

    public:
        // type definitions
        typedef T               value_type;         //!< The type of elements stored in the <code>vector</code>.
        class                   iterator;           //!< Forward declaration of class iterator.
        class                   const_iterator;     //!< Forward declaration of class const_iterator.
        typedef T *             pointer;            //!< A pointer to an element.
        typedef const T *       const_pointer;      //!< A const pointer to an element.
        typedef safe_ref<T>     reference;          //!< A reference to an element.
        typedef const T &       const_reference;    //!< A const reference to an element.
        typedef typename sized::size_type size_type;//!< An unsigned integral type that can represent any non-negative value of the container's distance type.
        typedef std::ptrdiff_t  difference_type;    //!< A signed integral type used to represent the distance between two iterators.

        /*! \brief A (random access) iterator used to iterate through the <code>vector</code>.
        */
        class iterator : public child<vector_type>, public std::iterator<std::random_access_iterator_tag, value_type>
        {
            friend class vector;

            /*! Private constructor.
            * \param parent The vector parent.
            * \param index The index to start with.
            */
            explicit iterator(vector_pointer const parent, size_type index)
                : child<vector_type>(parent), i(index) {}

        public:
            /*! Default constructor.
            * \remarks This constructor does not build a valid iterator instance, it is just
            *          there to satisfy some STL algorithms.
            */
            iterator()
                : child<vector_type>(0), i(-1) {}

            iterator operator+(difference_type n) const { return iterator(child<vector_type>::parent(), i + n); }
            iterator operator-(difference_type n) const { return iterator(child<vector_type>::parent(), i - n); }
            difference_type operator+(const iterator &rhs) const { return i + rhs.i; }
            difference_type operator-(const iterator &rhs) const { return i - rhs.i; }

            iterator& operator+=(difference_type n) { i += n; return *this; }
            iterator& operator-=(difference_type n) { i -= n; return *this; }
            iterator& operator++() { ++i; return *this; }
            iterator& operator++(int) { i++; return *this; }
            iterator& operator--() { --i; return *this; }
            iterator& operator--(int) { i--; return *this; }

            // comparison
            bool operator==(const iterator &other) const { return i == other.i; }
            bool operator!=(const iterator &other) const { return i != other.i; }
            bool operator>(const iterator &other) const { return i > other.i; }
            bool operator>=(const iterator &other) const { return i >= other.i; }
            bool operator<(const iterator &other) const { return i < other.i; }
            bool operator<=(const iterator &other) const { return i <= other.i; }

            reference operator*() const { return child<vector_type>::parent()->at(i); }
            operator const_iterator() const { return const_iterator(child<vector_type>::parent(), i); }

            /*! Overload for operator<<() of std::ostream to print an iterator.
            */
            friend std::ostream &operator<<(std::ostream &os, const iterator &it) { return os << it.i; }

        private:
            size_type i;    //!< Internal index to the current element.
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
                : child<vector_type>(parent), i(index) {}

        public:
            const_iterator operator+(difference_type n) const { return const_iterator(child<vector_type>::parent(), i + n); }
            const_iterator operator-(difference_type n) const { return const_iterator(child<vector_type>::parent(), i - n); }
            difference_type operator+(const const_iterator &rhs) const { return i + rhs.i; }
            difference_type operator-(const const_iterator &rhs) const { return i - rhs.i; }

            const_iterator& operator+=(difference_type n) { i += n; return *this; }
            const_iterator& operator-=(difference_type n) { i -= n; return *this; }
            const_iterator& operator++() { ++i; return *this; }
            const_iterator& operator++(int) { i++; return *this; }
            const_iterator& operator--() { --i; return *this; }
            const_iterator& operator--(int) { i--; return *this; }

            // comparison
            bool operator==(const const_iterator &other) const { return i == other.i; }
            bool operator!=(const const_iterator &other) const { return i != other.i; }
            bool operator>(const const_iterator &other) const { return i > other.i; }
            bool operator>=(const const_iterator &other) const { return i >= other.i; }
            bool operator<(const const_iterator &other) const { return i < other.i; }
            bool operator<=(const const_iterator &other) const { return i <= other.i; }

            const_reference operator*() const { return child<vector_type>::parent()->at(i); }

            /*! Overload for operator<<() of std::ostream to print a const_iterator.
            */
            friend std::ostream &operator<<(std::ostream &os, const const_iterator &it) { return os << it.i; }

        private:
            size_type i;    //!< Internal index to the current element.
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
            : head(0), chunks(0), tail(0) {}

        /*! Constructor to initialize vector with a custom size and an optional fill value.
        * \param n Custom initial vector size.
        * \param x Optional value to fill the vector with.
        */
        vector(size_type n, const_reference x = value_type())
            : head(0), chunks(0), tail(0) {
            assign(n, x);
        }

        /*! Constructor to initialize vector with a custom value range supplied by an iterator.
        * \param first Begin of value range.
        * \param last End of value range.
        */
        template <class InputIterator>
        vector(InputIterator first, InputIterator last)
            : head(0), chunks(0), tail(0) {
            assign(first, last);
        }

        /*! Copy constructor to copy from a <code>boost::self_healing::vector</code>.
        * \param rhs The other <code>boost::self_healing::vector</code> to copy from.
        */
        vector(const vector_reference rhs)
            : head(0), chunks(0), tail(0) {
            assign(rhs.begin(), rhs.end());
        }

        /*! Copy constructor to copy from a <code>std::vector</code>.
        * \param rhs The other <code>std::vector</code> to copy from.
        */
        vector(const std::vector<value_type> &rhs)
            : head(0), chunks(0), tail(0) {
            assign(rhs.begin(), rhs.end());
        }

        /*! Destructor.
        */
        ~vector() {
            check_header();
            delete[] head;
        }

        vector_reference operator=(const vector_reference rhs) {
            assign(rhs.begin(), rhs.end());
        };

        template <class InputIterator>
        void assign(InputIterator first, InputIterator last) {
            resize(last - first);
            size_type i = 0;
            check_header();
            for (InputIterator it = first; it != last; it++) {
                head[i / ChunkSize][i % ChunkSize] = *it;
                i++;
            }
            sized::set_size(i);
        }
        template <class Size, class TT>
        void assign(Size n, const TT &x = TT()) {
            resize(n, x);
            /*size_type i = 0;
            check_header();
            for (; i < n; i++) {
                head[i / ChunkSize][i % ChunkSize] = x;
            }
            sized::set_size(i);*/
        }

        // iterator support
        iterator begin() { check_header(); return iterator(this, 0); }
        const_iterator begin() const { check_header(); return const_iterator(this, 0); }
        iterator end() { check_header(); return iterator(this, sized::size()); }
        const_iterator end() const { check_header(); return const_iterator(this, sized::size()); }

        // reverse iterator support
        reverse_iterator rbegin() { return reverse_iterator(end()); }
        const_reverse_iterator rbegin() const { return const_reverse_iterator(end()); }
        reverse_iterator rend() { return reverse_iterator(begin()); }
        const_reverse_iterator rend() const { return const_reverse_iterator(begin()); }

        // capacity
        size_type size() const { return sized::size(); }
        bool empty() const { return sized::size() == 0; }
        size_type max_size() const {
            // determin how much chunks fit into memory and thus how much elements we can have
            const long int max_elems = std::numeric_limits<size_type>::max() / sizeof(chunk_type) * ChunkSize;
            return std::min(max_elems, std::numeric_limits<difference_type>::max());
        }
        size_type capacity() const { check_header(); return chunks * chunk_type::size(); }

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
        * \param new_capacity The (minimum) storage capacity to reserve.
        * \throws std::length_error
        */
        void reserve(size_type new_capacity) {
            const size_type new_chunk_count = std::ceil(static_cast<float>(new_capacity) / ChunkSize);
            new_capacity = new_chunk_count * ChunkSize;

            if (new_capacity > max_size()) {
                std::length_error e("unable to reserve capacity: " + to_string(new_capacity));
                boost::throw_exception(e);
            }

            const chunk_pointer new_head = new chunk_type[new_chunk_count];
#ifdef BOOST_SELF_HEALING_DEBUG
            std::cout << "boost::self_healing::vector<T, ChunkSize>::reserve() new head: " << new_head << std::endl;
#endif

            // set this vector as the parent of all vector chunks
            for (size_type c = 0; c < new_chunk_count; c++) {
                new_head[c].set_parent(reinterpret_cast<vector_pointer>(new_head));
            }

            check_header();
            if (head) {
                // copy values from old location to new location
                for (size_type i = 0; i < std::min(size(), new_capacity); i++) {
                    new_head[i / ChunkSize][i % ChunkSize] = head[i / ChunkSize][i % ChunkSize];
                };
                delete[] head;
            }

            head = new_head;
            tail = &new_head[new_chunk_count - 1]; // set to address of last chunk
            chunks = new_chunk_count;
        }

        // operator[]
        reference operator[](size_type i) { check_header(); return head[i / ChunkSize][i % ChunkSize]; }
        const_reference operator[](size_type i) const { check_header(); return head[i / ChunkSize][(i % ChunkSize)]; }

        // at() with range check
        reference at(size_type i) { rangecheck(i); return operator[](i); }
        const_reference at(size_type i) const { rangecheck(i); return operator[](i); }

        // front() and back()
        reference front() { check_header(); return head->front(); }
        const_reference front() const { check_header(); return head->front(); }
        reference back() { check_header(); return tail->back(); }
        const_reference back() const { check_header(); return tail->back(); }

        // modifiers
        iterator insert(iterator position, const_reference value) {
            // reserve space if needed
            if (size() == capacity()) {
                reserve(capacity() + 1);
            }

            // move all elements one field backwards to make room for the new
            // element. iterate from back to front.
            iterator it = end() - 1;
            while (it > position) {
                *it = *(it - 1);
                it--;
            }
            *position = value;

            // adjust size accordingly
            set_size(sized::size() + 1);
            return position;
        }
        void insert(iterator position, size_type n, const_reference value) {
            if (n == 0) {
                return;
            }
            // reserve slightly more space if needed, simpler computation
            if (size() + n >= capacity()) {
                reserve(capacity() + n);
            }

            // if we already have elements, we may have to move them backwards
            if (size() != 0) {
                // move all elements n fields backwards to make room for the new
                // elements. iterate from back to front.
                iterator source = begin() + (size() - 1); // source is last element
                iterator target = source + n; // target is n steps to the right
                while (source >= position) {
                    *target = *source;
                    source--;
                    target--;
                }
            }

            // adjust size accordingly
            set_size(sized::size() + n);

            // insert 'n' new elements of 'value' at 'position'
            for (iterator it = position; it != position + n; it++) {
                *it = value;
            }
        }
        template <class InputIterator>
        void insert(iterator position, InputIterator first, InputIterator last) {
            const difference_type n = last - first;
            if (n <= 0) {
                return;
            }
            // reserve slightly more space if needed, simpler computation
            if (size() + n >= capacity()) {
                reserve(capacity() + n);
            }

            // move all elements n fields backwards to make room for the new
            // elements. iterate from back to front.
            iterator source = begin() + (size() - 1); // source is last element
            iterator target = source + n; // target is n steps to the right
            while (source >= position) {
                *target = *source;
                source--;
                target--;
            }

            // adjust size accordingly
            set_size(sized::size() + n);

            // insert range [first, last) of new elements at 'position'
            iterator to = position, it = first;
            while (it != last) {
                *to = *it;
                to++;
                it++;
            }
        }

        iterator erase(iterator position)  {
            if (position + 1 != end()) {
                iterator it = position + 1;
                // move all elements one position backwards
                while (it != end()) {
                    *(it - 1) = *it;
                    it++;
                }
                // set new vector size and check if we can shrink the vector
                const size_type new_size = sized::size() - 1;
                const size_type new_chunk_count = std::ceil(static_cast<float>(new_size) / ChunkSize);
                set_size(new_size);
                if (new_chunk_count < chunks) {
                    reserve(new_chunk_count * ChunkSize);
                }
            }

            return position;
        }
        iterator erase(iterator first, iterator last) {
            iterator res = first, it = last;
            // move elements after the erase-range forward
            while (it != end()) {
                *res++ = *it++;
            }
            // reduce the size accordingly and check if we can shrink the vector
            const size_type new_size = sized::size() - (last - first);
            const size_type new_chunk_count = std::ceil(static_cast<float>(new_size) / ChunkSize);
            set_size(new_size);
            if (new_chunk_count < chunks) {
                reserve(new_chunk_count * ChunkSize);
            }
            return first;
        }

        void push_back(const_reference value) {
            if (size() < capacity()) {
                // vector is not full yet
                operator[](sized::size()) = value;
                set_size(sized::size() + 1);
            } else {
                insert(end(), value);
            }
        }
        void pop_back() {
            // set new vector size and check if we can shrink the vector
            const size_type new_size = sized::size() - 1;
            const size_type new_chunk_count = std::ceil(static_cast<float>(new_size) / ChunkSize);
            set_size(new_size);
            if (new_chunk_count < chunks) {
                reserve(new_chunk_count * ChunkSize);
            }
        }

        void clear() {
            if (!empty()) {
                erase(begin(), end());
            }
        }

        void swap(vector_reference rhs) {
            boost::swap(head, rhs.head);
            boost::swap(tail, rhs.tail);
            boost::swap(chunks, rhs.chunks);

            size_type tmp_size = size();
            set_size(rhs.size());
            rhs.set_size(tmp_size);

            check_header();
        }

        /*! Check index validity against size.
        * \param index The index to check.
        * \throws std::out_of_range Thrown if index is out of range.
        */
        void rangecheck(size_type index) const {
/*#ifdef BOOST_SELF_HEALING_DEBUG
            std::cout << "boost::self_healing::vector<T, ChunkSize>::rangecheck(" << index << ")" << std::endl;
#endif*/
            if (index != 0 && index >= size()) { // size() is 0 initially
                std::out_of_range e("index out of range");
                boost::throw_exception(e);
            }
        }

        /*! Validity check that tries to correct minor faults silently.
        * \return true, if the internal structure and data is valid
        * \see check_storage
        */
        bool is_valid() const {
#ifdef BOOST_SELF_HEALING_DEBUG
            std::cout << "boost::self_healing::vector<T, ChunkSize>::is_valid()" << std::endl;
#endif
            try {
                // stage 1: check header
                check_header();
                // stage 2: check nodes
                for (size_type i = 0; i < chunks; i++) {
                    // compute address of next chunk
                    chunk_pointer chunk = head + i * sizeof(chunk_type);
                    chunk->is_valid(reinterpret_cast<vector_pointer>(head));
                }
                return sized::is_valid();
            } catch (const std::runtime_error &e) {
#ifdef BOOST_SELF_HEALING_DEBUG
                std::cout << "boost::self_healing::vector<T, ChunkSize>::is_valid() caught runtime error while checking nodes: " << e.what() << std::endl;
#endif
                return false;
            };
        }

    private:
        void check_header() const {
            if ((head == 0 && tail == 0 && chunks == 0) || size() == 0) {
                return; // initial case where nothing is allocated yet
            }

#ifdef BOOST_SELF_HEALING_DEBUG
            std::cout << "boost::self_healing::vector<T, ChunkSize>::check_header()" << std::endl;
#endif

            // check if chunk counter is valid and fix accordingly
            const size_type estimated_chunks = size() == 0 ? 1 : std::ceil(static_cast<float>(size()) / ChunkSize);
            if (chunks != estimated_chunks) {
                const_cast<size_type &>(chunks) = estimated_chunks;
            }

            if (head + (chunks - 1) != tail) {
                // either one or both pointers are damaged
                const bool head_valid = is_valid_heap_address(head);
                const bool tail_valid = is_valid_heap_address(tail);

                if (head_valid && tail_valid) {
                    // the currently imperfect heap address check passed out
                    // indicating both are correct, which is wrongs
                    std::runtime_error e("head and tail pointer error");
                    boost::throw_exception(e);
                } else if (head_valid && head->parent() == reinterpret_cast<vector_pointer>(head)) {
                    const_cast<chunk_pointer &>(tail) = head + (chunks -1);
                } else if (tail_valid && tail->parent() == reinterpret_cast<vector_pointer>(tail - (chunks - 1))) {
                    const_cast<chunk_pointer &>(head) = tail - (chunks -1);
                } else {
                    std::runtime_error e("head and tail pointer error");
                    boost::throw_exception(e);
                }
            }
        }

        chunk_pointer head;  //!< Pointer to the first chunk of an array of chunk instances.
        size_type chunks;    //!< Chunk counter.
        chunk_pointer tail;  //!< Pointer to the last chunk in the array of chunk instances.
    };

    // comparisons
    template <class T, std::size_t ChunkSize>
    inline bool operator==(const vector<T, ChunkSize> &x, const vector<T, ChunkSize> &y) {
        return std::equal(x.begin(), x.end(), y.begin());
    }
    template <class T, std::size_t ChunkSize>
    inline bool operator<(const vector<T, ChunkSize> &x, const vector<T, ChunkSize> &y) {
        return std::lexicographical_compare(x.begin(), x.end(), y.begin(), y.end());
    }
    template <class T, std::size_t ChunkSize>
    inline bool operator!=(const vector<T, ChunkSize> &x, const vector<T, ChunkSize> &y) {
        return !(x == y);
    }
    template <class T, std::size_t ChunkSize>
    inline bool operator>(const vector<T, ChunkSize> &x, const vector<T, ChunkSize> &y) {
        return y < x;
    }
    template <class T, std::size_t ChunkSize>
    inline bool operator<=(const vector<T, ChunkSize> &x, const vector<T, ChunkSize> &y) {
        return !(y < x);
    }
    template <class T, std::size_t ChunkSize>
    inline bool operator>=(const vector<T, ChunkSize> &x, const vector<T, ChunkSize> &y) {
        return !(x < y);
    }

    /*! Global swap function.
    */
    template<class T, std::size_t ChunkSize>
    inline void swap(vector<T, ChunkSize> &x, vector<T, ChunkSize> &y) {
        x.swap(y);
    }

} } // namespace boost::self_healing


/*! Overload for operator << of std::ostream to print a vector.
*/
template <class T, std::size_t ChunkSize>
std::ostream &operator<<(std::ostream &os, const boost::self_healing::vector<T, ChunkSize> &vector)
{
    os << "[";
    for (std::size_t i = 0; i < vector.size(); i++) {
        os << (i == 0 ? "" : ",") << vector[i];
    }
    return os << "]";
}


#endif // BOOST_SELF_HEALING_VECTOR_HPP
