#include "hashMap.h"

#include <fstream>
#include <functional>
#include <iostream>
#include <string>

namespace {

struct DualLog final {
  std::ofstream file;

  explicit DualLog(const std::string& path) : file(path, std::ios::trunc) {
    if (!file) throw std::runtime_error("Cannot open log file: " + path);
  }

  void line(const std::string& s) {
    std::cout << s << "\n";
    file << s << "\n";
  }
};

int runTest(DualLog& log, const std::string& name, const std::function<void()>& fn) {
  log.line("TEST: " + name);
  try {
    fn();
    log.line("RESULT: PASS");
    return 1;
  } catch (const std::exception& e) {
    log.line(std::string("RESULT: FAIL  exception=") + e.what());
    return 0;
  } catch (...) {
    log.line("RESULT: FAIL  exception=<non-std>");
    return 0;
  }
}

} // namespace

int main() {
  DualLog log("log.txt");

  int passed = 0;
  int total = 0;

  auto add = [&](const std::string& name, const std::function<void()>& fn) {
    ++total;
    passed += runTest(log, name, fn);
    log.line("----");
  };

  add("insert and containsKey", [&] {
    Ip2::HashMap<std::string, int> m(3);
    m.insert("a", 1);
    m.insert("b", 2);
    if (!m.containsKey("a")) throw std::runtime_error("expected containsKey(a)=true");
    if (m.containsKey("c")) throw std::runtime_error("expected containsKey(c)=false");
    if (m.at("b") != 2) throw std::runtime_error("expected at(b)=2");
  });

  add("duplicate insert throws custom", [&] {
    Ip2::HashMap<std::string, int> m;
    m.insert("dup", 1);
    try {
      m.insert("dup", 2);
      throw std::runtime_error("expected DuplicateKey");
    } catch (const Ip2::HashMap<std::string, int>::DuplicateKey&) {
    }
  });

  add("erase existing and missing throws", [&] {
    Ip2::HashMap<std::string, int> m;
    m.insert("x", 42);
    m.erase("x");
    if (!m.empty()) throw std::runtime_error("expected empty after erase");
    try {
      m.erase("x");
      throw std::runtime_error("expected out_of_range");
    } catch (const std::out_of_range&) {
    }
  });

  add("update existing value", [&] {
    Ip2::HashMap<std::string, int> m;
    m.insert("k", 10);
    m.update("k", 20);
    if (m.at("k") != 20) throw std::runtime_error("expected updated value");
  });

  add("update missing throws", [&] {
    Ip2::HashMap<std::string, int> m;
    try {
      m.update("missing", 1);
      throw std::runtime_error("expected out_of_range");
    } catch (const std::out_of_range&) {
    }
  });

  add("clear empties map", [&] {
    Ip2::HashMap<std::string, int> m;
    m.insert("a", 1);
    m.insert("b", 2);
    m.clear();
    if (!m.empty()) throw std::runtime_error("expected empty after clear");
  });

  add("copy is independent", [&] {
    Ip2::HashMap<std::string, int> a;
    a.insert("k1", 10);
    a.insert("k2", 20);

    Ip2::HashMap<std::string, int> b = a;
    b.update("k1", 999);
    if (a.at("k1") != 10) throw std::runtime_error("copy is not independent");
  });

  add("operator[] inserts default value", [&] {
    Ip2::HashMap<std::string, int> m;
    if (m.size() != 0) throw std::runtime_error("expected initial size 0");
    int& value = m["new-key"];
    if (value != 0) throw std::runtime_error("expected default initialized int");
    value = 7;
    if (m.at("new-key") != 7) throw std::runtime_error("expected assigned value");
    if (m.size() != 1) throw std::runtime_error("expected size 1 after operator[] insert");
  });

  add("at missing throws", [&] {
    Ip2::HashMap<std::string, int> m;
    try {
      (void)m.at("missing");
      throw std::runtime_error("expected out_of_range");
    } catch (const std::out_of_range&) {
    }
  });

  add("equality operators", [&] {
    Ip2::HashMap<std::string, int> a;
    Ip2::HashMap<std::string, int> b;
    a.insert("x", 1);
    b.insert("x", 1);
    if (!(a == b)) throw std::runtime_error("expected a == b");
    b.update("x", 2);
    if (!(a != b)) throw std::runtime_error("expected a != b");
  });

  add("ordering operators", [&] {
    Ip2::HashMap<std::string, int> small;
    Ip2::HashMap<std::string, int> big;
    small.insert("a", 1);
    big.insert("a", 1);
    big.insert("b", 2);
    if (!(small < big)) throw std::runtime_error("expected small < big");
    if (!(small <= big)) throw std::runtime_error("expected small <= big");
    if (!(big > small)) throw std::runtime_error("expected big > small");
    if (!(big >= small)) throw std::runtime_error("expected big >= small");
  });

  add("operator! clears map", [&] {
    Ip2::HashMap<std::string, int> m;
    m.insert("a", 10);
    m.insert("b", 20);
    !m;
    if (!m.empty()) throw std::runtime_error("expected empty after operator!");
  });

  add("const at and bucket stats", [&] {
    Ip2::HashMap<std::string, int> m(7);
    m.insert("a", 1);
    const Ip2::HashMap<std::string, int>& c = m;
    if (c.at("a") != 1) throw std::runtime_error("expected const at(a)=1");
    if (c.bucketCount() == 0) throw std::runtime_error("expected non-zero bucketCount");
    if (c.loadFactor() <= 0.0) throw std::runtime_error("expected positive loadFactor");
  });

  log.line("SUMMARY:");
  log.line("passed=" + std::to_string(passed));
  log.line("total=" + std::to_string(total));

  return (passed == total) ? 0 : 1;
}

