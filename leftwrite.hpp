#pragma once

#include <atomic>
#include <functional>
#include <vector>

namespace LW {
template <typename T> class LeftWrite {
  struct LWHalf final {
    T half{};
    alignas(64) std::atomic<std::size_t> num_readers{0};
  };

  struct LWHalfPair final {
    LWHalf* active = nullptr;
    LWHalf* inactive = nullptr;
  };

  LWHalfPair LoadAtomicPointer() {
    return active_half_ptr.load(std::memory_order_acquire) == &left
               ? LWHalfPair{&left, &right}
               : LWHalfPair{&right, &left};
  }

  LWHalf left{};
  LWHalf right{};

  alignas(64) std::atomic<LWHalf*> active_half_ptr = &left;
  std::vector<std::function<void(T*)>> history{};

public:
  explicit LeftWrite(std::size_t Slack = 64) { history.reserve(Slack); }

  template <typename F> void Read(F&& fn) {
    auto curr = LoadAtomicPointer();
    curr.active->num_readers.fetch_add(1, std::memory_order_release);
    fn(&curr.active->half);
    curr.active->num_readers.fetch_sub(1, std::memory_order_release);
  }

  template <typename F> void Write(F&& fn) {
    history.emplace_back(std::forward<F>(fn));
  }

  void Swap() {
    if (history.empty()) {
      return;
    }

    auto [active, inactive] = LoadAtomicPointer();
    for (auto& i : history) {
      i(&inactive->half);
    }

    for (;;) {
      if (active->num_readers.load(std::memory_order_acquire) == 0) {
        active_half_ptr.store(inactive, std::memory_order_release);
        for (auto& i : history) {
          i(&active->half);
        }
        history.clear();
        return;
      }
    }
  }
};
} // namespace LW
