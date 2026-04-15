#pragma once

#include <cstddef>
#include <stdexcept>
#include <utility>

namespace Ip2 {

template <typename T>
class ArrayList final {
public:
  ArrayList() : data(nullptr), length(0) {}

  explicit ArrayList(std::size_t size) : data(nullptr), length(0) {
    if (size == 0) {
      return;
    }
    data = new T[size]();
    length = size;
  }

  ArrayList(const ArrayList& other) : data(nullptr), length(0) {
    if (other.length == 0) {
      return;
    }
    data = new T[other.length];
    length = other.length;
    for (std::size_t i = 0; i < length; ++i) {
      data[i] = other.data[i];
    }
  }

  ArrayList(ArrayList&& other) noexcept : data(other.data), length(other.length) {
    other.data = nullptr;
    other.length = 0;
  }

  ArrayList& operator=(const ArrayList& other) {
    if (this == &other) {
      return *this;
    }

    T* newData = nullptr;
    if (other.length > 0) {
      newData = new T[other.length];
      for (std::size_t i = 0; i < other.length; ++i) {
        newData[i] = other.data[i];
      }
    }

    delete[] data;
    data = newData;
    length = other.length;
    return *this;
  }

  ArrayList& operator=(ArrayList&& other) noexcept {
    if (this == &other) {
      return *this;
    }

    delete[] data;
    data = other.data;
    length = other.length;
    other.data = nullptr;
    other.length = 0;
    return *this;
  }

  ~ArrayList() { delete[] data; }

  T& operator[](std::size_t index) {
    if (index >= length) {
      throw std::out_of_range("ArrayList: index out of range");
    }
    return data[index];
  }

  const T& operator[](std::size_t index) const {
    if (index >= length) {
      throw std::out_of_range("ArrayList: index out of range");
    }
    return data[index];
  }

  std::size_t size() const { return length; }
  bool empty() const { return length == 0; }

  T* begin() { return data; }
  T* end() { return data + length; }
  const T* begin() const { return data; }
  const T* end() const { return data + length; }
  const T* cbegin() const { return data; }
  const T* cend() const { return data + length; }

private:
  T* data;
  std::size_t length;
};

} // namespace Ip2
