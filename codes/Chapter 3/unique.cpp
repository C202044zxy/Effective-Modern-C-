#include <iostream>
#include <memory>

class Investment {
public:
  Investment() = default;
  ~Investment() = default;
private:
  int val { 1 };
};

int main() {
  auto delFunc = [](Investment* pInvestment) {
    std::cout << "the object was deleted." << std::endl;
    delete pInvestment;
  };
  std::unique_ptr<Investment, decltype(delFunc)>
    ptr(new Investment(), delFunc);
}