/*! \file
* \brief Checksummed array.
*
* This file contains the class checksummed_array, a self-healing STL container
* (as wrapper) for arrays of constant size with checksumming. This allows to
* monitor the validity of the array's content.
*
* This class is based on boost::checksummed_array by Nicolai M. Josuttis.
* See
*      http://www.boost.org/libs/checksummed_array/
* for documentation.
*
* (C) Copyright Sascha Peilicke 2010.
*
* Distributed under the Boost Software License, Version 1.0. (See
* accompanying file LICENSE_1_0.txt or copy at
* http://www.boost.org/LICENSE_1_0.txt)
*
* 20 April 2010 - Initial Revision (Sascha Peilicke)
*/

#ifndef BOOST_SELF_HEALING_CHECKSUMMED_ARRAY_HPP
#define BOOST_SELF_HEALING_CHECKSUMMED_ARRAY_HPP

#include "./detail/safe_ref.hpp"

#include <boost/config.hpp>
#include <boost/crc.hpp>
#include <boost/detail/iterator.hpp>
#include <boost/detail/workaround.hpp>
#include <boost/swap.hpp>
#include <boost/throw_exception.hpp>

#ifdef BOOST_SELF_HEALING_DEBUG
#   include <iostream>
#endif
#include <stdexcept>


/// The namespace self_healing contains fault-tolerant data structures and utility classes.
namespace boost { namespace self_healing {

    /*! \brief Checksummed array.
    *
    * A checksummed array of constant size.
    *
    * \param T The data type of the stored values.
    * \param N The size of the checksummed_array.
    */
    template <class T, std::size_t N>
    class checksummed_array
    {
    public:
        // type definitions
        typedef T               value_type;         //!< The type of elements stored in the <code>checksummed_array</code>.
        class                   iterator;           //!< Forward declaration of class iterator.
        typedef const T *       const_iterator;     //!< A const (random access) iterator used to iterate through the <code>checksummed_array</code>.
        //typedef safe_ptr<T>     pointer;            //!< A pointer to the element.
        typedef const T *       const_pointer;      //!< A const pointer to the element.
        typedef safe_ref<T>     reference;          //!< A reference to an element.
        typedef const T &       const_reference;    //!< A const reference to an element.
        typedef std::size_t     size_type;          //!< An unsigned integral type that can represent any non-negative value of the container's distance type.
        typedef std::ptrdiff_t  difference_type;    //!< A signed integral type used to represent the distance between two iterators.
        typedef unsigned int    checksum_type;      //!< The type of the internal checksums

        /*! \brief A (random access) iterator used to iterate through the <code>checksummed_array</code>.
        *
        * A safe iterator that calls a function object if the value at the
        * current position is changed. Checksumms are also updated correctly if
        * the iterator is dereferenced.
        *
        * \see std::iterator, std::random_access_iterator_tag
        */
        class iterator : public std::iterator<std::random_access_iterator_tag, value_type>
        {
            friend class checksummed_array;

            /*! Private constructor.
            * \param rhs TODO.
            * \param check TODO.
            * \param update TODO.
            */
            explicit iterator(value_type *rhs, boost::function<void (void)> check, boost::function<void (void)> update)
                : m_p(rhs), check(check), update(update) {}

        public:
            /*! Copy constructor.
            * \param other The other iterator instance to copy from.
            */
            iterator(const iterator &other)
                : m_p(other.m_p), check(other.check), update(other.update) {}

            iterator& operator=(const iterator &rhs) { m_p = rhs.m_p; check = rhs.check; update = rhs.update; return *this; }

            iterator operator+(difference_type n) const { return iterator(m_p + n, check, update); }
            iterator operator-(difference_type n) const { return iterator(m_p - n, check, update); }
            difference_type operator+(const iterator &rhs) const { return m_p + rhs.m_p; }
            difference_type operator-(const iterator &rhs) const { return m_p - rhs.m_p; }

            iterator& operator+=(difference_type n) { m_p += n; return *this; }
            iterator& operator-=(difference_type n) { m_p -= n; return *this; }
            iterator& operator++() { ++m_p; return *this; }
            iterator& operator++(int) { m_p++; return *this; }
            iterator& operator--() { --m_p; return *this; }
            iterator& operator--(int) { m_p--; return *this; }

            // Comparison
            bool operator==(const iterator& other) const { return m_p == other.m_p; }
            bool operator!=(const iterator& other) const { return m_p != other.m_p; }
            bool operator>(const iterator &other) const { return m_p > other.m_p; }
            bool operator>=(const iterator &other) const { return m_p >= other.m_p; }
            bool operator<(const iterator &other) const { return m_p < other.m_p; }
            bool operator<=(const iterator &other) const { return m_p <= other.m_p; }

            reference operator*() const { check(); return reference(*m_p, update); }
            operator const_iterator() const { return m_p; }

            /*! Overload for operator<<() of std::ostream to print an iterator.
            */
            friend std::ostream &operator<<(std::ostream &os, const iterator &it) { return os << it.m_p; }

        private:
            value_type *m_p;                        //!< Internal pointer to the current position in the checksummed_array.
            boost::function<void (void)> check;     //!< Called to check the consistency of the corresponding checksummed_array instance.
            boost::function<void (void)> update;    //!< Called to update the checksums of the corresponding checksummed_array instance.
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


        /*! Constructor.
        * \param value An initial value that is set for all elements.
        */
        checksummed_array(const_reference value = 0) { update_checksums(); fill(value); }

        // iterator support
        iterator begin() {  return iterator(elements, check_checksums_functor(this), update_checksums_functor(this)); }
        const_iterator begin() const { return elements; }
        iterator end() { return iterator(elements + N, check_checksums_functor(this), update_checksums_functor(this)); }
        const_iterator end() const { return elements + N; }

        // reverse iterator support
        reverse_iterator rbegin() { return reverse_iterator(end()); }
        const_reverse_iterator rbegin() const { return const_reverse_iterator(end()); }
        reverse_iterator rend() { return reverse_iterator(begin()); }
        const_reverse_iterator rend() const { return const_reverse_iterator(begin()); }

        // operator[]
        reference operator[](size_type i) { check_checksums(); return reference(elements[i], update_checksums_functor(this)); }
        const_reference operator[](size_type i) const { check_checksums(); return elements[i]; }

        // at() with range check
        reference at(size_type i) { rangecheck(i); return operator[](i); }
        const_reference at(size_type i) const { rangecheck(i); return operator[](i); }

        // front() and back()
        reference front() { check_checksums(); return reference(elements[0], update_checksums_functor(this)); }
        const_reference front() const { check_checksums(); return elements[0]; }
        reference back() { check_checksums(); return reference(elements[N - 1], update_checksums_functor(this)); }
        const_reference back() const { check_checksums(); return elements[N - 1]; }

        // size is constant
        static size_type size() { return N; }
        static bool empty() { return false; }
        static size_type max_size() { return N; }
        enum { static_size = N };

        /*! Swap with other checksummed array (note: linear complexity).
        * \param other The other checksummed array to swap with.
        */
        void swap(checksummed_array<value_type, N> &other) {
            for (size_type i = 0; i < N; ++i) {
                boost::swap(elements[i], other.elements[i]);
            }
            boost::swap(crc1, other.crc1);
            boost::swap(crc2, other.crc2);
            check_checksums();
        }

        /*! Read-only direct access to data.
        */
        const_pointer data() const { check_checksums(); return elements; }

        //NOTE: this methods would bypass checksumming currently
        //pointer data() { return elements; }

        /*! Assignment operator with type conversion.
        * \param other The other checksummed_array to copy contents from.
        * \return Reference to the new checksummed_array.
        */
        template <typename T2>
        checksummed_array<value_type, N>& operator=(const checksummed_array<T2, N> &other) {
            std::copy(other.begin(), other.end(), begin());
            update_checksums();
            return *this;
        }

        // assign one value to all elements
        void assign(const_reference value) { fill(value); }    // A synonym for fill
        void fill(const_reference value) {
            std::fill_n(begin(), size(), value);
            update_checksums();
        }

        /*! Check index validity against static size.
        * \param index The index to check.
        * \throws std::out_of_range Thrown if index is out of range.
        */
        static void rangecheck(size_type index) {
#ifdef BOOST_SELF_HEALING_DEBUG
            std::cout << "boost::self_healing::checksummed_array<T, N>::rangecheck(" << index << ")" << std::endl;
#endif
            if (index >= size()) {
                std::out_of_range e("index out of range");
                boost::throw_exception(e);
            }
        }

        /*! Validity check that tries to correct minor faults silently.
        * \return true, if the internal structure and data is valid.
        * \see check_checksums()
        */
        bool is_valid() const {
#ifdef BOOST_SELF_HEALING_DEBUG
            std::cout << "boost::self_healing::checksummed_array<T, N>::is_valid()" << std::endl;
#endif
            try {
                check_checksums();
                return true;
            } catch (const std::runtime_error &e) {
#ifdef BOOST_SELF_HEALING_DEBUG
                std::cout << "boost::self_healing::checksummed_array<T, N>::is_valid() caught runtime error: " << e.what() << std::endl;
#endif
                return false;
            };
        }

    private:
        /*! \brief Validity check that tries to correct minor checksum faults silently.
        *
        * If only one out of the two stored checksums is wrong, this can be corrected.
        * If the temporary checksum computed over the current state of the data does
        * not match the (equal) values of the stored checksums, a malicious data
        * change happened and the data structure is no longer valid.
        *
        * \remarks This method is defined const to make it easier to call by other methods,
        *          it may change internal state nonetheless.
        * \throws std::runtime_error Thrown if the data was damaged and checksums mismatch.
        */
        void check_checksums() const {
#ifdef BOOST_SELF_HEALING_DEBUG
            std::cout << "boost::self_healing::checksummed_array<T, N>::check_checksums()" << std::endl;
#endif
            boost::crc_32_type crc3;
            crc3.process_bytes(&elements, N * sizeof(value_type));
            const bool equal_13 = crc1 == crc3.checksum();
            const bool equal_23 = crc2 == crc3.checksum();
            const bool equal_12 = crc1 == crc2;

            if (equal_12 && equal_13 && equal_23) {
                return;             // all fine
            } else if (equal_13) {
                const_cast<checksum_type &>(crc2) = crc1;   // fix crc2 as the others are equal
                return;             // all fine
            } else if (equal_23) {
                const_cast<checksum_type &>(crc1) = crc2;   // fix crc1 as the others are equal
                return;             // all fine
            } else if (equal_12) {
                // The computed checksum over the content is not the same as
                // the stored onces, thus the content was maliciously changed
                // and the checksummed_array is invalid.
                std::runtime_error e("data error");
                boost::throw_exception(e);
            } else {
                // All three checksums differ
                std::runtime_error e("checksum error");
                boost::throw_exception(e);
            }
        }

        /*! A private functor that calls check_checksums() for a given checksummed_array instance if called itself.
        */
        class check_checksums_functor
        {
        public:
            /*! Constructor.
            * \param parent The checksummed_array instance for which to call check_checksums().
            */
            explicit check_checksums_functor(checksummed_array<value_type, N> *parent)
                : m_parent(parent) {}

            void operator()() { m_parent->check_checksums(); }

        private:
            checksummed_array<value_type, N> *m_parent; //!< Internal reference to owning parent checksummed_array instance.
        };

        /*! Compute and store CRC checksums.
        */
        void update_checksums() {
#ifdef BOOST_SELF_HEALING_DEBUG
            std::cout << "boost::self_healing::checksummed_array<T, N>::update_checksums()" << std::endl;
#endif
            boost::crc_32_type crc;
            crc.process_bytes(&elements, N * sizeof(value_type));
            crc1 = crc.checksum();
            crc2 = crc1;
        }

        /*! A private functor that calls update_checksums() for a given checksummed_array instance if called itself.
        */
        class update_checksums_functor
        {
        public:
            /*! Constructor.
            * \param parent The checksummed_array instance for wich to call update_checksums().
            */
            explicit update_checksums_functor(checksummed_array<value_type, N> *parent)
                : m_parent(parent) {}

            void operator()() { m_parent->update_checksums(); }

        private:
            checksummed_array<value_type, N> *m_parent; //!< Internal reference to owning parent checksummed_array instance.
        };

        checksum_type crc1;         //!< Internal first checksum.
        value_type elements[N];     //!< Internal fixed-size checksummed_array of elements of type T.
        checksum_type crc2;         //!< Internal second checksum (backup).
    };

#if !defined(BOOST_NO_TEMPLATE_PARTIAL_SPECIALIZATION)
    /*! Partial template specialization for the corner case of a zero sized checksummed_array.
     */
    template<class T>
    class checksummed_array<T, 0>
    {
    public:
        // type definitions
        typedef T              value_type;      //!< The type of elements stored in the <code>checksummed_array</code>.
        typedef T *            iterator;        //!< A (random access) iterator used to iterate through the <code>checksummed_array</code>.
        typedef const T *      const_iterator;  //!< A const (random access) iterator used to iterate through the <code>checksummed_array</code>.
        typedef T *            pointer;         //!< A pointer to the element.
        typedef const T *      const_pointer;   //!< A const pointer to the element.
        typedef T &            reference;       //!< A reference to an element.
        typedef const T &      const_reference; //!< A const reference to an element.

        /*! An unsigned integral type that can represent any non-negative value of the container's distance type.
        */
        typedef std::size_t    size_type;

        /*! A signed integral type used to represent the distance between two iterators.
        */
        typedef std::ptrdiff_t difference_type;

        // reverse iterator support
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


        // iterator support
        iterator begin() { return iterator(reinterpret_cast<pointer>(this)); }
        const_iterator begin() const { return const_iterator(reinterpret_cast<const_pointer>(this)); }
        iterator end() { return begin(); }
        const_iterator end() const { return begin(); }

        // reverse iterator support
        reverse_iterator rbegin() { return reverse_iterator(end()); }
        const_reverse_iterator rbegin() const { return const_reverse_iterator(end()); }
        reverse_iterator rend() { return reverse_iterator(begin()); }
        const_reverse_iterator rend() const { return const_reverse_iterator(begin()); }

        // operator[]
        reference operator[](size_type) { return failed_rangecheck(); }
        const_reference operator[](size_type) const { return failed_rangecheck(); }

        // at() with range check
        reference at(size_type) { return failed_rangecheck(); }
        const_reference at(size_type) const { return failed_rangecheck(); }

        // front() and back()
        reference front() { return failed_rangecheck(); }
        const_reference front() const { return failed_rangecheck(); }
        reference back() { return failed_rangecheck(); }
        const_reference back() const { return failed_rangecheck(); }

        // size is constant
        static size_type size() { return 0; }
        static bool empty() { return true; }
        static size_type max_size() { return 0; }
        enum { static_size = 0 };

        void swap(checksummed_array<value_type, 0> &) {}

        // direct access to data (read-only)
        const_pointer data() const { return 0; }
        pointer data() { return 0; }

        // use checksummed_array as C checksummed_array (direct read/write access to data)
        pointer c_array() { return 0; }

        // assignment with type conversion
        template <typename T2>
        checksummed_array<value_type, 0>& operator=(const checksummed_array<T2, 0> &) { return *this; }

        // assign one value to all elements
        void assign(const_reference value) { fill(value); }
        void fill(const_reference) {}

        // check range (may be private because it is static)
        static reference failed_rangecheck() {
            std::out_of_range e("attempt to access element of an empty checksummed_array");
            boost::throw_exception(e);
#if defined(BOOST_NO_EXCEPTIONS) || !defined(BOOST_MSVC)
            //
            // We need to return something here to keep
            // some compilers happy: however we will never
            // actually get here....
            //
            static value_type placeholder;
            return placeholder;
#endif
        }
    };
#endif

    // comparisons
    template<class T, std::size_t N>
    bool operator==(const checksummed_array<T, N> &x, const checksummed_array<T, N> &y) {
        return std::equal(x.begin(), x.end(), y.begin());
    }
    template<class T, std::size_t N>
    bool operator<(const checksummed_array<T, N> &x, const checksummed_array<T, N> &y) {
        return std::lexicographical_compare(x.begin(), x.end(), y.begin(), y.end());
    }
    template<class T, std::size_t N>
    bool operator!=(const checksummed_array<T, N> &x, const checksummed_array<T, N> &y) {
        return !(x == y);
    }
    template<class T, std::size_t N>
    bool operator>(const checksummed_array<T, N> &x, const checksummed_array<T, N> &y) {
        return y < x;
    }
    template<class T, std::size_t N>
    bool operator<=(const checksummed_array<T, N> &x, const checksummed_array<T, N> &y) {
        return !(y < x);
    }
    template<class T, std::size_t N>
    bool operator>=(const checksummed_array<T, N> &x, const checksummed_array<T, N> &y) {
        return !(x < y);
    }

    /*! Global swap().
    */
    template<class T, std::size_t N>
    inline void swap(checksummed_array<T, N> &x, checksummed_array<T, N> &y) {
        x.swap(y);
    }

} } // namespace boost::self_healing


/*! Overload for operator<<() of std::ostream to print a vector.
* \param os TODO.
* \param array The array instance to print.
*/
template <class T, std::size_t N>
std::ostream &operator<<(std::ostream &os, const boost::self_healing::checksummed_array<T, N> &array)
{
    os << "[";
    for (std::size_t i = 0; i < array.size(); i++) {
        os << (i == 0 ? "" : ",") << array[i];
    }
    return os << "]";
}


#endif // BOOST_SELF_HEALING_CHECKSUMMED_ARRAY_HPP
