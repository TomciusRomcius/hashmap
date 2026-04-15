#pragma once

#include <cstddef>
#include <memory>
#include <stdexcept>
#include <string>
#include <utility>

namespace Ip2 {

// Hash map ADT with unique keys and value lookup/update.
// Uses bucket storage using a forward list to store the entries.
template <typename Key, typename Value>
class HashMap final {
public:
  using Entry = std::pair<Key, Value>;

  // Thrown when trying to insert an already existing key.
  class DuplicateKey : public std::invalid_argument {
  public:
    explicit DuplicateKey(const std::string& what_arg);
  };

  // Creates a map with default bucket count.
  HashMap();
  // Creates a map with a specific number of buckets.
  explicit HashMap(std::size_t bucketCount);
  HashMap(const HashMap& other);
  HashMap(HashMap&& other) noexcept;
  HashMap& operator=(const HashMap& other);
  HashMap& operator=(HashMap&& other) noexcept;
  ~HashMap();

  // Inserts a new key-value pair. Throws DuplicateKey if key already exists.
  void insert(const Key& key, const Value& value);
  // Removes key-value pair. Throws std::out_of_range if key is missing.
  void erase(const Key& key);
  // Replaces value for an existing key. Throws std::out_of_range if missing.
  void update(const Key& key, const Value& value);
  // Returns true if the key exists.
  bool containsKey(const Key& key) const;
  // Returns reference to stored value. Throws std::out_of_range if missing.
  Value& at(const Key& key);
  const Value& at(const Key& key) const;
  // Returns reference to value for key; inserts default value if missing.
  Value& operator[](const Key& key);
  // Clears all entries.
  HashMap& operator!();

  bool operator==(const HashMap& other) const;
  bool operator!=(const HashMap& other) const;
  bool operator<(const HashMap& other) const;
  bool operator<=(const HashMap& other) const;
  bool operator>(const HashMap& other) const;
  bool operator>=(const HashMap& other) const;

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
