/*
 * ARM Cortex-M4F Synthesizer
 *
 * Copyright (c) 2017 Terence M. Darwen - tmdarwen.com
 *
 * The MIT License
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
 * THE SOFTWARE.
 */

#pragma once

#include <stdint.h>

// This is a very typical FIFO container template I made from scratch b/c I needed it...
// Not sure what else to say about it, it should be fairly self-explanatory.
template <typename T, uint32_t size>
class FIFO
{
public:
    FIFO();
    void Push(T item);
    T Pop();
    T Peek();

    uint32_t GetCount();
    uint32_t GetMaxSize();

private:
    T data_[size];
    uint32_t count_;
    uint32_t headIndex_;
    uint32_t tailIndex_;
};

template <typename T, uint32_t size>
FIFO<T, size>::FIFO() : count_(0), headIndex_(0), tailIndex_(0) { }

template <typename T, uint32_t size>
void FIFO<T, size>::Push(T item)
{
    if (count_ == size)
    {
        // Queue is full, nothing to do
        return;
    }

    data_[tailIndex_] = item;
    ++tailIndex_;
    if (tailIndex_ == size)
    {
        tailIndex_ = 0;
    }

    ++count_;
}

template <typename T, uint32_t size>
T FIFO<T, size>::Pop()
{
    if (size == 0)
    {
        // Noting to pop
        return T();
    }

    T returnValue(data_[headIndex_]);
    ++headIndex_;
    if (headIndex_ == size)
    {
        headIndex_ = 0;
    }

    --count_;

    return returnValue;
}


template <typename T, uint32_t size>
T FIFO<T, size>::Peek()
{
    if (size == 0)
    {
        // Noting in the queue to peek at
        return T();
    }

    return data_[headIndex_];
}

template <typename T, uint32_t size>
uint32_t FIFO<T, size>::GetCount()
{
    return count_;
}

template <typename T, uint32_t size>
uint32_t FIFO<T, size>::GetMaxSize()
{
    return size;
}
