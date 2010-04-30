
// Copyright Sascha Peilicke <sasch.pe@gmx.de> 2010.
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
//  http://www.boost.org/LICENSE_1_0.txt)

#ifndef ROBUST_VECTOR_H
#define ROBUST_VECTOR_H

#include "chunk.h"

#include <algobase.h>
#include <allocator.h>
#include <error.h>
#include <iterator.h>
#include <select.h>
#include <defs.h>

namespace robust {

template <class T, class Allocator = allocator<T> >
class vector
{
public:

    /*explicit vector(const Allocator& = Allocator());
    explicit vector(size_type, const Allocator& = Allocator());
    vector(size_type, const T&, const Allocator& = Allocator());
    vector(const vector<T, Allocator>&);*/

    explicit vector(size_type);
    vector(const vector<T>);

    /*template <class InputIterator>
    vector(InputIterator, InputIterator, const Allocator& = Allocator());*/
    ~vector();

    //vector<T, Allocator>& operator=(const vector<T, Allocator>&);


    /*template <class InputIterator>
    void assign(InputIterator first, InputIterator last);
    template <class Size, class TT>
    void assign(Size n);
    template <class Size, class TT>
    void assign(Size n, const TT&);
    allocator_type get_allocator() const;*/

    // Iterators
    iterator begin();
    const_iterator begin() const;
    iterator end();
    const_iterator end() const;
    reverse_iterator rbegin();
    const_reverse_iterator rbegin() const;
    reverse_iterator rend();
    const_reverse_iterator rend() const;

    // Capacity
    size_type size() const;
    size_type max_size() const;
    void resize(size_type);
    void resize(size_type, T);
    size_type capacity() const;
    bool empty() const;
    void reserve(size_type);

    // Element Access
    reference operator[](size_type);
    const_reference operator[](size_type) const;
    reference at(size_type);
    const_reference at(size_type) const;
    reference front();
    const_reference front() const;
    reference back();
    const_reference back() const;

    // Modifiers
    void push_back(const T &);
    void pop_back();
    /*iterator insert(iterator);
    iterator insert(iterator, const T &);
    void insert(iterator, size_type, const T &);
    template <class InputIterator>
    void insert(iterator, InputIterator, InputIterator);
    iterator erase(iterator);
    iterator erase(iterator, iterator);*/
    void swap(vector<T> &);

private:
    T const *m_head;
    T *m_tail;
    size_type m_size;
    size_type m_capacity;
};

/*// comparisons
template<class T, class Allocator>
inline bool operator== (const vector<T> &x, const vector<T> &y) {
    return std::equal(x.begin(), x.end(), y.begin());
}
template<class T, class Allocator>
inline bool operator< (const vector<T> &x, const vector<T> &y) {
    return std::lexicographical_compare(x.begin(),x.end(),y.begin(),y.end());
}
template<class T, class Allocator>
inline bool operator!= (const vector<T> &x, const vector<T> &y) {
    return !(x==y);
}
template<class T, class Allocator>
inline bool operator> (const vector<T> &x, const vector<T> &y) {
    return y<x;
}
template<class T, class Allocator>
inline bool operator<= (const vector<T> &x, const vector<T> &y) {
    return !(y<x);
}
template<class T, class Allocator>
inline bool operator>= (const vector<T>& x, const vector<T> &y) {
    return !(x<y);
}

// global swap()
template<class T, class Allocator>
inline void swap (vector<T, Allocator> &x, vector<T, Allocator> &y) {
    x.swap(y);
}*/

} // namespace robust

#endif // ROBUST_VECTOR_H
