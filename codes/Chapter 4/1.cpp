#include <vector>
#include <iostream>

std::vector<std::string> names;

template<typename T>
void add(T&& name) {
  names.emplace_back(std::forward<T>(name));
}

int main() {
  std::string petName("dym");
  add(petName);
  add(std::string("dym"));
  add("dym");
}