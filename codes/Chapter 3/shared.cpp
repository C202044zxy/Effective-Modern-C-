#include <iostream>
#include <memory>

void myFunc(std::shared_ptr<int> ptr) {
  std::cout << ptr.use_count();
}

int main() {
  std::shared_ptr<int> ptr1 = std::make_shared<int>(10);
  myFunc(std::move(ptr1));
  std::cout << ptr1.use_count();
}
