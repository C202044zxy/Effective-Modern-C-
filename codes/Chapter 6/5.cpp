#include <iostream>
#include <thread>

void doAsyncWork() {
  using namespace std::literals;
  for (int i = 0; i < 10; i++) {
    std::cout << "running ... " << std::endl;
    std::this_thread::sleep_for(1s);
  }
}

int main() {
  std::thread t(doAsyncWork);
  t.detach();
  using namespace std::literals;
  std::this_thread::sleep_for(3s);
  std::cout << "The calling thread is about to terminate.\n";
  // thread t keeps running. 
  // but the result won't be shown on the screen. 
}