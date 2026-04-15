#pragma once

#include <cstddef>
#include <memory>
#include <stdexcept>

namespace Ip2 {

template <typename T>
class ArrayList final {
public:
  ArrayList();
  explicit ArrayList(std::size_t size);
  ArrayList(const ArrayList& other);
  ArrayList(ArrayList&& other) noexcept;
  ArrayList& operator=(const ArrayList& other);
  ArrayList& operator=(ArrayList&& other) noexcept;
  ~ArrayList();

  T& operator[](std::size_t index);
  const T& operator[](std::size_t index) const;

  std::size_t size() const;
  bool empty() const;

  T* begin();
  T* end();
  const T* begin() const;
  const T* end() const;
  const T* cbegin() const;
  const T* cend() const;

private:
  class Impl;
  std::unique_ptr<Impl> impl;
};

} // namespace Ip2
