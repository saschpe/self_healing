/*! \file
* \brief Multiset.
*
* This file contains the class multiset, an self-healing STL multiset that
* uses a B-tree internally with checksumming. This allows to monitor the
* validity of the multiset content.
*
* (C) Copyright Sascha Peilicke 2010.
*
* Distributed under the Boost Software License, Version 1.0. (See
* accompanying file LICENSE_1_0.txt or copy at
* http://www.boost.org/LICENSE_1_0.txt)
*
* 31 May 2010 - Initial Revision (Sascha Peilicke)
*/

#ifndef BOOST_SELF_HEALING_MULTISET_HPP
#define BOOST_SELF_HEALING_MULTISET_HPP

#include "detail/safe_ref.hpp"
#include "detail/multiset_leaf.hpp"
#include "detail/multiset_node.hpp"

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

    /*! \brief Self-healing multiset that uses a B-tree internally.
    *
    * TODO.
    *
    * \param Key The data type of the stored values.
    * \param Compare TODO.
    * \param L Optional amount of children of nodes.
    * \param CS Optional (chunk) storage capacity of leaves.
    * \remarks The chunk size should be chosen based on CPU cache size.
    * \see multiset_leaf, multiset_node
    */
    template <class Key, class Compare = less<Key>, std::size_t L = 8, std::size_t CS = 64>
    class multiset
    {
        // private type definitions
        typedef multiset<Key, Compare, L, CS>   multiset_type;
        typedef multiset<Key, Compare, L, CS> * multiset_pointer;
        typedef multiset_node<Key, L, CS>       leaf_type;
        typedef multiset_node<Key, L, CS>       node_type;

    public:
        // type definitions
        typedef Key             key_type;           //!< TODO.
        typedef Key             value_type;         //!< The type of elements stored in the <code>multiset</code>.
        typedef Compare         key_compare;        //!< TODO.
        typedef Compare         value_compare;      //!< TODO.
        class                   iterator;           //!< Forward declaration of class iterator.
        class                   const_iterator;     //!< Forward declaration of class const_iterator.
        typedef Key *           pointer;            //!< A pointer to an element.
        typedef const Key *     const_pointer;      //!< A const pointer to an element.
        typedef safe_ref<Key>   reference;          //!< A reference to an element.
        typedef const Key &     const_reference;    //!< A const reference to an element.
        typedef std::size_t     size_type;          //!< An unsigned integral type that can represent any non-negative value of the container's distance type.
        typedef std::ptrdiff_t  difference_type;    //!< A signed integral type used to represent the distance between two iterators.

    public:
        /*! \brief A (random access) iterator used to iterate through the <code>multiset</code>.
        */
        class iterator : public child<multiset_type>, public std::iterator<std::random_access_iterator_tag, value_type>
        {};
#if 0
        {
            friend class multiset;

            /*! Private constructor.
            * \param parent The multiset parent.
            * \param index The index to start with.
            */
            explicit iterator(tree_pointer const parent, size_type index)
                : child<multiset_type>(parent), m_i(index) {}

        public:
            /*! Default constructor.
            * \remarks This constructor does not build a valid iterator instance, it is just
            *          there to satisfy some STL algorithms.
            */
            iterator()
                : child<multiset_type>(0), m_i(-1) {}

            iterator operator+(difference_type n) const { return iterator(child<multiset_type>::parent(), m_i + n); }
            iterator operator-(difference_type n) const { return iterator(child<multiset_type>::parent(), m_i - n); }
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

            reference operator*() const { return child<multiset_type>::parent()->at(m_i); }
            operator const_iterator() const { return const_iterator(child<multiset_type>::parent(), m_i); }

            /*! Overload for operator<<() of std::ostream to print an iterator.
            */
            friend std::ostream &operator<<(std::ostream &os, const iterator &it) { return os << it.m_i; }

        private:
            size_type m_i;                          //!< Internal index to the current element.
        };
#endif

        /*! A const (random access) iterator used to iterate through the <code>multiset</code>.
        */
        class const_iterator : public child<multiset_type>, public std::iterator<std::random_access_iterator_tag, value_type>
        {};
#if 0
            friend class multiset;

            /*! Private constructor.
            * \param parent The multiset parent.
            * \param index The index to start with.
            */
            explicit const_iterator(tree_pointer const parent, size_type index)
                : child<multiset_type>(parent), m_i(index) {}

        public:
            const_iterator operator+(difference_type n) const { return const_iterator(child<multiset_type>::parent(), m_i + n); }
            const_iterator operator-(difference_type n) const { return const_iterator(child<multiset_type>::parent(), m_i - n); }
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

            const_reference operator*() const { return child<multiset_type>::parent()->at(m_i); }

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
        * \param compare TODO.
        */
        explicit multiset(const Compare &compare = Compare()) {
            //TODO
        }

        /*! Constructor to initialize multiset with a custom value range supplied by an iterator.
        * \param first Begin of value range.
        * \param last End of value range.
        * \param compare TODO.
        */
        template <class InputIterator>
        multiset(InputIterator first, InputIterator last, const Compare &compare = Compare()) {
            //TODO
        }

        /*! Copy constructor to copy from a <code>boost::self_healing::multiset</code>.
        * \param rhs The other <code>boost::self_healing::multiset</code> to copy from.
        */
        multiset(const multiset<Key, Compare, L, CS> &rhs) {
            //TODO
        }

        /*! Copy constructor to copy from a <code>std::multiset</code>.
        * \param rhs The other <code>std::multiset</code> to copy from.
        */
        multiset(const std::multiset<Key, Compare> &rhs) {
            //TODO
        }

        /*! Destructor.
        */
        ~multiset() {
            //TODO
        }

        multiset<Key, Compare, L, CS>& operator=(const multiset<Key, Compare, L, CS> &rhs) {
            //TODO
        }

        // iterators
        iterator begin();
        const_iterator begin() const;
        iterator end();
        const_iterator end() const;

        // reverse iterator support
        reverse_iterator rbegin() { return reverse_iterator(end()); }
        const_reverse_iterator rbegin() const { return const_reverse_iterator(end()); }
        reverse_iterator rend() { return reverse_iterator(begin()); }
        const_reverse_iterator rend() const { return const_reverse_iterator(begin()); }

        // capacity
        bool empty() const;
        size_type size() const;
        size_type max_size() const;

        // modifiers
        iterator insert(const value_type &value);
        iterator insert(iterator, const value_type &value);
        template <class InputIterator>
        void insert(InputIterator first, InputIterator last);

        void erase(iterator pos);
        size_type erase(const key_type &key);
        void erase(iterator first, iterator last);
        void swap(multiset<Key, Compare, L, CS> &rhs);
        void clear();

        // Observers
        key_compare key_comp () const;
        value_compare value_comp () const;

        // multiset operations
        iterator find(const key_type &key) const;
        size_type count(const key_type &key) const;
        iterator lower_bound(const key_type &key) const;
        iterator upper_bound(const key_type &key) const;
        pair<iterator, iterator> equal_range(const key_type &key) const;

        /*! Check index validity against size.
        * \param index The index to check.
        * \throws std::out_of_range Thrown if index is out of range.
        */
        void rangecheck(size_type index) const {
#ifdef BOOST_SELF_HEALING_DEBUG
            std::cout << "boost::self_healing::multiset<Key, Compare, L, CS>::rangecheck(" << index << ")" << std::endl;
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
            std::cout << "boost::self_healing::multiset<Key, Compare, L, CS>::is_valid()" << std::endl;
#endif
            try {
                // check all parts of the data structure
                //TODO
                return true;
            } catch (const std::runtime_error &e) {
#ifdef BOOST_SELF_HEALING_DEBUG
                std::cout << "boost::self_healing::multiset<Key, Compare, L, CS>::is_valid() caught runtime error: " << e.what() << std::endl;
#endif
                return false;
            };
        }

    private:
        node_type root[3];
        //TODO
    };

    // comparisons
    template <class Key, class Compare, std::size_t L, std::size_t CS>
    inline bool operator==(const multiset<Key, L, CS> &x, const multiset<Key, L, CS> &y) {
        return std::equal(x.begin(), x.end(), y.begin());
    }
    template <class Key, class Compare, std::size_t L, std::size_t CS>
    inline bool operator<(const multiset<Key, Compare, L, CS> &x, const multiset<Key, Compare, L, CS> &y) {
        return std::lexicographical_compare(x.begin(), x.end(), y.begin(), y.end());
    }
    template <class Key, class Compare , std::size_t L, std::size_t CS>
    inline bool operator!=(const multiset<Key, Compare, L, CS> &x, const multiset<Key, Compare, L, CS> &y) {
        return !(x == y);
    }
    template <class Key, class Compare, std::size_t L, std::size_t CS>
    inline bool operator>(const multiset<Key, Compare, L, CS> &x, const multiset<Key, Compare, L, CS> &y) {
        return y < x;
    }
    template <class Key, class Compare, std::size_t L, std::size_t CS>
    inline bool operator<=(const multiset<Key, Compare, L, CS> &x, const multiset<Key, Compare, L, CS> &y) {
        return !(y < x);
    }
    template <class Key, class Compare, std::size_t L, std::size_t CS>
    inline bool operator>=(const multiset<Key, Compare, L, CS> &x, const multiset<Key, Compare, L, CS> &y) {
        return !(x < y);
    }

    /*! Global swap function.
    */
    template <class Key, class Compare, std::size_t L, std::size_t CS>
    inline void swap(multiset<Key, Compare, L, CS> &x, multiset<Key, Compare, L, CS> &y) {
        x.swap(y);
    }

} } // namespace boost::self_healing


/*! Overload for operator << of std::ostream to print a multiset.
*/
template <class Key, class Compare, std::size_t L, std::size_t CS>
std::ostream &operator<<(std::ostream &os, const boost::self_healing::multiset<Key, Compare, L/, CS> &multiset)
{
    os << "[";
    for (std::size_t i = 0; i < multiset.size(); i++) {
        os << (i == 0 ? "" : ",") << multiset[i];
    }
    return os << "]";
}


#endif // BOOST_SELF_HEALING_MULTISET_HPP
