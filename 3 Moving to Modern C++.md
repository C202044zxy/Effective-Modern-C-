## 3.1 Use {} when creating objects

Using braces, specifying the initial contents of a container is easy:

```c++
std::vector<int> v{ 1, 2, 5 }; 
```

Braces can also be used to specify default initialization values for non-static members:

```c++
class Widget {
private:
	int x{ 0 };	// fine
	int y = 0;	// also fine
	int z(0); 	// ERROR: conflict with the direct initialization
}
```

On the other hand, uncopiable objects (e.g., `std::atomic`) may be initialized using braces or parentheses, but not `=`.

```c++
std::atomic<int> x { 0 };	// fine
std::atomic<int> y (0); 	// also fine
std::atomic<int> z = 0; 	// ERROR!
```

Therefore, only braces can be used everywhere. Additionally, braced initialization offers many novel features. The first one is that it prohibits implicit narrowing conversions between built-in types. 

Another noteworthy characteristic of braced initialization is its immunity to C++'s *most vexing parse*:

```c++
Widget w2();	// declares a function named w2 that returns a Widget. 
				// not calls Widget constructor with no args. 
Widget w3{}; 	// calls Widget constructor with no args. 
```

## 3.2 Prefer nullptr to 0 and NULL

The actual type of `nullptr` is `std::nullptr_t`. This type implicitly converts to all raw pointer types, making `nullptr` act as if it were a pointer of all types. 

`nullptr` can't be viewed as anything integral. It can also improve code clarity, especially when `auto` variables are involved. 

## 3.3 Prefer alias declarations to typedefs

Alias declaration is easy to implement with types involving function pointers:

```c++
using FP = void (*)(int, const std::string&);
```

Alias declarations may be templatized. For example, consider defining a synonym for a linked list that uses a custom allocator:

```c++
template<typename T>
using MyAllocList = std::list<T, MyAlloc<T>>;
MyAllocList<Widget> l;
```

## 3.4 Prefer scoped enum to unscoped enum

Enumerators of scoped enum are visible only within the enum. Using scoped enum can avoid possible variable name conflict.

```c++
enum Color { black, white, red };
auto white = false;	// ERROR: white already declared in this scope. 
```

```c++
enum class Color { black, white, red };
auto white = false; // fine
```

There are no implicit conversions from enumerators in a scoped enum to any other type. Additionally, their names can be specified without specifying their enumerators (i.e., forward-declared). 

The default underlying type for scoped enum is `int`. We can convert enumerators to its underlying type using `static_cast`:

```c++
template<typename E>
constexpr auto toUType(E enumerator) noexcept {
	return static_cast<std::underlying_type_t<E>>(enumerator);
}
```

## 3.5 Prefer deleted functions

In C++11, declaring functions `= delete` can prevent clients from calling them.

For example, we can mark the copy constructor and the copy assignment operator as deleted functions for class template `basic_ios` in C++ standard library:

```c++
template<class charT, class traits = char_traits<charT>>
class basic_ios : public ios_base {
public:
    basic_ios(const basic_ios&) = delete;
    basic_ios& operator=(const basic_ios&) = delete;
};
```

## 3.6 Declare overriding functions override

Virtual function overriding is what makes it possible to invoke derived class function through a base class interface:

```c++
class Base {
public:
  virtual void doWork() {
    std::cout << "do work base\n";
  }
};

class Derived: public Base {
public:
  virtual void doWork() override {
    std::cout << "do work derived\n";
  }
};

int main() {
  std::unique_ptr<Base> ptr = std::make_unique<Derived>();
  ptr->doWork();	// do work derived
}
```

For override to occur, several requirements must be met:

- The base class function must be virtual.
- The base and derived function names must be identical. 
- The parameter types and constness of the base and derived functions must be identical. 
- The return types and exception specifications of the base and derived functions must be compatible. 
- The functions' *reference qualifier* must be identical. 

```c++
class Widget {
public:
	void doWork() &;	// runs only when *this is an lvalue
	void doWork() &&; 	// runs only when *this is an rvalue 
};
```

