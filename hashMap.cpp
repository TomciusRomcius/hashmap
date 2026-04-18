#include "hashMap.h"

#include <functional>
#include <sstream>
#include <utility>

#include "arrayList.h"
#include "forwardList.h"

namespace Ip2 {

DuplicateKey::DuplicateKey(const std::string& what_arg)
  : std::invalid_argument(what_arg) {}

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
    ArrayList<ForwardList<Entry>> newBuckets(newBucketCount);
    for (const auto& bucket : buckets) {
      bucket.for_each([&](const Entry& entry) {
        const std::size_t newIdx = std::hash<Key>{}(entry.first) % newBuckets.size();
        newBuckets[newIdx].push_front(entry);
      });
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

  ArrayList<ForwardList<Entry>> buckets;
  std::size_t itemCount;
};

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
  const bool removed = bucket.erase_first_if([&](const Entry& entry) {
    return entry.first == key;
  });
  if (removed) {
    --impl->itemCount;
    return;
  }
  throw std::out_of_range("HashMap: key not found");
}

template <typename Key, typename Value>
void HashMap<Key, Value>::update(const Key& key, const Value& value) {
  Value stored = at(key);
  stored = value;
}

template <typename Key, typename Value>
bool HashMap<Key, Value>::containsKey(const Key& key) const {
  const std::size_t idx = impl->bucketIndex(key);
  const auto& bucket = impl->buckets[idx];
  return bucket.find_if([&](const Entry& entry) {
           return entry.first == key;
         }) != nullptr;
}

template <typename Key, typename Value>
Value& HashMap<Key, Value>::at(const Key& key) {
  const std::size_t idx = impl->bucketIndex(key);
  auto& bucket = impl->buckets[idx];
  Entry* found = bucket.find_if([&](const Entry& entry) {
    return entry.first == key;
  });
  if (found == nullptr) {
    throw std::out_of_range("HashMap: key not found");
  }
  return found->second;
}

template <typename Key, typename Value>
const Value& HashMap<Key, Value>::at(const Key& key) const {
  const std::size_t idx = impl->bucketIndex(key);
  const auto& bucket = impl->buckets[idx];
  const Entry* found = bucket.find_if([&](const Entry& entry) {
    return entry.first == key;
  });
  if (found == nullptr) {
    throw std::out_of_range("HashMap: key not found");
  }
  return found->second;
}

template <typename Key, typename Value>
Value& HashMap<Key, Value>::operator[](const Key& key) {
  const std::size_t idx = impl->bucketIndex(key);
  auto& bucket = impl->buckets[idx];
  Entry* found = bucket.find_if([&](const Entry& entry) {
    return entry.first == key;
  });
  if (found != nullptr) {
    return found->second;
  }
  bucket.push_front(Entry(key, Value{}));
  ++impl->itemCount;
  impl->rehashIfNeeded();
  return at(key);
}

template <typename Key, typename Value>
HashMap<Key, Value>& HashMap<Key, Value>::operator!() {
  clear();
  return *this;
}

template <typename Key, typename Value>
bool HashMap<Key, Value>::operator==(const HashMap& other) const {
  if (size() != other.size()) {
    return false;
  }

  for (const auto& bucket : impl->buckets) {
    bool mismatch = false;
    bucket.for_each([&](const Entry& entry) {
      const std::size_t otherIdx = other.impl->bucketIndex(entry.first);
      const auto& otherBucket = other.impl->buckets[otherIdx];
      const Entry* otherIt = otherBucket.find_if([&](const Entry& candidate) {
        return candidate.first == entry.first;
      });

      if (otherIt == nullptr || !(otherIt->second == entry.second)) {
        mismatch = true;
      }
    });
    if (mismatch) {
      return false;
    }
  }

  return true;
}

template <typename Key, typename Value>
bool HashMap<Key, Value>::operator!=(const HashMap& other) const {
  return !(*this == other);
}

template <typename Key, typename Value>
bool HashMap<Key, Value>::operator<=(const HashMap& other) const {
  for (const auto& bucket : impl->buckets) {
    bool missing = false;
    bucket.for_each([&](const Entry& entry) {
      const std::size_t otherIdx = other.impl->bucketIndex(entry.first);
      const auto& otherBucket = other.impl->buckets[otherIdx];
      const Entry* otherIt = otherBucket.find_if([&](const Entry& candidate) {
        return candidate.first == entry.first;
      });

      if (otherIt == nullptr || !(otherIt->second == entry.second)) {
        missing = true;
      }
    });
    if (missing) {
      return false;
    }
  }

  return true;
}

template <typename Key, typename Value>
bool HashMap<Key, Value>::operator<(const HashMap& other) const {
  return size() < other.size() && (*this <= other);
}

template <typename Key, typename Value>
bool HashMap<Key, Value>::operator>(const HashMap& other) const {
  return other < *this;
}

template <typename Key, typename Value>
bool HashMap<Key, Value>::operator>=(const HashMap& other) const {
  return other <= *this;
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