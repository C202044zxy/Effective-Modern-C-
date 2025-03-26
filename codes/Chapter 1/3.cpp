#include <bits/stdc++.h>

template<typename Container, typename Index>
decltype(auto) get(Container &c, Index i) {
    return c[i];  // Preserves reference
}

int main() {
  std::vector<int> v = { 1, 2, 3 };
  get(v, 1) = 5;  // v[1] thus can be modified 
  
  int z = 0;
  decltype(z) y = v[0];
  std::cout << std::is_reference<decltype((z))>::value << "\n";
}