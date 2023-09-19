#pragma once
#include <vector>

/*
 * The LoopQueue is not thread safe and not check the boundary.
 * You should better peak it when full.
 */

template<typename T>
class LoopQueue {
public:
    explicit LoopQueue(size_t capacity);

    void put(const T &value);

    T peakFrontI(size_t idx) const;

    T peakBackI(size_t idx) const;

    bool empty() const;

    bool full() const;

    size_t size() const;

private:
    std::vector<T> data_;
    size_t capacity_;
    size_t head_;
    size_t tail_;
    bool full_;
};

template<typename T>
LoopQueue<T>::LoopQueue(size_t capacity) : data_(capacity), capacity_(capacity), head_(0), tail_(0), full_(false) {}

template<typename T>
void LoopQueue<T>::put(const T &value) {
    data_[tail_] = value;
    if (full_) {
        head_ = (head_ + 1) % capacity_;
    }
    tail_ = (tail_ + 1) % capacity_;
    full_ = (head_ == tail_);
}

template<typename T>
T LoopQueue<T>::peakFrontI(size_t idx) const {
    return data_[(head_ + idx) % capacity_];
}

template<typename T>
T LoopQueue<T>::peakBackI(size_t idx) const {
    return data_[(tail_ - 1 - idx + capacity_) % capacity_];
}

template<typename T>
bool LoopQueue<T>::empty() const {
    return (!full_ && (head_ == tail_));
}

template<typename T>
bool LoopQueue<T>::full() const {
    return full_;
}

template<typename T>
size_t LoopQueue<T>::size() const {
    if (full_) return capacity_;
    else if (tail_ >= head_) return tail_ - head_;
    else return capacity_ + tail_ - head_;
}
