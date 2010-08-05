/*! \file
* \brief Checksummed array.
*
* This file contains the class checksummed array, a self-healing STL container
* (as wrapper) for arrays of constant size with checksumming. This allows to
* monitor the validity of the array's content.
*
* This class is based on boost::array by Nicolai M. Josuttis.
* See
*      http://www.boost.org/libs/array
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

#ifndef BOOST_SELF_HEALING_ARRAY_HPP
#define BOOST_SELF_HEALING_ARRAY_HPP

#include "detail/safe_ref.hpp"

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
    * \param Size The size of the checksummed array.
    */
    template <class T, std::size_t Size>
    class array
    {
        // private type definitions
        typedef array<T, Size>                      array_type;
        typedef array<T, Size> *                    array_pointer;
        typedef array<T, Size> &                    array_reference;
        typedef typename safe_ref<T>::function_type function_type;

    public:
        // type definitions
        typedef T               value_type;         //!< The type of elements stored in the <code>array</code>.
        class                   iterator;           //!< Forward declaration of class iterator.
        typedef const T *       const_iterator;     //!< A const (random access) iterator used to iterate through the <code>array</code>.
        //typedef safe_ptr<T>     pointer;            //!< A pointer to an element.
        typedef T *             pointer;            //!< A pointer to an element.
        typedef const T *       const_pointer;      //!< A const pointer to an element.
        typedef safe_ref<T>     reference;          //!< A reference to an element.
        typedef const T &       const_reference;    //!< A const reference to an element.
        typedef std::size_t     size_type;          //!< An unsigned integral type that can represent any non-negative value of the container's distance type.
        typedef std::ptrdiff_t  difference_type;    //!< A signed integral type used to represent the distance between two iterators.
        typedef unsigned int    checksum_type;      //!< The type of the internal checksums

        /*! \brief A (random access) iterator used to iterate through the <code>array</code>.
        *
        * A safe iterator that calls a function object if the value at the
        * current position is changed. Checksumms are also updated correctly if
        * the iterator is dereferenced.
        *
        * \see std::iterator, std::random_access_iterator_tag
        */
        class iterator : public std::iterator<std::random_access_iterator_tag, value_type>
        {
            friend class array;

            /*! Private constructor.
            * \param rhs TODO.
            * \param check TODO.
            * \param update TODO.
            */
            explicit iterator(pointer rhs, function_type check, function_type update)
                : p(rhs), check(check), update(update) {}

        public:
            iterator operator+(difference_type n) const { return iterator(p + n, check, update); }
            iterator operator-(difference_type n) const { return iterator(p - n, check, update); }
            difference_type operator+(const iterator &rhs) const { return  p + rhs.p; }
            difference_type operator-(const iterator &rhs) const { return p - rhs.p; }

            iterator& operator+=(difference_type n) { p += n; return *this; }
            iterator& operator-=(difference_type n) { p -= n; return *this; }
            iterator& operator++() { ++p; return *this; }
            iterator& operator++(int) { p++; return *this; }
            iterator& operator--() { --p; return *this; }
            iterator& operator--(int) { p--; return *this; }

            // Comparison
            bool operator==(const iterator &other) const { return p == other.p; }
            bool operator!=(const iterator &other) const { return p != other.p; }
            bool operator>(const iterator &other) const { return p > other.p; }
            bool operator>=(const iterator &other) const { return p >= other.p; }
            bool operator<(const iterator &other) const { return p < other.p; }
            bool operator<=(const iterator &other) const { return p <= other.p; }

            reference operator*() const { check(); return reference(*p, update); }
            operator const_iterator() const { return p; }

            /*! Overload for operator<<() of std::ostream to print an iterator.
            */
            friend std::ostream &operator<<(std::ostream &os, const iterator &it) { return os << it.p; }

        private:
            pointer p;              //!< Internal pointer to the current position in the checksummed array.
            function_type check;    //!< Called to check the consistency of the corresponding checksummed array instance.
            function_type update;   //!< Called to update the checksums of the corresponding checksummed array instance.
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
        array(const_reference value = 0) { update_checksums(); fill(value); }

        // iterator support
        iterator begin() {  return iterator(elements, check_checksums_functor(this), update_checksums_functor(this)); }
        const_iterator begin() const { return elements; }
        iterator end() { return iterator(elements + Size, check_checksums_functor(this), update_checksums_functor(this)); }
        const_iterator end() const { return elements + Size; }

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
        reference back() { check_checksums(); return reference(elements[Size - 1], update_checksums_functor(this)); }
        const_reference back() const { check_checksums(); return elements[Size - 1]; }

        // size is constant
        static size_type size() { return Size; }
        static bool empty() { return false; }
        static size_type max_size() { return Size; }
        enum { static_size = Size };

        /*! Swap with other checksummed array (note: linear complexity).
        * \param other The other checksummed array to swap with.
        */
        void swap(array_reference other) {
            for (size_type i = 0; i < Size; ++i) {
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
        * \param other The other checksummed array to copy contents from.
        * \return Reference to the new checksummed array.
        */
        template <typename T2>
        array_reference operator=(const array<T2, Size> &other) {
            for (size_type i = 0; i < size(); i++) {
                elements[i] = other.elements[i];
            }
            update_checksums();
            return *this;
        }

        // assign one value to all elements
        void assign(const_reference value) { fill(value); }    // A synonym for fill
        void fill(const_reference value) {
            for (size_type i = 0; i < size(); i++) {
                elements[i] = value;
            }
            update_checksums();
        }

        /*! Check index validity against static size.
        * \param index The index to check.
        * \throws std::out_of_range Thrown if index is out of range.
        */
        static void rangecheck(size_type index) {
#ifdef BOOST_SELF_HEALING_DEBUG
            std::cout << "boost::self_healing::array<T, Size>::rangecheck(" << index << ")" << std::endl;
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
            std::cout << "boost::self_healing::array<T, Size>::is_valid()" << std::endl;
#endif
            try {
                check_checksums();
                return true;
            } catch (const std::runtime_error &e) {
#ifdef BOOST_SELF_HEALING_DEBUG
                std::cout << "boost::self_healing::array<T, Size>::is_valid() caught runtime error: " << e.what() << std::endl;
#endif
                return false;
            };
        }

        /*! Overload for operator<<() of std::ostream to print an array.
        * \param os TODO.
        * \param array The array instance to print.
        */
        friend std::ostream &operator<<(std::ostream &os, const array_reference array)
        {
            os << "[";
            for (std::size_t i = 0; i < array.size(); i++) {
                os << (i == 0 ? "" : ",") << array.elements[i];
            }
            return os << "]";
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
            std::cout << "boost::self_healing::array<T, Size>::check_checksums();" << std::endl;
#endif
            boost::crc_32_type crc3;
            crc3.process_bytes(&elements, Size * sizeof(value_type));
            const bool equal_13 = crc1 == crc3.checksum();
            const bool equal_23 = crc2 == crc3.checksum();
            const bool equal_12 = crc1 == crc2;

            if (equal_12 && equal_13 && equal_23) {
                return;             // all fine
            } else if (equal_13) {
#ifdef BOOST_SELF_HEALING_FIXING_CHECKS
                const_cast<checksum_type &>(crc2) = crc1;   // fix crc2 as the others are equal
#else
                std::runtime_error e("fixable checksum error");
                boost::throw_exception(e);
#endif
            } else if (equal_23) {
#ifdef BOOST_SELF_HEALING_FIXING_CHECKS
                const_cast<checksum_type &>(crc1) = crc2;   // fix crc1 as the others are equal
#else
                std::runtime_error e("fixable checksum error");
                boost::throw_exception(e);
#endif
            } else if (equal_12) {
                // The computed checksum over the content is not the same as
                // the stored ones, thus the content was maliciously changed
                // and the checksummed array is invalid.
#ifdef BOOST_SELF_HEALING_ADVANCED_CHECKS
                // Try to recover by flipping each bit and compare resulting
                // checksum with stored ones.
                for (std::size_t byte_index = 0; byte_index < Size * sizeof(value_type); byte_index++) {
                    // get a byte pointer from our (const) elements array and loop through all 8 bits
                    char *current_byte = reinterpret_cast<char *>(const_cast<pointer>(elements)) + byte_index;
                    for (std::size_t bit_index = 0; bit_index < 8; bit_index++) {
                        *current_byte ^= 2 << bit_index;
                        // Compute checksum. If successful break, otherwise flip back
                        boost::crc_32_type tmp_crc;
                        tmp_crc.process_bytes(&elements, Size * sizeof(value_type));
                        if (tmp_crc.checksum() == crc1) {
                            break;
                        }
                        *current_byte ^= 2 << bit_index; // Flip back
                    }
                }
                //NOTE: Possibly apply recursively for 2, 3, ... bits
#else
                std::runtime_error e("data error");
                boost::throw_exception(e);
#endif
            } else {
                // All three checksums differ
                std::runtime_error e("checksum error");
                boost::throw_exception(e);
            }
        }

        /*! A private functor that calls check_checksums() for a given checksummed array instance if called itself.
        */
        class check_checksums_functor
        {
        public:
            /*! Constructor.
            * \param parent The checksummed array instance for which to call check_checksums().
            */
            explicit check_checksums_functor(array_pointer parent)
                : parent(parent) {}

            void operator()() { parent->check_checksums(); }

        private:
            array_pointer parent; //!< Internal reference to owning parent checksummed array instance.
        };

        /*! Compute and store CRC checksums.
        */
        void update_checksums() {
#ifdef BOOST_SELF_HEALING_DEBUG
            std::cout << "boost::self_healing::array<T, Size>::update_checksums()" << std::endl;
#endif
            boost::crc_32_type crc;
            crc.process_bytes(&elements, Size * sizeof(value_type));
            crc1 = crc.checksum();
            crc2 = crc1;
        }

        /*! A private functor that calls update_checksums() for a given checksummed array instance if called itself.
        */
        class update_checksums_functor
        {
        public:
            /*! Constructor.
            * \param parent The checksummed array instance for wich to call update_checksums().
            */
            explicit update_checksums_functor(array_pointer parent)
                : parent(parent) {}

            void operator()() { parent->update_checksums(); }

        private:
            array_pointer parent; //!< Internal reference to owning parent checksummed array instance.
        };

        checksum_type crc1;         //!< Internal first checksum.
        value_type elements[Size];  //!< Internal fixed-size checksummed array of elements of type T.
        checksum_type crc2;         //!< Internal second checksum (backup).
    };

#if !defined(BOOST_NO_TEMPLATE_PARTIAL_SPECIALIZATION)
    /*! Partial template specialization for the corner case of a zero sized checksummed array.
     */
    template<class T>
    class array<T, 0>
    {
    public:
        // type definitions
        typedef T              value_type;      //!< The type of elements stored in the <code>array</code>.
        typedef T *            iterator;        //!< A (random access) iterator used to iterate through the <code>array</code>.
        typedef const T *      const_iterator;  //!< A const (random access) iterator used to iterate through the <code>array</code>.
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

        void swap(array<value_type, 0> &) {}

        // direct access to data (read-only)
        const_pointer data() const { return 0; }
        pointer data() { return 0; }

        // return checksummed array as C array (direct read/write access to data)
        pointer c_array() { return 0; }

        // assignment with type conversion
        template <typename T2>
        array<value_type, 0>& operator=(const array<T2, 0> &) { return *this; }

        // assign one value to all elements
        void assign(const_reference value) { fill(value); }
        void fill(const_reference) {}

        // check range (may be private because it is static)
        static reference failed_rangecheck() {
            std::out_of_range e("attempt to access element of an empty checksummed array");
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
    template<class T, std::size_t Size>
    inline bool operator==(const array<T, Size> &x, const array<T, Size> &y) {
        return std::equal(x.begin(), x.end(), y.begin());
    }
    template<class T, std::size_t Size>
    inline bool operator<(const array<T, Size> &x, const array<T, Size> &y) {
        return std::lexicographical_compare(x.begin(), x.end(), y.begin(), y.end());
    }
    template<class T, std::size_t Size>
    inline bool operator!=(const array<T, Size> &x, const array<T, Size> &y) {
        return !(x == y);
    }
    template<class T, std::size_t Size>
    inline bool operator>(const array<T, Size> &x, const array<T, Size> &y) {
        return y < x;
    }
    template<class T, std::size_t Size>
    inline bool operator<=(const array<T, Size> &x, const array<T, Size> &y) {
        return !(y < x);
    }
    template<class T, std::size_t Size>
    inline bool operator>=(const array<T, Size> &x, const array<T, Size> &y) {
        return !(x < y);
    }

    /*! Global swap().
    */
    template<class T, std::size_t Size>
    inline void swap(array<T, Size> &x, array<T, Size> &y) {
        x.swap(y);
    }

} } // namespace boost::self_healing


#endif // BOOST_SELF_HEALING_ARRAY_HPP
