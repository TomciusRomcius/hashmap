#include "forwardList.h"

#include <string>
#include <utility>

namespace Ip2 {

template <typename T>
class ForwardList<T>::Impl final {
public:
  struct Node {
    T value;
    Node* next;

    explicit Node(const T& valueIn) : value(valueIn), next(nullptr) {}
    explicit Node(T&& valueIn) : value(std::move(valueIn)), next(nullptr) {}
  };

  Impl() : head(nullptr) {}

  Impl(const Impl& other) : head(nullptr) {
    const Node* current = other.head;
    Node** tailNext = &head;
    while (current != nullptr) {
      *tailNext = new Node(current->value);
      tailNext = &((*tailNext)->next);
      current = current->next;
    }
  }

  Impl(Impl&& other) noexcept : head(other.head) {
    other.head = nullptr;
  }

  Impl& operator=(const Impl& other) {
    if (this == &other) {
      return *this;
    }
    Impl tmp(other);
    swap(tmp);
    return *this;
  }

  Impl& operator=(Impl&& other) noexcept {
    if (this == &other) {
      return *this;
    }
    clear();
    head = other.head;
    other.head = nullptr;
    return *this;
  }

  ~Impl() { clear(); }

  void push_front(const T& value) {
    Node* node = new Node(value);
    node->next = head;
    head = node;
  }

  void push_front(T&& value) {
    Node* node = new Node(std::move(value));
    node->next = head;
    head = node;
  }

  void clear() {
    Node* current = head;
    while (current != nullptr) {
      Node* next = current->next;
      delete current;
      current = next;
    }
    head = nullptr;
  }

  void swap(Impl& other) noexcept {
    Node* tmp = head;
    head = other.head;
    other.head = tmp;
  }

  Node* head;
};

template <typename T>
ForwardList<T>::ForwardList() : impl(std::make_unique<Impl>()) {}

template <typename T>
ForwardList<T>::ForwardList(const ForwardList& other)
    : impl(std::make_unique<Impl>(*other.impl)) {}

template <typename T>
ForwardList<T>::ForwardList(ForwardList&& other) noexcept = default;

template <typename T>
ForwardList<T>& ForwardList<T>::operator=(const ForwardList& other) {
  if (this == &other) {
    return *this;
  }
  impl = std::make_unique<Impl>(*other.impl);
  return *this;
}

template <typename T>
ForwardList<T>& ForwardList<T>::operator=(ForwardList&& other) noexcept = default;

template <typename T>
ForwardList<T>::~ForwardList() = default;

template <typename T>
void ForwardList<T>::push_front(const T& value) {
  impl->push_front(value);
}

template <typename T>
void ForwardList<T>::push_front(T&& value) {
  impl->push_front(std::move(value));
}

template <typename T>
void ForwardList<T>::clear() {
  impl->clear();
}

template <typename T>
T* ForwardList<T>::find_if(const std::function<bool(const T&)>& predicate) {
  typename Impl::Node* current = impl->head;
  while (current != nullptr) {
    if (predicate(current->value)) {
      return &current->value;
    }
    current = current->next;
  }
  return nullptr;
}

template <typename T>
const T* ForwardList<T>::find_if(const std::function<bool(const T&)>& predicate) const {
  const typename Impl::Node* current = impl->head;
  while (current != nullptr) {
    if (predicate(current->value)) {
      return &current->value;
    }
    current = current->next;
  }
  return nullptr;
}

template <typename T>
bool ForwardList<T>::erase_first_if(const std::function<bool(const T&)>& predicate) {
  typename Impl::Node* prev = nullptr;
  typename Impl::Node* current = impl->head;

  while (current != nullptr) {
    if (predicate(current->value)) {
      if (prev == nullptr) {
        impl->head = current->next;
      } else {
        prev->next = current->next;
      }
      delete current;
      return true;
    }
    prev = current;
    current = current->next;
  }

  return false;
}

template <typename T>
void ForwardList<T>::for_each(const std::function<void(const T&)>& visitor) const {
  const typename Impl::Node* current = impl->head;
  while (current != nullptr) {
    visitor(current->value);
    current = current->next;
  }
}

template class ForwardList<std::pair<std::string, int>>;

} // namespace Ip2
