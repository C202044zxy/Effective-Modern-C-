#include <iostream>
#include <future>
#include <thread>

int someFunc() {
  std::this_thread::sleep_for(std::chrono::seconds(1));
  return 42;
}

int main() {
  auto fut = std::async(std::launch::deferred, someFunc);
  using namespace std::literals;
  if (fut.wait_for(0s) == std::future_status::deferred) {
    std::cout << "the thread is deferred. \n";
  }
  fut.wait();
  if (fut.wait_for(3s) == std::future_status::ready) {
    std::cout << "the thread is completed. \n";
  }
}