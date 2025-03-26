#include <iostream>
#include <type_traits>

enum class ppp { ksm, ars, otae, rimi, saya }; 
template<typename E>
constexpr auto toUType(E enumerator) noexcept {
  return static_cast<std::underlying_type_t<E>>(enumerator);
}

int main() {
  std::cout << toUType(ppp::otae) << "\n";
}