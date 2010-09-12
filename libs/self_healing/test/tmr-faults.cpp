/* Test case for TMR data structures.
 *
 * (C) Copyright Sascha Peilicke 2010.
 *
 * Distributed under the Boost Software License, Version 1.0. (See
 * accompanying file LICENSE_1_0.txt or copy at
 * http://www.boost.org/LICENSE_1_0.txt)
 *
 * 11 September 2010 - Initial Revision (Sascha Peilicke)
 */

#include "utility.hpp"

#include <boost/config.hpp>
#include <boost/detail/workaround.hpp>
#include <boost/throw_exception.hpp>

#include <iostream>

using namespace boost;
using namespace std;

/*! Simple implementation of a TMR array.
*/
template <class T, std::size_t Size = 64>
class tmr_array
{
public:
    explicit tmr_array(const T &value = 0) {
        for (std::size_t i = 0; i < Size; i++) {
            elements[0][i] = value;
            elements[1][i] = value;
            elements[2][i] = value;
        }
    }

    T& operator[](std::size_t i) { check_triple(i); return elements[0][i]; }
    const T& operator[](std::size_t i) const { check_triple(i); return elements[0][i]; }

    /*! Compares validity against a reference TMR array.
    */
    bool is_valid(const tmr_array<T, Size> * const other = 0) const {
        try {
            if (other) {
                // in-depth comparison possible
                for (std::size_t i = 0; i < Size; i++) {
                    if (operator[](i) != (*other)[i]) {
                        return false;
                    }
                }
            } else {
                check_contents();
            }
            return true;
        } catch (std::runtime_error &) {
            return false;
        }
    }

private:
    void check_triple(std::size_t index) const {
        const bool equal_12 = elements[0][index] == elements[1][index];
        const bool equal_13 = elements[0][index] == elements[2][index];
        const bool equal_23 = elements[1][index] == elements[2][index];

        if (equal_12 && equal_13) {
            return; // all fine
        } else if (equal_12) {
            const_cast<T &>(elements[2][index]) = elements[0][index];
        } else if (equal_13) {
            const_cast<T &>(elements[1][index]) = elements[0][index];
        } else if (equal_23) {
            const_cast<T &>(elements[0][index]) = elements[1][index];
        } else {
            // All three values differ
            std::runtime_error e("tmr triple error");
            boost::throw_exception(e);
        }
    }

    void check_contents() const {
        for (std::size_t i = 0; i < Size; i++) {
            check_triple(i);
        }
    }

    T elements[3][Size];
};

int main()
{
    cout << "testing TMR fault injection" << endl;

    cout << "bit flip tests" << endl;
    /*for (size_t bit_flips = 1; bit_flips <= 4; bit_flips++) {
        const tmr_array<int, 100> ref_tmp(23);
        size_t still_valid = 0;
        for (size_t i = 0; i < 10000; i++) {
            tmr_array<int, 100> tmp(23);
            utility::flip_bits(&tmp, sizeof(tmp), bit_flips);
            if (tmp.is_valid(&ref_tmp)) { still_valid++; }
        }
        cout << "tmr_array<int,100>: " << still_valid << " out of 10000 are still valid for "
             << bit_flips << " bit flip(s)." << endl;
    }*/
    for (size_t bit_flips = 1; bit_flips <= 4; bit_flips++) {
        const tmr_array<int, 1000> ref_tmp(23);
        size_t still_valid = 0;
        for (size_t i = 0; i < 10000; i++) {
            tmr_array<int, 1000> tmp(23);
            utility::flip_bits(&tmp, sizeof(tmp), bit_flips);
            if (tmp.is_valid(&ref_tmp)) { still_valid++; }
        }
        cout << "tmr_array<int,1000>: " << still_valid << " out of 10000 are still valid for "
             << bit_flips << " bit flip(s)." << endl;
    }

    cout << "burst tests" << endl;
    /*for (size_t burst_length = 4; burst_length <= 32; burst_length *= 2) {
        const tmr_array<int, 100> ref_tmp(23);
        size_t still_valid = 0;
        for (size_t i = 0; i < 10000; i++) {
            tmr_array<int, 100> tmp(23);
            utility::burst_flip_bits(&tmp, sizeof(tmp), burst_length);
            if (tmp.is_valid(&ref_tmp)) { still_valid++; }
        }
        cout << "tmr_array<int,100>: " << still_valid << " out of 10000 are still valid for burst of length "
             << burst_length << endl;
    }*/
    /*for (size_t burst_length = 4; burst_length <= 32; burst_length *= 2) {
        const tmr_array<int, 1000> ref_tmp(23);
        size_t still_valid = 0;
        for (size_t i = 0; i < 10000; i++) {
            tmr_array<int, 1000> tmp(23);
            utility::burst_flip_bits(&tmp, sizeof(tmp), 32);
            if (tmp.is_valid(&ref_tmp)) { still_valid++; }
        }
        cout << "tmr_array<int,1000>: " << still_valid << " out of 10000 are still valid for burst of length "
             << burst_length << endl;
    }*/
    return 0;
}
