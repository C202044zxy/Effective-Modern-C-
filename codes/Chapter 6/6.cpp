#include <iostream>
#include <thread>

// RAII stands for "Resource Acquisition Is Initialization"
class ThreadRAII {
public:
  enum class DtorAction { join, detach };
  ThreadRAII(std::thread&& t, DtorAction a)
  : action(a), t(std::move(t)) {}
  ~ThreadRAII() {
    if (t.joinable()) {
      if (action == DtorAction::join) {
        t.join();
      } else {
        t.detach();
      }
    }
  }
  std::thread& get() { return t; }
private:
  DtorAction action;
  std::thread t;
};

int main() {
  ThreadRAII obj(std::thread([]{
    std::cout << "The object starts running" << std::endl;
    using namespace std::literals;
    std::this_thread::sleep_for(1s);
  }), ThreadRAII::DtorAction::join);
}