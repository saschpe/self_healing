/*! \file
* \brief B-Tree.
*
* This file contains the class b-tree, an self-healing STL-like B-tree
* with checksumming. This allows to monitor the validity of the B-tree's
* content.
*
* (C) Copyright Sascha Peilicke 2010.
*
* Distributed under the Boost Software License, Version 1.0. (See
* accompanying file LICENSE_1_0.txt or copy at
* http://www.boost.org/LICENSE_1_0.txt)
*
* 31 May 2010 - Initial Revision (Sascha Peilicke)
*/

#ifndef BOOST_SELF_HEALING_BTREE_HPP
#define BOOST_SELF_HEALING_BTREE_HPP

#include "./detail/safe_ref.hpp"
#include "./detail/btree_leaf.hpp"
#include "./detail/btree_node.hpp"

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


/// The namespace self_healing contains fault-tolerant data structures and utility classes.
namespace boost { namespace self_healing {

    /*! \brief Self-healing B-tree.
    *
    * TODO.
    *
    * \param T The data type of the stored values.
    * \param CS Optional (chunk) storage capacity of node leaves.
    * \remarks The chunk size should be chosen based on CPU cache size.
    * \see btree_leaf, btree_node
    */
    template <class T, std::size_t CS = 64>
    class btree
    {
    public:
        // type definitions
        typedef T               value_type;         //!< The type of elements stored in the <code>btree</code>.
        class                   iterator;           //!< Forward declaration of class iterator.
        class                   const_iterator;     //!< Forward declaration of class const_iterator.
        typedef T *             pointer;            //!< A pointer to an element.
        typedef const T *       const_pointer;      //!< A const pointer to an element.
        typedef safe_ref<T>     reference;          //!< A reference to an element.
        typedef const T &       const_reference;    //!< A const reference to an element.
        typedef std::size_t     size_type;          //!< An unsigned integral type that can represent any non-negative value of the container's distance type.
        typedef std::ptrdiff_t  difference_type;    //!< A signed integral type used to represent the distance between two iterators.

    private:
        // private type definitions
    public:

        /*! \brief A (random access) iterator used to iterate through the <code>btree</code>.
        *
        * A safe iterator that calls a functor if the value at the current
        * position is changed. Checksumms are also updated correctly if the
        * iterator is dereferenced.
        */
        class iterator : public child<btree<value_type, CS> >, public std::iterator<std::random_access_iterator_tag, value_type>
        {};
#if 0
        {
            friend class btree;

            /*! Private constructor.
            * \param parent The btree parent.
            * \param index The index to start with.
            */
            explicit iterator(btree<value_type, CS> *const parent, size_type index)
                : child<btree<value_type, CS> >(parent), m_i(index) {}

        public:
            /*! Default constructor.
            * \remarks This constructor does not build a valid iterator instance, it is just
            *          there to satisfy some bad STL algorithms.
            */
            iterator()
                : child<btree<value_type, CS> >(0), m_i(-1) {}

            /*! Copy constructor.
            * \param other The other iterator instance to copy from.
            */
            iterator(const iterator &other)
                : child<btree<value_type, CS> >(other.parent()), m_i(other.m_i) {}

            iterator& operator=(const iterator &rhs) { m_i = rhs.m_i; set_parent(rhs.parent()); return *this; }

            iterator operator+(difference_type n) const { return iterator(child<btree<value_type, CS> >::parent(), m_i + n); }
            iterator operator-(difference_type n) const { return iterator(child<btree<value_type, CS> >::parent(), m_i - n); }
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

            reference operator*() const { return child<btree<value_type, CS> >::parent()->at(m_i); }
            operator const_iterator() const { return const_iterator(child<btree<value_type, CS> >::parent(), m_i); }

            /*! Overload for operator<<() of std::ostream to print an iterator.
            */
            friend std::ostream &operator<<(std::ostream &os, const iterator &it) { return os << it.m_i; }

        private:
            size_type m_i;                          //!< Internal index to the current element.
        };
#endif

        /*! A const (random access) iterator used to iterate through the <code>btree</code>.
        */
        class const_iterator : public child<btree<value_type, CS> >, public std::iterator<std::random_access_iterator_tag, value_type>
        {};
#if 0
            friend class btree;

            /*! Private constructor.
            * \param parent The btree parent.
            * \param index The index to start with.
            */
            explicit const_iterator(btree<value_type, CS> *const parent, size_type index)
                : child<btree<value_type, CS> >(parent), m_i(index) {}

        public:
            /*! Copy constructor.
            * \param other The other const_iterator instance to copy from.
            */
            const_iterator(const const_iterator &other)
                : child<btree<value_type, CS> >(other.parent()), m_i(other.m_i) {}

            const_iterator& operator=(const const_iterator &rhs) { m_i = rhs.m_i; set_parent(rhs.parent()); return *this; }

            const_iterator operator+(difference_type n) const { return const_iterator(child<btree<value_type, CS> >::parent(), m_i + n); }
            const_iterator operator-(difference_type n) const { return const_iterator(child<btree<value_type, CS> >::parent(), m_i - n); }
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

            const_reference operator*() const { return child<btree<value_type, CS> >::parent()->at(m_i); }

            /*! Overload for operator<<() of std::ostream to print a const_iterator.
            */
            friend std::ostream &operator<<(std::ostream &os, const const_iterator &it) { return os << it.m_i; }

        private:
            size_type m_i;  //!< Internal index to the current element.
        };
#endif

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
        explicit btree() {}

        /*! Constructor to initialize the B-tree with a custom size and an optional fill value.
        * \param n Custom initial B-tree size.
        * \param x Optional value to fill the B-tree with.
        */
        btree(size_type n, const_reference x = value_type()) {
            assign(n, x);
        }

        /*! Constructor to initialize the B-tree with a custom value range supplied by an iterator.
        * \param first Begin of value range.
        * \param last End of value range.
        */
        template <class InputIterator>
        btree(InputIterator first, InputIterator last) {
            assign(first, last);
        }

        /*! Copy constructor to copy from a <code>btree</code>.
        * \param rhs The other <code>btree</code> to copy from.
        */
        btree(const btree<value_type, CS> &rhs) {
            assign(rhs.begin(), rhs.end());
        }

        /*! Destructor.
        */
        ~btree() {
            //TODO:
        }

        btree<value_type, CS>& operator=(const btree<value_type, CS> &rhs) {
            assign(rhs.begin(), rhs.end());
        };

        template <class InputIterator>
            //TODO:
        void assign(InputIterator first, InputIterator last) {
        }
        template <class Size, class TT>
        void assign(Size n, const TT &x = TT()) {
            //TODO:
        }

        // iterator support
        iterator begin() {}
        const_iterator begin() const {}
        iterator end() {}
        const_iterator end() const {}

        // reverse iterator support
        reverse_iterator rbegin() { return reverse_iterator(end()); }
        const_reverse_iterator rbegin() const { return const_reverse_iterator(end()); }
        reverse_iterator rend() { return reverse_iterator(begin()); }
        const_reverse_iterator rend() const { return const_reverse_iterator(begin()); }

        // capacity
        size_type size() const {}
        bool empty() const { return size() == 0; }
        size_type max_size() const {
            // determin how much chunks fit into memory and thus how much elements we can have
            //const long int max_elems = std::numeric_limits<size_type>::max() / vector_chunk_size * CS;
            //return std::min(max_elems, std::numeric_limits<difference_type>::max());
        }
        size_type capacity() const {}

        // operator[]
        reference operator[](size_type i) {}
        const_reference operator[](size_type i) const {}

        // at() with range check
        reference at(size_type i) { rangecheck(i); return operator[](i); }
        const_reference at(size_type i) const { rangecheck(i); return operator[](i); }

        // front() and back()
        reference front() {}
        const_reference front() const {}
        reference back() {}
        const_reference back() const {}

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

        void clear() {
            if (!empty()) {
                erase(begin(), end());
            }
        }

        void swap(btree<value_type> &rhs) {
            //TODO:
        }

        /*! Check index validity against size.
        * \param index The index to check.
        * \throws std::out_of_range Thrown if index is out of range.
        */
        void rangecheck(size_type index) const {
#ifdef BOOST_SELF_HEALING_DEBUG
            std::cout << "boost::self_healing::btree<T, CS>::rangecheck(" << index << ")" << std::endl;
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
            std::cout << "boost::self_healing::btree<T, CS>::is_valid()" << std::endl;
#endif
            try {
                // check all parts of the data structure
                //TODO
                return true;
            } catch (const std::runtime_error &e) {
#ifdef BOOST_SELF_HEALING_DEBUG
                std::cout << "boost::self_healing::btree<T, CS>::is_valid() caught runtime error: " << e.what() << std::endl;
#endif
                return false;
            };
        }

    private:
        //TODO
    };

    // comparisons
    template <class T, std::size_t CS>
    inline bool operator==(const btree<T, CS> &x, const btree<T, CS> &y) {
        return std::equal(x.begin(), x.end(), y.begin());
    }
    template <class T, std::size_t CS>
    inline bool operator<(const btree<T, CS> &x, const btree<T, CS> &y) {
        return std::lexicographical_compare(x.begin(), x.end(), y.begin(), y.end());
    }
    template <class T, std::size_t CS>
    inline bool operator!=(const btree<T, CS> &x, const btree<T, CS> &y) {
        return !(x == y);
    }
    template <class T, std::size_t CS>
    inline bool operator>(const btree<T, CS> &x, const btree<T, CS> &y) {
        return y < x;
    }
    template <class T, std::size_t CS>
    inline bool operator<=(const btree<T, CS> &x, const btree<T, CS> &y) {
        return !(y < x);
    }
    template <class T, std::size_t CS>
    inline bool operator>=(const btree<T, CS>& x, const btree<T, CS> &y) {
        return !(x < y);
    }

    /*! Global swap function.
    */
    template<class T>
    inline void swap(btree<T> &x, btree<T> &y) {
        x.swap(y);
    }

} } // namespace boost::self_healing


#endif // BOOST_SELF_HEALING_BTREE_HPP
