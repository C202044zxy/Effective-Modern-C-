#include <iostream>

void someFunc(int x) {
  std::cout << "Function is called successfully" << "\n";
}

template<typename T>
void f1(T param) {
  std::cout << std::is_reference<decltype(param)>::value << "\n";
  (*param)(123);
}

template<typename T>
void f2(T& param) {
  std::cout << std::is_reference<decltype(param)>::value << "\n";
  param(123);
}

int main() {
  f1(someFunc);
  f2(someFunc);
}