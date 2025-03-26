#include <string>
#include <vector>
#include <memory>
#include <iostream>

class Widget {
public:
  Widget() = default;
  ~Widget() = default;
};

int main() {
  auto spw = std::make_shared<Widget>();
  std::weak_ptr<Widget> wpw(spw);
  spw = nullptr;
  auto res = wpw.lock();
  if (res == nullptr) {
    std::cout << "It is indeed null" << std::endl;
  }
}