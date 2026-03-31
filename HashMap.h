#pragma once

#include <algorithm>
#include <cstddef>
#include <forward_list>
#include <functional>
#include <sstream>
#include <stdexcept>
#include <string>
#include <utility>
#include <vector>

namespace ip2 {

template <typename Key, typename Value>
class HashMap final {
public:
  using Entry = std::pair<Key, Value>;

  class DuplicateKey : public std::invalid_argument {
  public:
    explicit DuplicateKey(const std::string& what_arg) : std::invalid_argument(what_arg) {}
  };

  HashMap() : HashMap(11) {}

  explicit HashMap(std::size_t bucketCount)
      : buckets(bucketCount <= 0 ? 1 : bucketCount), itemCount(0) {}

  void insert(const Key& key, const Value& value) {
    if (containsKey(key)) {
      throw DuplicateKey("HashMap: duplicate key");
    }
    const std::size_t idx = bucketIndex(key);
    buckets[idx].push_front(Entry(key, value));
    itemCount++;
    rehashIfNeeded();
  }

  void erase(const Key& key) {
    const std::size_t idx = bucketIndex(key);
    auto& bucket = buckets[idx];
    auto prev = bucket.before_begin();
    for (auto it = bucket.begin(); it != bucket.end(); ++it) {
      if (it->first == key) {
        bucket.erase_after(prev);
        itemCount--;
        return;
      }
      ++prev;
    }
    throw std::out_of_range("HashMap: key not found");
  }

  void update(const Key& key, const Value& value) {
    Value& stored = at(key);
    stored = value;
  }

  bool containsKey(const Key& key) const {
    const std::size_t idx = bucketIndex(key);
    const auto& bucket = buckets[idx];
    return std::find_if(bucket.begin(), bucket.end(), [&](const Entry& entry) {
             return entry.first == key;
           }) != bucket.end();
  }

  Value& at(const Key& key) {
    const std::size_t idx = bucketIndex(key);
    auto& bucket = buckets[idx];
    auto it = std::find_if(bucket.begin(), bucket.end(), [&](Entry& entry) {
      return entry.first == key;
    });
    if (it == bucket.end()) {
      throw std::out_of_range("HashMap: key not found");
    }
    return it->second;
  }

  const Value& at(const Key& key) const {
    const std::size_t idx = bucketIndex(key);
    const auto& bucket = buckets[idx];
    auto it = std::find_if(bucket.begin(), bucket.end(), [&](const Entry& entry) {
      return entry.first == key;
    });
    if (it == bucket.end()) {
      throw std::out_of_range("HashMap: key not found");
    }
    return it->second;
  }

  Value& operator[](const Key& key) {
    const std::size_t idx = bucketIndex(key);
    auto& bucket = buckets[idx];
    auto it = std::find_if(bucket.begin(), bucket.end(), [&](Entry& entry) {
      return entry.first == key;
    });
    if (it != bucket.end()) {
      return it->second;
    }
    bucket.push_front(Entry(key, Value{}));
    ++itemCount;
    rehashIfNeeded();
    return at(key);
  }

  std::size_t size() const { return itemCount; }
  bool empty() const { return itemCount == 0; }
  std::size_t bucketCount() const { return buckets.size(); }

  double loadFactor() const {
    return buckets.empty()
               ? 0.0
               : static_cast<double>(itemCount) / static_cast<double>(buckets.size());
  }

  void clear() {
    for (auto& bucket : buckets) {
      bucket.clear();
    }
    itemCount = 0;
  }

  std::string toString() const {
    std::ostringstream oss;
    oss << "HashMap{size=" << size() << ", buckets=" << bucketCount() << ", loadFactor=" << loadFactor() << "}";
    return oss.str();
  }

private:
  std::size_t bucketIndex(const Key& key) const {
    if (buckets.empty()) {
      throw std::logic_error("HashMap: bucket storage not initialized");
    }
    return std::hash<Key>{}(key) % buckets.size();
  }

  void rehashIfNeeded() {
    if (loadFactor() <= 0.75) {
      return;
    }
    const std::size_t newBucketCount = buckets.size() * 2 + 1;
    std::vector<std::forward_list<Entry>> newBuckets(newBucketCount);
    for (const auto& bucket : buckets) {
      for (const auto& entry : bucket) {
        const std::size_t newIdx = std::hash<Key>{}(entry.first) % newBuckets.size();
        newBuckets[newIdx].push_front(entry);
      }
    }
    buckets = std::move(newBuckets);
  }

  std::vector<std::forward_list<Entry>> buckets;
  std::size_t itemCount;
};

} // namespace ip2
