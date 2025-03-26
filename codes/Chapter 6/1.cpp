#include <iostream>
#include <thread>
#include <future>

int calcValue() {
  std::cout << "the process is running" << std::endl;
  return 42;
}

int main() {
  std::packaged_task<int()> pt(calcValue);
  auto fut = pt.get_future();
  // std::cout << fut.get() << std::endl;
  std::thread t(std::move(pt));
  t.join();
  std::cout << fut.get() << std::endl;
}