#include "HashMap.h"

#include <algorithm>
#include <forward_list>
#include <functional>
#include <sstream>
#include <utility>

#include "arraylist.h"

namespace Ip2 {

template <typename Key, typename Value>
class HashMap<Key, Value>::Impl final {
public:
  explicit Impl(std::size_t bucketCount)
      : buckets(bucketCount == 0 ? 1 : bucketCount), itemCount(0) {}

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
    ArrayList<std::forward_list<Entry>> newBuckets(newBucketCount);
    for (const auto& bucket : buckets) {
      for (const auto& entry : bucket) {
        const std::size_t newIdx = std::hash<Key>{}(entry.first) % newBuckets.size();
        newBuckets[newIdx].push_front(entry);
      }
    }
    buckets = std::move(newBuckets);
  }

  std::size_t size() const { return itemCount; }
  bool empty() const { return itemCount == 0; }
  std::size_t bucketCount() const { return buckets.size(); }
  double loadFactor() const {
    return buckets.empty()
               ? 0.0
               : static_cast<double>(itemCount) / static_cast<double>(buckets.size());
  }

  ArrayList<std::forward_list<Entry>> buckets;
  std::size_t itemCount;
};

template <typename Key, typename Value>
HashMap<Key, Value>::DuplicateKey::DuplicateKey(const std::string& what_arg)
    : std::invalid_argument(what_arg) {}

template <typename Key, typename Value>
HashMap<Key, Value>::HashMap() : HashMap(11) {}

template <typename Key, typename Value>
HashMap<Key, Value>::HashMap(std::size_t bucketCount)
    : impl(std::make_unique<Impl>(bucketCount)) {}

template <typename Key, typename Value>
HashMap<Key, Value>::HashMap(const HashMap& other)
    : impl(std::make_unique<Impl>(*other.impl)) {}

template <typename Key, typename Value>
HashMap<Key, Value>::HashMap(HashMap&& other) noexcept = default;

template <typename Key, typename Value>
HashMap<Key, Value>& HashMap<Key, Value>::operator=(const HashMap& other) {
  if (this == &other) {
    return *this;
  }
  impl = std::make_unique<Impl>(*other.impl);
  return *this;
}

template <typename Key, typename Value>
HashMap<Key, Value>& HashMap<Key, Value>::operator=(HashMap&& other) noexcept = default;

template <typename Key, typename Value>
HashMap<Key, Value>::~HashMap() = default;

template <typename Key, typename Value>
void HashMap<Key, Value>::insert(const Key& key, const Value& value) {
  if (containsKey(key)) {
    throw DuplicateKey("HashMap: duplicate key");
  }
  const std::size_t idx = impl->bucketIndex(key);
  impl->buckets[idx].push_front(Entry(key, value));
  ++impl->itemCount;
  impl->rehashIfNeeded();
}

template <typename Key, typename Value>
void HashMap<Key, Value>::erase(const Key& key) {
  const std::size_t idx = impl->bucketIndex(key);
  auto& bucket = impl->buckets[idx];
  auto prev = bucket.before_begin();
  for (auto it = bucket.begin(); it != bucket.end(); ++it) {
    if (it->first == key) {
      bucket.erase_after(prev);
      --impl->itemCount;
      return;
    }
    ++prev;
  }
  throw std::out_of_range("HashMap: key not found");
}

template <typename Key, typename Value>
void HashMap<Key, Value>::update(const Key& key, const Value& value) {
  Value& stored = at(key);
  stored = value;
}

template <typename Key, typename Value>
bool HashMap<Key, Value>::containsKey(const Key& key) const {
  const std::size_t idx = impl->bucketIndex(key);
  const auto& bucket = impl->buckets[idx];
  return std::find_if(bucket.begin(), bucket.end(), [&](const Entry& entry) {
           return entry.first == key;
         }) != bucket.end();
}

template <typename Key, typename Value>
Value& HashMap<Key, Value>::at(const Key& key) {
  const std::size_t idx = impl->bucketIndex(key);
  auto& bucket = impl->buckets[idx];
  auto it = std::find_if(bucket.begin(), bucket.end(), [&](Entry& entry) {
    return entry.first == key;
  });
  if (it == bucket.end()) {
    throw std::out_of_range("HashMap: key not found");
  }
  return it->second;
}

template <typename Key, typename Value>
const Value& HashMap<Key, Value>::at(const Key& key) const {
  const std::size_t idx = impl->bucketIndex(key);
  const auto& bucket = impl->buckets[idx];
  auto it = std::find_if(bucket.begin(), bucket.end(), [&](const Entry& entry) {
    return entry.first == key;
  });
  if (it == bucket.end()) {
    throw std::out_of_range("HashMap: key not found");
  }
  return it->second;
}

template <typename Key, typename Value>
Value& HashMap<Key, Value>::operator[](const Key& key) {
  const std::size_t idx = impl->bucketIndex(key);
  auto& bucket = impl->buckets[idx];
  auto it = std::find_if(bucket.begin(), bucket.end(), [&](Entry& entry) {
    return entry.first == key;
  });
  if (it != bucket.end()) {
    return it->second;
  }
  bucket.push_front(Entry(key, Value{}));
  ++impl->itemCount;
  impl->rehashIfNeeded();
  return at(key);
}

template <typename Key, typename Value>
std::size_t HashMap<Key, Value>::size() const {
  return impl->size();
}

template <typename Key, typename Value>
bool HashMap<Key, Value>::empty() const {
  return impl->empty();
}

template <typename Key, typename Value>
std::size_t HashMap<Key, Value>::bucketCount() const {
  return impl->bucketCount();
}

template <typename Key, typename Value>
double HashMap<Key, Value>::loadFactor() const {
  return impl->loadFactor();
}

template <typename Key, typename Value>
void HashMap<Key, Value>::clear() {
  for (auto& bucket : impl->buckets) {
    bucket.clear();
  }
  impl->itemCount = 0;
}

template <typename Key, typename Value>
std::string HashMap<Key, Value>::toString() const {
  std::ostringstream oss;
  oss << "HashMap{size=" << size() << ", buckets=" << bucketCount()
      << ", loadFactor=" << loadFactor() << "}";
  return oss.str();
}

template class HashMap<std::string, int>;

} // namespace Ip2