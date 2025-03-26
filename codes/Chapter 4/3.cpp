#include <iostream>

class Widget {
public:
  Widget() = default; 
  Widget(int& rhs) {
    std::cout << "this is a lvalue\n";
  }
  Widget(int&& rhs) {
    std::cout << "this is a rvalue\n";
  }
};

template<typename T>
void fwd(T&& arg) {
  Widget A(std::forward<T>(arg));
}

int main() {
  int x = 1;
  int y = 1;
  decltype(x) sb;
  fwd(x + y);
  fwd((x + y));
  Widget z((x + y));
}