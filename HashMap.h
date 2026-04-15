#pragma once

#include <cstddef>
#include <memory>
#include <stdexcept>
#include <string>
#include <utility>

namespace Ip2 {

template <typename Key, typename Value>
class HashMap final {
public:
  using Entry = std::pair<Key, Value>;

  class DuplicateKey : public std::invalid_argument {
  public:
    explicit DuplicateKey(const std::string& what_arg);
  };

  HashMap();
  explicit HashMap(std::size_t bucketCount);
  HashMap(const HashMap& other);
  HashMap(HashMap&& other) noexcept;
  HashMap& operator=(const HashMap& other);
  HashMap& operator=(HashMap&& other) noexcept;
  ~HashMap();

  void insert(const Key& key, const Value& value);
  void erase(const Key& key);
  void update(const Key& key, const Value& value);
  bool containsKey(const Key& key) const;
  Value& at(const Key& key);
  const Value& at(const Key& key) const;
  Value& operator[](const Key& key);

  std::size_t size() const;
  bool empty() const;
  std::size_t bucketCount() const;
  double loadFactor() const;
  void clear();
  std::string toString() const;

private:
  class Impl;
  std::unique_ptr<Impl> impl;
};

} // namespace Ip2
