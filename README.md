## LeftWrite

#### LeftWrite is a C++20 templated, header-only wait-free concurrent data structure for multi-threaded single-writer, multiple-reader scenarios.

```cpp
namespace {
auto LeftWrite = LW::LeftWrite<std::vector<std::string>>();

[[noreturn]] auto reader() {
  for (;;) {
    LeftWrite.Read([](auto* lw) {
      for (auto& i : *lw) {
        assert(i == "Hello");
      }
    });
  }
}

[[noreturn]] auto writer() {
  for (;;) {
    LeftWrite.Write([](auto* lw) { lw->emplace_back("Hello"); }).Swap();
  }
}
}

auto main() -> int {
  // readers
  for (auto& i : {1, 2, 3}) {
    std::jthread(reader).detach();
  }

  // writer
  std::jthread(writer).join();
}

```


Inspired by Left-Right: A Concurrency Control Technique with Wait-Free Population Oblivious Reads (Ramalhete, P. and Correia, A.)