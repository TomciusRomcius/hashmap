#pragma once

#include <cstddef>
#include <functional>
#include <memory>
#include <utility>

namespace Ip2 {

template <typename T>
class ForwardList final {
public:
  ForwardList();
  ForwardList(const ForwardList& other);
  ForwardList(ForwardList&& other) noexcept;
  ForwardList& operator=(const ForwardList& other);
  ForwardList& operator=(ForwardList&& other) noexcept;
  ~ForwardList();

  void push_front(const T& value);
  void push_front(T&& value);
  void clear();

  T* find_if(const std::function<bool(const T&)>& predicate);
  const T* find_if(const std::function<bool(const T&)>& predicate) const;
  bool erase_first_if(const std::function<bool(const T&)>& predicate);
  void for_each(const std::function<void(const T&)>& visitor) const;

private:
  class Impl;
  std::unique_ptr<Impl> impl;
};

} // namespace Ip2
