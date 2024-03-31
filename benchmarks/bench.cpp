#include "leftwrite.hpp"
#include <cassert>
#include <thread>

#define ANKERL_NANOBENCH_IMPLEMENT
#include "nanobench.h"

namespace BenchCfg {
using namespace std::chrono_literals;
constexpr auto num_iterations = 2557697;
constexpr auto sleep_duration = 10000ms;
}

static void SleepUntilExit() {
  std::this_thread::sleep_for(BenchCfg::sleep_duration);
}

static void Benchmark() {
  static auto leftWrite = LW::LeftWrite<int>();
  static constexpr auto Bench = [] {
    static int result = 0;
    ankerl::nanobench::Bench()
        .name("Readers")
        .minEpochIterations(BenchCfg::num_iterations)
        .run([&] {
          leftWrite
              .Read([](auto* ptr) {
                result = *ptr;
              });
          ankerl::nanobench::doNotOptimizeAway(result);
        });
    assert(result == 451);
  };

  std::jthread([] {
    ankerl::nanobench::Bench()
        .minEpochIterations(BenchCfg::num_iterations)
        .name("Writer")
        .run([&] {
          leftWrite
              .Write([](auto* ptr) {
                *ptr = 451;
              })
              .Swap();
        });
  }).detach();

  for (auto& i : {1, 2, 3, 4, 5}) {
    i == 5 ? std::jthread([&] { Bench(); }).join()
           : std::jthread([&] { Bench(); }).detach();
  }
}

int main() {
  Benchmark();

  SleepUntilExit();
}
