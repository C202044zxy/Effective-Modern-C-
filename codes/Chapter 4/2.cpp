#include <iostream>
#include <utility>

class Type {
public:
  Type() {
    std::cout << "construction happens" << std::endl;
  }
  Type(Type&) {
    std::cout << "copy happens" << std::endl;
  }
  Type(Type&&) {
    std::cout << "move happens" << std::endl;
  }
  ~Type() {
    std::cout << "goodbye" << std::endl;
  }
};

void func(const Type &x) {
  std::cout << "function happens" << std::endl;
}

int main() {
  func(Type());
  std::cout << "hello" << std::endl;
}