/*! \file
* \brief Checksummed array.
*
* This file contains the class checksummed_array, a robust STL container (as
* wrapper) for arrays of constant size with checksumming. This allows to
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
*
* http://github.com/saschpe/robust
*/

#ifndef BOOST_ROBUST_CHECKSUMMED_ARRAY_HPP
#define BOOST_ROBUST_CHECKSUMMED_ARRAY_HPP

#include <boost/detail/workaround.hpp>

#include <stdexcept>
#include <boost/swap.hpp>
#include <boost/crc.hpp>

// Handles broken standard libraries better than <iterator>
#include <boost/detail/iterator.hpp>
#include <boost/throw_exception.hpp>

// FIXES for broken compilers
#include <boost/config.hpp>

#include "./detail/nullary_function.hpp"
#include "reference.hpp"


/// The namespace robust contains fault-tolerant data structures and utility classes.
namespace boost { namespace robust {

    /*! Exception that is thrown when a checksum error happened.
    */
    class checksum_error : public std::runtime_error
    {
    public:
        explicit checksum_error(const std::string &what_arg)
            : std::runtime_error(what_arg) {}
    };

    /*! \brief Checksummed array.
    *
    * A checksummed array of constant size.
    *
    * \param T The data type of the stored values.
    * \param N The size of the checksummed_array.
    *
    * \remarks TODO.
    *
    * \see nullary_function, empty_nullary_function
    */
    template <class T, std::size_t N>
    class checksummed_array
    {
    public:
        // type definitions
        typedef T                    value_type;        //!< The type of elements stored in the <code>checksummed_array</code>.
        class                        iterator;          //!< Forward declaration of class iterator.
        //TODO: Write own const iterator
        typedef const T *            const_iterator;    //!< A const (random access) iterator used to iterate through the <code>checksummed_array</code>.
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

        /*! \brief A (random access) iterator used to iterate through the <code>checksummed_array</code>.
        *
        * A safe iterator that calls a function object if the value at the
        * current position is changed. Checksumms are also updated correctly if
        * the iterator is dereferenced.
        *
        * \see std::iterator, std::random_access_iterator_tag, nullary_function, empty_nullary_function
        */
        class iterator : public std::iterator<std::random_access_iterator_tag, T>
        {
            friend class checksummed_array;

            /*! Private constructor.
            * \param rhs TODO.
            * \param check TODO.
            * \param update TODO.
            */
            explicit iterator(T *rhs, nullary_function &check = empty_nullary_function,
                                      nullary_function &update = empty_nullary_function)
                : p(rhs), check(check), update(update) {}

            iterator& operator=(const reference &rhs) { check(); *p = rhs; update(); ++p; return *this; }

        public:
            /*! Copy constructor.
            * \param other The other iterator instance to copy from.
            */
            iterator(const iterator &other)
                : p(other.p), check(other.check), update(other.update) {}

            iterator& operator=(const iterator &rhs) { p = rhs.p; check = rhs.check; update = rhs.update; return *this; }

            iterator& operator+(difference_type n) const { return p + n; }
            iterator& operator-(difference_type n) const { return p - n; }
            difference_type operator+(const iterator &rhs) const { return p + rhs.p; }
            difference_type operator-(const iterator &rhs) const { return p - rhs.p; }

            iterator& operator+=(difference_type n) { p += n; return *this; }
            iterator& operator-=(difference_type n) { p -= n; return *this; }
            iterator& operator++() { ++p; return *this; }
            iterator& operator++(int) { p++; return *this; }
            iterator& operator--() { --p; return *this; }
            iterator& operator--(int) { p--; return *this; }

            // Comparison
            bool operator<(const iterator &other) const { return p < other.p; }
            bool operator>(const iterator &other) const { return p > other.p; }
            bool operator>=(const iterator &other) const { return p >= other.p; }
            bool operator==(const iterator& other) const { return p == other.p; }
            bool operator!=(const iterator& other) const { return p != other.p; }

            reference operator*() const { check(); return reference(*p, update); }
            operator const_iterator() const { return p; }

            /*! Overload for operator<<() of std::ostream to print an iterator.
            */
            friend std::ostream &operator<<(std::ostream &os, const iterator &it) { return os << it.p; }

        private:
            T *p;                       //!< Internal pointer to the current position in the checksummed_array.
            nullary_function &check;    //!< TODO.
            nullary_function &update;   //!< TODO.
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


        /*! Constructor.
        * \param value An initial value that is set for all elements.
        */
        checksummed_array(const T &value = 0)
            : check(this), update(this) { fill(value); }

        // iterator support
        iterator begin() {  return iterator(elements, check, update); }
        const_iterator begin() const { return elements; }
        iterator end() { return iterator(elements + N, check, update); }
        const_iterator end() const { return elements + N; }

        // reverse iterator support
        reverse_iterator rbegin() { return reverse_iterator(end()); }
        const_reverse_iterator rbegin() const { return const_reverse_iterator(end()); }
        reverse_iterator rend() { return reverse_iterator(begin()); }
        const_reverse_iterator rend() const { return const_reverse_iterator(begin()); }

        // operator[] with range check
        reference operator[](size_type i) { rangecheck(i); check_and_repair_checksums(); return reference(elements[i], update); }
        const_reference operator[](size_type i) const { rangecheck(i); check_and_repair_checksums(); return elements[i]; }

        // at() with range check
        reference at(size_type i) { rangecheck(i); check_and_repair_checksums(); return reference(elements[i], update); }
        const_reference at(size_type i) const { rangecheck(i); check_and_repair_checksums(); return elements[i]; }

        // front() and back()
        reference front() { check_and_repair_checksums(); return reference(elements[0], update); }
        const_reference front() const { check_and_repair_checksums(); return elements[0]; }
        reference back() { check_and_repair_checksums(); return reference(elements[N - 1], update); }
        const_reference back() const { check_and_repair_checksums(); return elements[N - 1]; }

        // size is constant
        static size_type size() { return N; }
        static bool empty() { return false; }
        static size_type max_size() { return N; }
        enum { static_size = N };

        /*! Swap with other checksummed array (note: linear complexity).
        * \param other The other checksummed array to swap with.
        */
        void swap(checksummed_array<T, N> &other) {
            for (size_type i = 0; i < N; ++i) {
                boost::swap(elements[i], other.elements[i]);
            }
            boost::swap(crc1, other.crc1);
            boost::swap(crc2, other.crc2);
            check_and_repair_checksums();
        }

        /*! Read-only direct access to data.
        */
        const_pointer data() const { check_and_repair_checksums(); return elements; }

        //NOTE: this methods would bypass checksumming currently
        //pointer data() { return elements; }

        /*! Assignment operator with type conversion.
        * \param other The other checksummed_array to copy contents from.
        * \return Reference to the new checksummed_array.
        */
        template <typename T2>
        checksummed_array<T, N>& operator=(const checksummed_array<T2, N> &other) {
            std::copy(other.begin(), other.end(), begin());
            update_checksums();
            return *this;
        }

        // assign one value to all elements
        void assign(const T &value) { fill(value); }    // A synonym for fill
        void fill(const T &value) {
            std::fill_n(begin(), size(), value);
            update_checksums();
        }

        /*! Check index validity against static size.
        * \param index The index to check.
        * \throws std::out_of_range
        */
        static void rangecheck(size_type index) {
            if (index >= size()) {
                std::out_of_range e("checksummed_array<>: index out of range");
                boost::throw_exception(e);
            }
        }

        /*! \brief Validity check that tries to correct minor checksum faults silently.
        *
        * \return true, if the internal structure and data is valid.
        *
        * \see check_and_repair_checksums()
        */
        bool is_valid() {
            try {
                check_and_repair_checksums();
                return true;
            } catch (checksum_error const & e) {
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
        * \remark This method is defined const to make it easier to call by other methods,
        *         it may change internal state nonetheless.
        *
        * \throws checksum_error Thrown if the data was damaged and checksums mismatch.
        */
        void check_and_repair_checksums() const {
            //std::cout << "boost::robust::checksummed_array<T, N>::check_and_repair_checksums()" << std::endl;
            boost::crc_32_type crc3;
            crc3.process_bytes(&elements, N * sizeof(T));
            const bool equal_13 = crc1 == crc3.checksum();
            const bool equal_23 = crc2 == crc3.checksum();
            const bool equal_12 = crc1 == crc2;

            if (equal_12 && equal_13 && equal_23) {
                return;             // all fine
            }
            if (equal_13) {
                const_cast<unsigned int &>(crc2) = crc1;    // fix crc2 as the others are equal
                return;             // all fine
            }
            if (equal_23) {
                const_cast<unsigned int &>(crc1) = crc2;    // fix crc1 as the others are equal
                return;             // all fine
            }
            if (equal_12) {
                // The computed checksum over the content is not the same as
                // the stored onces, thus the content was maliciously changed
                // and the checksummed_array is invalid.
                checksum_error e("checksummed_array<>: data error");
                boost::throw_exception(e);
            }
            // All three checksums differ
            checksum_error e("checksummed_array<>: checksum error");
            boost::throw_exception(e);
        }

        /*! \brief Check and repair checksums functor.
        *
        * A private functor implementation that calls check_and_repair_checksums() for a
        * given checksummed_array instance if called itself.
        *
        * \see nullary_function
        */
        class check_and_repair_checksums_functor : public nullary_function
        {
        public:
            /*! Constructor.
            * \param parent The checksummed_array instance that owns the functor instance.
            */
            explicit check_and_repair_checksums_functor(checksummed_array<T, N> *parent)
                : m_parent(parent) {}

            void operator()() { m_parent->check_and_repair_checksums(); }

        private:
            checksummed_array<T, N> *m_parent;  //!< Internal reference to owning parent checksummed_array instance.
        } check;                                //!< Internal functor instance to pass to reference instances.


        /*! Compute and store CRC checksums.
        */
        void update_checksums() {
            //std::cout << "boost::robust::checksummed_array<T, N>::update_checksums()" << std::endl;
            boost::crc_32_type crc;
            crc.process_bytes(&elements, N * sizeof(T));
            crc1 = crc.checksum();
            crc2 = crc1;
        }

        /*! \brief Update checksums functor.
        *
        * A private functor implementation that calls update_checksums() for a
        * given checksummed_array instance if called itself.
        *
        * \see nullary_function
        */
        class update_checksums_functor : public nullary_function
        {
        public:
            /*! Constructor.
            * \param parent The checksummed_array instance that owns the functor instance.
            */
            explicit update_checksums_functor(checksummed_array<T, N> *parent)
                : m_parent(parent) {}

            void operator()() { m_parent->update_checksums(); }

        private:
            checksummed_array<T, N> *m_parent;  //!< Internal reference to owning parent checksummed_array instance.
        } update;                               //!< Internal functor instance to pass to reference instances.

        unsigned int crc1;      //!< Internal first checksum.
        T elements[N];          //!< Internal fixed-size checksummed_array of elements of type T.
        unsigned int crc2;      //!< Internal second checksum (backup).
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

        /*! \brief The size type.
        *
        * An unsigned integral type that can represent any non-negative value of the container's distance type.
        */
        typedef std::size_t    size_type;

        /*! \brief The distance type.
        *
        * A signed integral type used to represent the distance between two iterators.
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
        typedef std::reverse_iterator<iterator, T> reverse_iterator;
        typedef std::reverse_iterator<const_iterator, T> const_reverse_iterator;
#endif


        // iterator support
        iterator begin() { return iterator(reinterpret_cast<T *>(this)); }
        const_iterator begin() const { return const_iterator(reinterpret_cast<const T *>(this)); }
        iterator end() { return begin(); }
        const_iterator end() const { return begin(); }

        reverse_iterator rbegin() { return reverse_iterator(end()); }
        const_reverse_iterator rbegin() const { return const_reverse_iterator(end()); }
        reverse_iterator rend() { return reverse_iterator(begin()); }
        const_reverse_iterator rend() const { return const_reverse_iterator(begin()); }

        // operator[]
        reference operator[](size_type /*i*/) { return failed_rangecheck(); }
        const_reference operator[](size_type /*i*/) const { return failed_rangecheck(); }

        // at() with range check
        reference at(size_type /*i*/) { return failed_rangecheck(); }
        const_reference at(size_type /*i*/) const { return failed_rangecheck(); }

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

        void swap(checksummed_array<T, 0> &/*y*/) {}

        // direct access to data (read-only)
        const_pointer data() const { return 0; }
        pointer data() { return 0; }

        // use checksummed_array as C checksummed_array (direct read/write access to data)
        pointer c_checksummed_array() { return 0; }

        // assignment with type conversion
        template <typename T2>
        checksummed_array<T, 0>& operator=(const checksummed_array<T2, 0> &) { return *this; }

        // assign one value to all elements
        void assign(const T &value) { fill(value); }
        void fill(const T &/*value*/) {}

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
            static T placeholder;
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

} } // namespace boost::robust


/*! Overload for operator<<() of std::ostream to print a vector.
* \param os TODO.
* \param array The array instance to print.
*/
template <class T, std::size_t N>
std::ostream &operator<<(std::ostream &os, const boost::robust::checksummed_array<T, N> &array)
{
    os << "[";
    for (std::size_t i = 0; i < array.size(); i++) {
        os << (i == 0 ? "" : ",") << array[i];
    }
    return os << "]";
}


#endif // BOOST_ROBUST_CHECKSUMMED_ARRAY_HPP
