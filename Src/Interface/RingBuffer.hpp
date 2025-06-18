#pragma once

#include <cstdint>
#include <string>

#include "stm32f7xx.h"

template<typename T, size_t Size>
class RingBuffer {
    static_assert((Size & (Size - 1)) == 0, "Size must be power of 2");

    volatile uint16_t m_head;
    volatile uint16_t m_tail;
    static constexpr uint16_t mask = Size - 1;
    T m_buffer[Size];
public:
    RingBuffer() : m_head(0), m_tail(0) {}

    inline bool push(const T &value) {
        uint16_t next = (m_head + 1) & mask;
        if (next == m_tail) return false; ///< Буфер полон
        m_buffer[m_head] = value;
        m_head = next;
        return true;
    }

    inline bool pop(T &value) {
        if (isEmpty()) return false;
        value = m_buffer[m_tail];
        m_tail = (m_tail + 1) & mask;
        return true;
    }

    inline bool peek(T &value) const {
        if (isEmpty()) return false;
        value = m_buffer[m_tail];
        return true;
    }

    inline bool isEmpty() const {
        return m_head == m_tail;
    }

    inline bool isFull() const {
        return ((m_head + 1) & mask) == m_tail;
    }

    inline uint16_t available() const {
        return (m_head - m_tail + Size) & mask;
    }

    inline void reset() {
        m_head = m_tail = 0;
    }
};
