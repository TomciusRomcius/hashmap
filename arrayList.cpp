#include "arrayList.h"

#include <stdexcept>
#include <string>
#include <utility>

#include "forwardList.h"

namespace Ip2 {

template <typename T>
class ArrayList<T>::Impl final {
public:
  Impl() : data(nullptr), length(0) {}

  explicit Impl(std::size_t size) : data(nullptr), length(0) {
    if (size == 0) {
      return;
    }
    data = new T[size]();
    length = size;
  }

  Impl(const Impl& other) : data(nullptr), length(0) {
    if (other.length == 0) {
      return;
    }
    data = new T[other.length];
    length = other.length;
    for (std::size_t i = 0; i < length; ++i) {
      data[i] = other.data[i];
    }
  }

  Impl(Impl&& other) noexcept : data(other.data), length(other.length) {
    other.data = nullptr;
    other.length = 0;
  }

  Impl& operator=(const Impl& other) {
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

  Impl& operator=(Impl&& other) noexcept {
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

  ~Impl() { delete[] data; }

  T* data;
  std::size_t length;
};

template <typename T>
ArrayList<T>::ArrayList() : impl(std::make_unique<Impl>()) {}

template <typename T>
ArrayList<T>::ArrayList(std::size_t size) : impl(std::make_unique<Impl>(size)) {}

template <typename T>
ArrayList<T>::ArrayList(const ArrayList& other)
    : impl(std::make_unique<Impl>(*other.impl)) {}

template <typename T>
ArrayList<T>::ArrayList(ArrayList&& other) noexcept = default;

template <typename T>
ArrayList<T>& ArrayList<T>::operator=(const ArrayList& other) {
  if (this == &other) {
    return *this;
  }
  impl = std::make_unique<Impl>(*other.impl);
  return *this;
}

template <typename T>
ArrayList<T>& ArrayList<T>::operator=(ArrayList&& other) noexcept = default;

template <typename T>
ArrayList<T>::~ArrayList() = default;

template <typename T>
T& ArrayList<T>::operator[](std::size_t index) {
  if (index >= impl->length) {
    throw std::out_of_range("ArrayList: index out of range");
  }
  return impl->data[index];
}

template <typename T>
const T& ArrayList<T>::operator[](std::size_t index) const {
  if (index >= impl->length) {
    throw std::out_of_range("ArrayList: index out of range");
  }
  return impl->data[index];
}

template <typename T>
std::size_t ArrayList<T>::size() const {
  return impl->length;
}

template <typename T>
bool ArrayList<T>::empty() const {
  return impl->length == 0;
}

template <typename T>
T* ArrayList<T>::begin() {
  return impl->data;
}

template <typename T>
T* ArrayList<T>::end() {
  return impl->data + impl->length;
}

template <typename T>
const T* ArrayList<T>::begin() const {
  return impl->data;
}

template <typename T>
const T* ArrayList<T>::end() const {
  return impl->data + impl->length;
}

template <typename T>
const T* ArrayList<T>::cbegin() const {
  return impl->data;
}

template <typename T>
const T* ArrayList<T>::cend() const {
  return impl->data + impl->length;
}

template class ArrayList<ForwardList<std::pair<std::string, int>>>;

} // namespace Ip2
