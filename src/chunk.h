
// Copyright Sascha Peilicke <sasch.pe@gmx.de> 2010.
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
//  http://www.boost.org/LICENSE_1_0.txt)

#ifndef ROBUST_CHUNK_H
#define ROBUST_CHUNK_H

#include <boost/crc.hpp>

#include <ostream>

namespace robust {

template <class T, std::size_t N = 64>
class chunk
{
public:
    chunk(const T &value = 0, void *head = NULL, void *tail = NULL)
        : m_head(head), m_tail(tail)
    {
        for (std::size_t i = 0; i < N; i++) {
            m_payload[i] = value;
        }

        // Compute and store initial CRC checksums
        boost::crc_32_type crc;
        crc.process_bytes(&m_payload, N * sizeof(T));
        m_crc1 = crc.checksum();
        m_crc2 = crc.checksum();
    }

    bool is_valid() {
        bool valid = true;
        valid &= check_head_pointer();
        valid &= check_tail_pointer();
        valid &= check_checksums();
        return valid;
    }

    T at(std::size_t index) const {
        //TODO: Range check
        return m_payload[index];
    }

    std::size_t size() const {
        return N;
    }

private:
    bool check_head_pointer() {
        bool success = true;
        // Check if the circular reference on head is ok
        if (m_head != NULL) {
            //**m_head == &m_head;
            crc
        }

    }

    bool check_tail_pointer() {
        bool success = true;
        // Check if the circular reference on tail is ok
        if (m_tail != NULL) {
            //success &= **m_tail == &m_tail;
        }
        return success;
    }

    unsigned int check_checksums() {
        //TODO: check crcs
        return m_crc1;
    }

    void *m_head;
    unsigned int m_crc1;            // First CRC-32 checksum
    T m_payload[N];                 //
    unsigned int m_crc2;            // Second CRC-32 checksum
    void *m_tail;
};

} // namespace robust

template <class T, std::size_t N>
std::ostream &operator<<(std::ostream &os, const robust::chunk<T, N> &chunk)
{
    os << "[";
    for (std::size_t i = 0; i < chunk.size(); i++) {
        os << (i == 0 ? "" : ",") << chunk.at(i);
    }
    os << "]";
    return os;
}

#endif // ROBUST_CHUNK_H
