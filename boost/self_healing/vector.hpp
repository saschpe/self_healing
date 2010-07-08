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
            bool operator==(const iterator& other) const { return i == other.i; }
            bool operator!=(const iterator& other) const { return i != other.i; }
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
            bool operator==(const const_iterator& other) const { return i == other.i; }
            bool operator!=(const const_iterator& other) const { return i != other.i; }
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
            : head(0), size1(0), chunks(0), size2(0), tail(0), size3(0) {}

        /*! Constructor to initialize vector with a custom size and an optional fill value.
        * \param n Custom initial vector size.
        * \param x Optional value to fill the vector with.
        */
        vector(size_type n, const_reference x = value_type())
            : head(0), size1(0), chunks(0), size2(0), tail(0), size3(0) {
            assign(n, x);
        }

        /*! Constructor to initialize vector with a custom value range supplied by an iterator.
        * \param first Begin of value range.
        * \param last End of value range.
        */
        template <class InputIterator>
        vector(InputIterator first, InputIterator last)
            : head(0), size1(0), chunks(0), size2(0), tail(0), size3(0) {
            assign(first, last);
        }

        /*! Copy constructor to copy from a <code>boost::self_healing::vector</code>.
        * \param rhs The other <code>boost::self_healing::vector</code> to copy from.
        */
        vector(const vector_type &rhs)
            : head(0), size1(0), chunks(0), size2(0), tail(0), size3(0) {
            assign(rhs.begin(), rhs.end());
        }

        /*! Copy constructor to copy from a <code>std::vector</code>.
        * \param rhs The other <code>std::vector</code> to copy from.
        */
        vector(const std::vector<value_type> &rhs)
            : head(0), size1(0), chunks(0), size2(0), tail(0), size3(0) {
            assign(rhs.begin(), rhs.end());
        }

        /*! Destructor.
        */
        ~vector() {
            check_storage();
            delete[] head;
        }

        vector_type& operator=(const vector_type &rhs) {
            assign(rhs.begin(), rhs.end());
        };

        template <class InputIterator>
        void assign(InputIterator first, InputIterator last) {
            resize(last - first);
            size_type i = 0;
            check_storage();
            for (InputIterator it = first; it != last; it++) {
                head[i / CS][i % CS] = *it;
                i++;
            }
            size1 = i;
            size2 = i;
            size3 = i;
        }
        template <class Size, class TT>
        void assign(Size n, const TT &x = TT()) {
            resize(n);
            size_type i = 0;
            check_storage();
            for (; i < n; i++) {
                head[i / CS][i % CS] = x;
            }
            size1 = i;
            size2 = i;
            size3 = i;
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
        size_type size() const { check_size(); return size1; }
        bool empty() const { return size() == 0; }
        size_type max_size() const {
            // determin how much chunks fit into memory and thus how much elements we can have
            const long int max_elems = std::numeric_limits<size_type>::max() / vector_chunk_size * CS;
            return std::min(max_elems, std::numeric_limits<difference_type>::max());
        }
        size_type capacity() const { check_storage(); return chunks * vector_chunk_type::size(); }

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
            if (capacity() < new_capacity) {
                const size_type new_chunk_count = std::ceil(static_cast<float>(new_capacity) / CS);
                new_capacity = new_chunk_count * CS;

                if (new_capacity > max_size()) {
                    std::length_error e("unable to reserve capacity: " + to_string(new_capacity));
                    boost::throw_exception(e);
                }

                const vector_chunk_pointer new_head = new vector_chunk_type[new_chunk_count];

                // set this vector as the parent of all vector chunks
                for (size_type c = 0; c < new_chunk_count; c++) {
                    new_head[c].set_parent(this);
                }

                check_storage();
                if (head) {
                    // copy values from old location to new location
                    for (size_type i = 0; i < size(); i++) {
                        new_head[i / CS][i % CS] = head[i / CS][i % CS];
                    };
                    delete[] head;
                }

                head = new_head;
                tail = &new_head[new_chunk_count - 1]; // set to address of last chunk
                chunks = new_chunk_count;
            }
        }

        // operator[]
        reference operator[](size_type i) { check_storage(); return head[i / CS][i % CS]; }
        const_reference operator[](size_type i) const { check_storage(); return head[i / CS][(i % CS)]; }

        // at() with range check
        reference at(size_type i) { rangecheck(i); return operator[](i); }
        const_reference at(size_type i) const { rangecheck(i); return operator[](i); }

        // front() and back()
        reference front() { check_storage(); return head.front(); }
        const_reference front() const { check_storage; return head.front(); }
        reference back() { check_storage(); return tail.back(); }
        const_reference back() const { check_storage(); return tail.back(); }

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
            size1++;
            size2++;
            size3++;

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
            size1 += n;
            size2 += n;
            size3 += n;

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
            size1 += n;
            size2 += n;
            size3 += n;

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
                check_size();
                size1--;
                size2--;
                size3--;
            }
            return position;
        }
        iterator erase(iterator first, iterator last) {
            iterator res = first, it = last;

            // move elements after the erase-range forward
            while (it != end()) {
                *res++ = *it++;
            }
            // reduce the size accordingly
            const size_type diff = last - first;
            check_size();
            size1 -= diff;
            size2 -= diff;
            size3 -= diff;
            return first;
        }

        void push_back(const_reference value) {
            if (size() < capacity()) {
                // vector is not full yet
                operator[](size1) = value;
                size1++;
                size2++;
                size3++;
            } else {
                insert(end(), value);
            }
        }
        void pop_back() {
            check_size();
            size1--;
            size2--;
            size3--;
        }

        void clear() {
            if (!empty()) {
                erase(begin(), end());
            }
        }

        void swap(vector<value_type> &rhs) {
            boost::swap(head, rhs.head);
            boost::swap(size1, rhs.size1);
            boost::swap(tail, rhs.tail);
            boost::swap(size2, rhs.size2);
            boost::swap(chunks, rhs.chunks);
            boost::swap(size3, rhs.size3);
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
            if (index != 0 && index >= size()) { // size() is 0 initially
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
                for (int i = 0; i < chunks; i++) {
                    // compute address of next chunk
                    vector_chunk_pointer chunk = head + i * vector_chunk_size;
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
#ifdef BOOST_SELF_HEALING_DEBUG
            std::cout << "boost::self_healing::vector<T, CS>::check_storage()" << std::endl;
#endif

            if (head == 0 && tail == 0 && chunks == 0) {
                return; // initial case where nothing is allocated yet
            }

            const vector_chunk_pointer test_head = dynamic_cast<vector_chunk_pointer>(head);
            const vector_chunk_pointer test_tail = dynamic_cast<vector_chunk_pointer>(tail);
            const size_type estimated_min_chunks = std::ceil(size() * CS);

#ifdef BOOST_SELF_HEALING_DEBUG
            std::cout << "boost::self_healing::vector<T, CS>::check_storage()"
                      << " head: " << test_head << " tail: " << test_tail << " this: " << this << std::endl;
#endif

            if (test_head && test_tail) {
                // compute the chunk counter difference and fix chunk counter if needed
                const size_type head_tail_diff_abs = static_cast<size_type>((tail - head) / sizeof(vector_chunk_type));
                if (chunks != head_tail_diff_abs + 1) {
#ifdef BOOST_SELF_HEALING_DEBUG
                    std::cout << "boost::self_healing::vector<T, CS>::check_storage() fix chunk counter" << std::endl;
#endif
                    const_cast<size_type &>(chunks) = head_tail_diff_abs + 1;
                }
            } else if (test_head) {
                if (estimated_min_chunks > chunks) {
                    // chunk counter was damaged and shows less than real, this may lead to a chunks loss
                    std::runtime_error e("tail and chunk counter error");
                    boost::throw_exception(e);
                    //TODO: see if fixable
                    //const_cast<size_type &>(chunks) = estimated_min_chunks;
                }
                const_cast<vector_chunk_pointer &>(tail) = &head[chunks];
            } else if (test_tail) {
                if (estimated_min_chunks > chunks) {
                    // chunk counter was damaged and shows less than real, this may lead to a chunks loss
                    std::runtime_error e("head and chunk counter error");
                    boost::throw_exception(e);
                    //TODO: see if fixable
                    //const_cast<size_type &>(chunks) = estimated_min_chunks;
                }
                const_cast<vector_chunk_pointer &>(head) = tail - (chunks - 1) * sizeof(vector_chunk_type);
            } else {
                std::runtime_error e("head and tail pointer error");
                boost::throw_exception(e);
            }

            if (test_head->parent() != this) {
#ifdef BOOST_SELF_HEALING_DEBUG
                std::cout << "boost::self_healing::vector<T, CS>::check_storage() fix parent of head" << std::endl;
#endif
                test_head->set_parent(const_cast<vector_pointer>(this));
            }
            if (test_tail->parent() != this) {
#ifdef BOOST_SELF_HEALING_DEBUG
                std::cout << "boost::self_healing::vector<T, CS>::check_storage() fix parent of tail" << std::endl;
#endif
                test_tail->set_parent(const_cast<vector_pointer>(this));
            }
        }

        void check_size() const {
#ifdef BOOST_SELF_HEALING_DEBUG
            std::cout << "boost::self_healing::vector<T, CS>::check_size()" << std::endl;
#endif
            // check and repair size via TMR voting
            const bool equal_13 = size1 == size3;
            const bool equal_23 = size2 == size3;
            const bool equal_12 = size1 == size2;

            if (equal_12 && equal_13 && equal_23) {
                // all fine
            } else if (equal_13) {
                const_cast<size_type &>(size2) = size1; // fix m_size1 as the others are equal
                // all fine
            } else if (equal_23) {
                const_cast<size_type &>(size1) = size2; // fix m_size1 as the others are equal
                // all fine
            } else if (equal_12) {
                const_cast<size_type &>(size3) = size1; // fix m_size3 as the others are equal
                // all fine
            } else {
                std::runtime_error e("size error");         // all three sizes differ
                boost::throw_exception(e);
            }

            /*if (size1 > capacity()) {
                std::runtime_error e("size is bigger than capacity");
                boost::throw_exception(e);
            }*/
        }

        vector_chunk_pointer head;  //!< Pointer to the first chunk of an array of vector_chunk instances.
        size_type size1;            //!< Counts how much elements are stored currently in all chunks.
        size_type chunks;           //!< Chunk counter.
        size_type size2;
        vector_chunk_pointer tail;  //!< Pointer to the last chunk in the array of vector_chunk instances.
        size_type size3;
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
