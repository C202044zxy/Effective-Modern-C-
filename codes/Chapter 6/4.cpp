#include <iostream>
#include <thread>

void doAsyncWork() {
  using namespace std::literals; 
  for (int i = 1; i < 10; i++) {
    std::cout << "In thread 1\n";
  }
  // std::this_thread::sleep_for(10s);
}

void doAsyncWork2() {
  for (int i = 1; i < 10; i++) {
    std::cout << "In thread 2\n";
  }
}

int main() {
  std::thread t(doAsyncWork);
  std::thread t2(doAsyncWork2);
  t.join();
  t2.join();
  std::cout << "the process is blocked." << std::endl;
}