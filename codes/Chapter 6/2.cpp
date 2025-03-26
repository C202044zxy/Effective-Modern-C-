#include <iostream>
#include <thread>
#include <mutex>

std::mutex mtx;

void access_data(int id) {
  std::lock_guard<std::mutex> lck(mtx);
  std::cout << "Thread " << id << " is accessing the data\n";
}

int main() {
  std::thread t1(access_data, 1);
  std::thread t2(access_data, 2);
  std::thread t3(access_data, 3);
  t1.join();
  t2.join();
  t3.join();
}