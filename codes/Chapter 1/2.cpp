#include <bits/stdc++.h>

int main() {
  std::vector<int> v;
  auto resetV = [&v](const auto &newValue) { v = newValue; };
  auto newv = { 1, 2, 3 };
  resetV(newv);
  std::cout << v[0] << "\n";
}