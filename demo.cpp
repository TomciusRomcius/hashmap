#include "HashMap.h"

#include <iostream>

int main() {
  using Map = Ip2::HashMap<std::string, int>;

  Map m(5);
  std::cout << m.toString() << "\n";

  m.insert("key1", 10);
  m.insert("key2", 20);
  m["gamma"] = 30;

  std::cout << "containsKey(key1)=" << (m.containsKey("key1") ? "true" : "false") << "\n";
  std::cout << "containsKey(delta)=" << (m.containsKey("delta") ? "true" : "false") << "\n";
  std::cout << "value(key1)=" << m.at("key1") << "\n";
  std::cout << "value(gamma)=" << m.at("gamma") << "\n";
  std::cout << m.toString() << "\n";

  m.update("key2", 99);
  std::cout << "after update(key2): " << m.toString() << "\n";

  m.erase("key1");
  std::cout << "after erase(key1): " << m.toString() << "\n";

  Map copy = m;
  copy["delta"] = 1234;
  std::cout << "copy value(delta): " << copy.at("delta") << "\n";
  std::cout << "original containsKey(delta): " << (m.containsKey("delta") ? "true" : "false") << "\n";

  m.clear();
  std::cout << "cleared original: " << m.toString() << "\n";
  std::cout << "copy remains: " << copy.toString() << "\n";

  return 0;
}

