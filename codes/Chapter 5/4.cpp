#include <iostream>
#include <future>
#include <thread>

void producer(std::promise<int> prom) {
  using namespace std::literals;
  std::this_thread::sleep_for(1s);
  prom.set_value(42);
}

void consumer(std::future<int> fut) {
  std::cout << "waiting for the value \n";
  auto message = fut.get();
  std::cout << "value received! \n";
}

int main() {
  std::promise<int> prom;
  std::future<int> fut = prom.get_future(); // bind promise to future

  std::thread t1(producer, std::move(prom));
  std::thread t2(consumer, std::move(fut));

  t1.join();
  t2.join();
}