#include <iostream>
#include <type_traits>

template<typename T>
void f(T&& param) {
  T x;
  std::cout << std::is_reference<T>::value << std::endl;
}

int main() {
  int y = 1;
  auto x = y;
  f(1);
}