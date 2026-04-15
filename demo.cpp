#include "hashMap.h"

#include <iostream>

int main() {
  using Map = Ip2::HashMap<std::string, int>;

  Map m(5);
  std::cout << "initial: " << m.toString() << "\n";
  std::cout << "empty=" << (m.empty() ? "true" : "false") << ", size=" << m.size()
            << ", bucketCount=" << m.bucketCount() << ", loadFactor=" << m.loadFactor() << "\n";

  m.insert("key1", 10);
  m.insert("key2", 20);
  m["gamma"] = 30;
  std::cout << "after inserts: " << m.toString() << "\n";

  std::cout << "containsKey(key1)=" << (m.containsKey("key1") ? "true" : "false") << "\n";
  std::cout << "containsKey(delta)=" << (m.containsKey("delta") ? "true" : "false") << "\n";

  std::cout << "at(key1)=" << m.at("key1") << "\n";
  const Map& constRef = m;
  std::cout << "const at(gamma)=" << constRef.at("gamma") << "\n";

  m.update("key2", 99);
  std::cout << "after update(key2): " << m.toString() << "\n";

  m.erase("key1");
  std::cout << "after erase(key1): " << m.toString() << "\n";

  Map copy = m;
  copy["delta"] = 1234;
  std::cout << "copy value(delta): " << copy.at("delta") << "\n";
  std::cout << "original containsKey(delta): " << (m.containsKey("delta") ? "true" : "false") << "\n";

  std::cout << "copy == m: " << (copy == m ? "true" : "false") << "\n";
  std::cout << "copy != m: " << (copy != m ? "true" : "false") << "\n";
  std::cout << "m < copy: " << (m < copy ? "true" : "false") << "\n";
  std::cout << "m <= copy: " << (m <= copy ? "true" : "false") << "\n";
  std::cout << "copy > m: " << (copy > m ? "true" : "false") << "\n";
  std::cout << "copy >= m: " << (copy >= m ? "true" : "false") << "\n";

  !m;
  std::cout << "after operator! (clear): " << m.toString() << "\n";

  copy.clear();
  std::cout << "after clear(copy): " << copy.toString() << "\n";

  return 0;
}

