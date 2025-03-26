#include <bits/stdc++.h>

int main() {
  auto f1 = [](int x, int y) {
    std::cout << "1" << "\n";
    std::cout << "2" << "\n";
    std::cout << "2" << "\n";
    std::cout << "2" << "\n";
    std::cout << "2" << "\n";
  };
  
  std::function<void()> f2 = []() {
    std::cout << "1" << "\n";
    std::cout << "2" << "\n";
  };

  std::cout << sizeof(f1) << "\n";
  std::cout << sizeof(f2) << "\n";
}