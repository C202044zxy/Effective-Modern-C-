#include <iostream>
#include <functional>
#include <vector>
using namespace std;

int main() {
  vector<int> vc = { 1, 2, 3, 4 };
  auto func = std::bind(
    [](const vector<int>& vc, int y) {
      for (const auto& x : vc) {
        std::cout << x << std::endl;
      }
    },
    std::move(vc), 
    std::placeholders::_1
  );
  func(1);
  std::cout << vc.size() << std::endl;
}