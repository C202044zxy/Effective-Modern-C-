#include <iostream>
#include <thread>
#include <mutex>
#include <condition_variable>

std::mutex mtx;
std::condition_variable cv;
bool flag = false;

void notifier() {
  std::lock_guard<std::mutex> lck(mtx);
  flag = true;
  cv.notify_one();
  std::cout << "the thread notify the other.\n";
}

void worker() {
  std::unique_lock<std::mutex> lck(mtx);
  std::cout << "the thread is waiting.\n";
  cv.wait(lck, []{ return flag; });
  std::cout << "the thread is waken up.\n";
}

int main() {
  std::thread t1(worker);
  // ensure wait() before notify_one()
  using namespace std::literals;
  std::this_thread::sleep_for(1s);
  std::thread t2(notifier);
  t1.join();
  t2.join();
}