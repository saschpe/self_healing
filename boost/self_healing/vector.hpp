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
#include <boost/swap.hpp>

// Handles broken standard libraries better than <iterator>
#include <boost/detail/iterator.hpp>
#include <boost/throw_exception.hpp>

// FIXES for broken compilers
#include <boost/config.hpp>

#include "./detail/chunk.hpp"
#include "reference_wrapper.hpp"


/// The namespace self_healing contains fault-tolerant data structures and utility classes.
namespace boost { namespace self_healing {

    /*! Exception that is thrown when a size error happened.
    */
    class size_error : public std::runtime_error
    {
    public:
        explicit size_error(const std::string &what_arg)
            : std::runtime_error(what_arg) {}
    };

    /*! \brief Vector.
    *
    * TODO.
    *
    * \param A
    * \param CS The size of the internal chunks.
    * \param T The data type of the stored values.
    * \remarks The chunk size should be chosen based on CPU cache size.
    * \see chunk
    */
    template <class T, std::size_t CS = 64, class A = std::allocator<T> >
    class vector
    {
    public:
        // type definitions
        typedef T                    value_type;        //!< The type of elements stored in the <code>vector</code>.
        class                        iterator;          //!< Forward declaration of class iterator.
        class                        const_iterator;    //!< Forward declaration of class const_iterator.
        //typedef self_healing::pointer<T>   pointer;   //!< A pointer to the element.
        typedef const T *            const_pointer;     //!< A const pointer to the element.
        typedef reference_wrapper<T> reference;         //!< A reference to an element.
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
        typedef A                    allocator_type;    //!< The type of an allocator used in the <code>circular_buffer</code>.


        explicit vector(const allocator_type & = A())
            : m_head(new chunk<value_type, vector<value_type, CS, allocator_type>, CS>(this)), m_tail(m_head), m_chunks(1), m_size(0) {}

        /*explicit vector(size_type, const allocator_type & = A());
        vector(size_type, const_reference, const allocator_type & = A());*/

        /*! Copy constructor.
        * \param other The other vector to copy from.
        */
        //vector(const vector<value_type, CS, allocator_type> &other);
        //vector(const std::vector<value_type>);

        /*explicit vector(size_type);

        template <class InputIterator>
        vector(InputIterator, InputIterator, const allocator_type & = A());*/

        ~vector() {
            delete[] m_head;
        }

        /*vector<value_type, CS, allocator_type>& operator=(const vector<value_type, CS, allocator_type> &);

        template <class InputIterator>
        void assign(InputIterator first, InputIterator last);
        template <class Size, class TT>
        void assign(Size n);
        template <class Size, class TT>
        void assign(Size n, const TT&);
        allocator_type get_allocator() const;*/

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

        // Capacity
        size_type size() const { check_and_repair_size(); return m_size; }

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

        size_type capacity() const { check_and_repair_chunks(); return m_chunks * CS; }
        bool empty() const { return size() == 0; }

        void resize(size_type new_size, value_type fill = T()) {
            check_and_repair_size();
            if (new_size > m_size) {
                const int old_size = m_size;
                reserve(new_size);
                // TODO: fill new elelemts with provide value
                /*for (int i = old_size; i < size; i++) {
                }*/
            } else if (new_size < m_size) {

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
            check_and_repair_size();
            if (new_size > m_size) {
                //TODO: Do something actually
            }
            // vector throws a length_error if resized above max_size
        }

        // Element Access
        reference operator[](size_type i) {
            rangecheck(i);

        }
        const_reference operator[](size_type i) const {
            rangecheck(i);
        }

        reference at(size_type i) { return operator[](i); }
        const_reference at(size_type i) const { return operator[](i); }
        reference front() { return operator[](0); }
        const_reference front() const { return operator[](0); }
        reference back() { check_and_repair_size(); return operator[](m_size - 1); }
        const_reference back() const { check_and_repair_size(); return operator[](m_size - 1); }

        // Modifiers
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
        * \see check_and_repair_size
        */
        bool is_valid() const {
            try {
                check_and_repair_chunks();
                check_and_repair_size();
                //TODO: Add more checks here
                return true;
            } catch (const size_error &e) {
                return false;
            };
        }

    private:

        void check_and_repair_chunks() const {
            //TODO: Check and repair chunk count
            size_error e("chunk count error");
            boost::throw_exception(e);
        }

        void check_and_repair_size() const {
            //TODO: check and repair size
            size_error e("size error");
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
        }*/

        /*bool is_head(const chunk<T, CS> *chunk) const { return this == parent->m_head; }
        bool is_tail(const chunk<T, CS> *chunk) const { return this == parent->m_tail; }*/

        chunk<value_type, vector<value_type, CS, allocator_type>, CS> *m_head;  //!< Internal pointer to the first chunk.
        chunk<value_type, vector<value_type, CS, allocator_type>, CS> *m_tail;  //!< Internal pointer to the last chunk.

        size_type m_chunks;                         //!< Chunk counter.
        size_type m_size;                           //!< Counts how much elements are stored currently in all chunks.
    };


    //TODO: Add vector<T, 0, A> template specialization

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
