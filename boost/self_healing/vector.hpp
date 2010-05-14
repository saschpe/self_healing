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

#include <stdexcept>
#include <vector>
#include <boost/swap.hpp>

// Handles broken standard libraries better than <iterator>
#include <boost/detail/iterator.hpp>
#include <boost/throw_exception.hpp>

// FIXES for broken compilers
#include <boost/config.hpp>

#include "./detail/chunk.hpp"
#include "./detail/reference_wrapper.hpp"


/// The namespace self_healing contains fault-tolerant data structures and utility classes.
namespace boost { namespace self_healing {

    /*! \brief Vector.
    *
    * TODO.
    *
    * \param T The data type of the stored values.
    * \param CS The size of the internal chunks.
    * \remarks The chunk size should be chosen based on CPU cache size.
    * \see chunk
    */
    template <class T, std::size_t CS = 64>
    class vector
    {
    public:
        // type definitions
        typedef T                    value_type;        //!< The type of elements stored in the <code>vector</code>.
        typedef A                    allocator_type;    //!< The type of an allocator used in the <code>vector</code>.
        class                        iterator;          //!< Forward declaration of class iterator.
        class                        const_iterator;    //!< Forward declaration of class const_iterator.
        typedef reference_wrapper<T> reference;         //!< A reference to an element.
        typedef const T &            const_reference;   //!< A const reference to an element.

        /*! An unsigned integral type that can represent any non-negative value of the container's distance type.
        */
        typedef std::size_t          size_type;

        /*! A signed integral type used to represent the distance between two iterators.
        */
        typedef std::ptrdiff_t       difference_type;

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
        typedef typename A::reverse_iterator<iterator> reverse_iterator;
        typedef typename A::reverse_iterator<const_iterator> const_reverse_iterator;
#elif defined(_MSC_VER) && (_MSC_VER == 1300) && defined(BOOST_DINKUMWARE_STDLIB) && (BOOST_DINKUMWARE_STDLIB == 310)
        // workaround for broken reverse_iterator in VC7
        typedef typename A::reverse_iterator<std::_Ptrit<value_type, difference_type, iterator,
                                                         reference, iterator, reference> > reverse_iterator;
        typedef typename A::reverse_iterator<std::_Ptrit<value_type, difference_type, const_iterator,
                                                         const_reference, iterator, reference> > const_reverse_iterator;
#elif defined(_RWSTD_NO_CLASS_PARTIAL_SPEC)
        typedef typename A::reverse_iterator<iterator, std::random_access_iterator_tag,
                                             value_type, reference, iterator, difference_type> reverse_iterator;
        typedef typename A::reverse_iterator<const_iterator, std::random_access_iterator_tag,
                                             value_type, const_reference, const_iterator, difference_type> const_reverse_iterator;
#else
        // workaround for broken reverse_iterator implementations
        typedef typename A::reverse_iterator<iterator, T> reverse_iterator;
        typedef typename A::reverse_iterator<const_iterator, T> const_reverse_iterator;
#endif


        // constructors
        explicit vector(const allocator_type &alloc = A())
            : m_alloc(alloc) {}

        vector(size_type n, const_reference x = value_type(), const allocator_type &alloc = A())
            : m_alloc(alloc) {
            assign(n, x);
        }

        template <class InputIterator>
        vector(InputIterator first, InputIterator last, const allocator_type &alloc = A())
            : m_alloc(alloc) {
            assign(first, last);
        }

        /*! Copy constructor.
        * \param other The other <code>boost::self_healing::vector</code> to copy from.
        */
        vector(const vector<value_type, CS, allocator_type> &rhs)
            : m_alloc(rhs.get_allocator()) {
            assign(rhs.begin(), rhs.end());
        }
        /*! Copy constructor.
        * \param other The other <code>std::vector</code> to copy from.
        */
        vector(const std::vector<value_type> &rhs);
            : m_alloc(rhs.get_allocator()) {
            assign(rhs.begin(), rhs.end());
        }

        ~vector() {
            //m_alloc.deallocate(m_alloc.head, TODO);
        }

        vector<value_type, CS, allocator_type>& operator=(const vector<value_type, CS, allocator_type> &);

        template <class InputIterator>
        void assign(InputIterator first, InputIterator last);
        template <class Size, class TT>
        void assign(Size n);
        template <class Size, class TT>
        void assign(Size n, const TT&);

        allocator_type get_allocator() const { return m_alloc; }

        // iterator support
        //iterator begin() { return iterator(m_chunks); }
        //const_iterator begin() const { return const_iterator(m_head, m_head->elements.begin()); }
        //iterator end() { return iterator(m_chunks + N); }
        //const_iterator end() const { return const_iterator(m_tail, m_tail->elements.end()); }

        // reverse iterator support
        //reverse_iterator rbegin() { return reverse_iterator(end()); }
        //const_reverse_iterator rbegin() const { return const_reverse_iterator(end()); }
        //reverse_iterator rend() { return reverse_iterator(begin()); }
        //const_reverse_iterator rend() const { return const_reverse_iterator(begin()); }

        // capacity
        size_type size() const { check_size(); return m_alloc.size; }
        bool empty() const { return size() == 0; }

        /*! \brief Get the largest possible size or capacity of the <code>vector</code>. (It depends on
        *          allocator's %max_size()).
        * \return The maximum size/capacity the <code>vector</code> can be set to.
        * \throws Nothing.
        * \par Exception Safety
        *      No-throw.
        * \par Iterator Invalidation
        *      Does not invalidate any iterators.
        * \par Complexity
        *      Constant (in the size of the <code>vector</code>).
        * \sa <code>size()</code>, <code>capacity()</code>, <code>reserve()</code>
        */
        size_type max_size() const {
            //TODO: return something reasonable
            return (std::min<size_type>)(/*m_alloc.max_size()*/999999999, (std::numeric_limits<difference_type>::max)());
        }

        size_type capacity() const { check_chunks(); return m_chunks * CS; }

        void resize(size_type new_size, value_type fill = T()) {
            check_size();
            if (new_size > size()) {
                //const int old_size = m_size;
                reserve(new_size);
                // TODO: fill new elelemts with provide value
                /*for (int i = old_size; i < size; i++) {
                }*/
            } else if (new_size < size()) {

            }
            // vector throws a length_error if resized above max_size
            /*void resize(size_type new_size, param_value_type item = value_type()) {
                if (new_size > size()) {
                    if (new_size > capacity())
                        set_capacity(new_size);
                    insert(end(), new_size - size(), item);
                } else {
                    iterator e = end();
                    erase(e - (size() - new_size), e);
                }
            }*/
        }
        void reserve(size_type new_size) {
            check_size();
            if (new_size > size()) {
                //TODO: Do something actually
            }
            // vector throws a length_error if resized above max_size
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
        /*void push_back(const_reference value);
        void pop_back();*/
        /*iterator insert(iterator it);
        iterator insert(iterator it, const_reference value);
        void insert(iterator it, size_type, const_reference value);
        template <class InputIterator>
        void insert(iterator, InputIterator, InputIterator);
        iterator erase(iterator);
        iterator erase(iterator, iterator);*/
        //void swap(vector<value_type> &);

        /*! Check index validity against size.
        * \param index The index to check.
        * \throws std::out_of_range Thrown if index is out of range.
        */
        void rangecheck(size_type index) const {
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
            try {
                check_chunks();
                check_size();
                return true;
            } catch (const std::runtime_error &e) {
                return false;
            };
        }

    private:
        void check_chunks() const {
            //TODO: Check and repair chunk count
            std::runtime_error e("chunk count error");
            boost::throw_exception(e);
        }

        void check_size() const {
            //TODO: check and repair size
            std::runtime_error e("size error");
            boost::throw_exception(e);
        }

        void check_allocator() const {
            //TODO: check and repair allocator pointer
            std::runtime_error e("allocator error");
            boost::throw_exception(e);
        }

        /*chunk<T, CS> *next_chunk(const chunk<T, CS> *chunk) const {
            if (this >= parent->m_tail) {
                std::out_of_range e("vector<>: index out of range");
                boost::throw_exception(e);
            }
            //return const_cast<chunk *>(this + sizeof(chunk));
        }
        chunk<T, CS> *previous_chunk(const chunk<T, CS> *chunk) const {
            if (this <= parent->m_head) {
                std::out_of_range e("vector<>: index out of range");
                boost::throw_exception(e);
            }
            //return const_cast<chunk *>(this - sizeof(chunk));
        }
        bool is_head(const chunk<T, CS> *chunk) const { return this == parent->m_head; }
        bool is_tail(const chunk<T, CS> *chunk) const { return this == parent->m_tail; }*/

        //TODO: make this one fault-tolerant
        struct vector_allocator : public allocator_type
        {
            vector(const allocator_type &alloc)
                : allocator_type(alloc), head(NULL), tail(NULL), m_chunks(0);

            chunk<value_type, vector<value_type, CS, allocator_type>, CS> *head;  //!< Pointer to the first chunk.
            chunk<value_type, vector<value_type, CS, allocator_type>, CS> *tail;  //!< Pointer to the last chunk.
            size_type chunks;                   //!< Chunk counter.
            size_type size;                     //!< Counts how much elements are stored currently in all chunks.
            size_type capacity;
        };

        vector_allocator m_alloc;
    };

#if !defined(BOOST_NO_TEMPLATE_PARTIAL_SPECIALIZATION)
    /*! Partial template specialization for the corner case of a vector with zero sized chunks.
     */
    template <class T>
    class vector<T, 0>
    {
        // type definitions
        typedef T              value_type;      //!< The type of elements stored in the <code>checksummed_array</code>.
        typedef T *            iterator;        //!< A (random access) iterator used to iterate through the <code>checksummed_array</code>.
        typedef const T *      const_iterator;  //!< A const (random access) iterator used to iterate through the <code>checksummed_array</code>.
        typedef T *            pointer;         //!< A pointer to the element.
        typedef const T *      const_pointer;   //!< A const pointer to the element.
        typedef T &            reference;       //!< A reference to an element.
        typedef const T &      const_reference; //!< A const reference to an element.

        /// An unsigned integral type that can represent any non-negative value of the container's distance type.
        typedef std::size_t    size_type;

        /// A signed integral type used to represent the distance between two iterators.
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


        // constructors
        vector(size_type, const_reference) {}
        vector(const vector<value_type, 0> &) {}
        vector(const std::vector<value_type>) {}
        explicit vector(size_type) {}
        template <class InputIterator>
        vector(InputIterator, InputIterator) {}

        // assignment
        vector<value_type, 0>& operator=(const vector<value_type, 0> &) { return *this; }

        // assign one value to all elements
        template <class InputIterator>
        void assign(InputIterator, InputIterator) {}
        template <class Size, class TT>
        void assign(Size) {}
        template <class Size, class TT>
        void assign(Size, const TT &) {}

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

        // size is constant
        static size_type size() { return 0; }
        static bool empty() { return true; }
        static size_type max_size() { return 0; }
        static size_type capacity() { return 0; }
        enum { static_size = 0 };

        void resize(size_type, value_type = T()) {
            std::length_error e("attempt to resize a zero sized vector");
            boost::throw_exception(e);
        }
        void reserve(size_type) {
            std::length_error e("attempt to resize a zero sized vector");
            boost::throw_exception(e);
        }

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

        // modifiers
        void push_back(const_reference value) {}
        void pop_back() {}
        iterator insert(iterator) { return iterator(reinterpret_cast<pointer>(this)); }
        iterator insert(iterator, const_reference) { return iterator(reinterpret_cast<pointer>(this)); }
        void insert(iterator, size_type, const_reference) {}
        template <class InputIterator>
        void insert(iterator, InputIterator, InputIterator) {}
        iterator erase(iterator) { return iterator(reinterpret_cast<pointer>(this)); }
        iterator erase(iterator, iterator) { return iterator(reinterpret_cast<pointer>(this)); }

        void swap(vector<value_type, 0> &) {}

        // check range (may be private because it is static)
        static reference failed_rangecheck() {
            std::out_of_range e("attempt to access element of an empty vector");
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

} } // namespace boost::self_healing


/*! Overload for operator<<() of std::ostream to print a vector.
*/
template <class T, std::size_t CS, class A>
std::ostream &operator<<(std::ostream &os, const boost::self_healing::vector<T, CS, A> &vector)
{
    os << "[";
    for (std::size_t i = 0; i < vector.size(); i++) {
        os << (i == 0 ? "" : ",") << vector[i];
    }
    return os << "]";
}


#endif // BOOST_SELF_HEALING_VECTOR_HPP
