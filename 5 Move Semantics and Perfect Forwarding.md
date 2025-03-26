**Move semantics** makes it possible for compilers to replace expensive copying operations with less expensive moves. 

**Perfect forwarding** makes it possible to write function templates that takes arbitrary arguments and forward them to other functions such that the target functions receive exactly the same arguments as were passed to the forwarding functions. 

It's especially important to bear in mind that a parameter is always a lvalue, even if its type is an rvalue reference. 

## 5.1 std::move and std::forward

`std::move` and `std::forward` are merely functions that perform casts. They generate no executable code. Not a single byte. 

`std::move` can be written this way:

```c++
template<typename T>
decltype(auto) move(T&& param) {
	using ReturnType = remove_reference_t<T>&&;
	return static_cast<ReturnType>(param);
}
```

Consider the following example:

```c++
class Annotation {
public:
    explicit Annotation(const std::string text)
    : value(std::move(text)) { ... }
    // No move constructor
    // std::string copy constructor is called. 
private:
    std::string value;
}
```

The result of `std::move(text)` is an rvalue of type `const std::string`. That rvalue can't be passed to move constructor of `std::string`, because the move constructor takes an rvalue reference to a non-const `std::string`. 

There are two lessons drawn from this example. First, don't declare objects `const` if you want to be able to move from them. Second, `std::move` not only doesn't actually move anything, it doesn't guarantee that the object will be eligible to be moved. 

`std::forward` performs a conditional cast — it casts to an rvalue only if its arguments was initialized with an rvalue. 

Therefore, `std::move` typically sets up a move, while `std::forward` just passes an object to another function in a way that retains its original lvalue-ness or rvalue-ness. Neither `std::move` nor `std::forward` do anything at runtime. 

## 5.2 Distinguish universal references from rvalue references

If a function template parameter has type `T&&` for a deduced type `T`, or if an object is declared using `auto&&`, the parameter or object is a universal reference. 

If the form of the type declaration isn't precisely `type&&`, or if type deduction does not occur, `type&&` denotes an rvalue reference. 

## 5.3 Use std::move on rvalue references, std::forward on universal references

If you have an rvalue reference parameter, you know that the object it's bound to may be moved:

```c++
class Widget {
public:
	// move constructor
	Widget(Widget&& rhs) 
		: name(std::move(rhs.name)), p(std::move(rhs.p)) {}
private:
    std::string name;
    std::shared_ptr<SomeDataStructure> p;
};
```

A universal reference, on the other hand, might be bound to an object that is eligible for moving: 

```c++
class Widget {
public:
	template<typename T>
	void setName(T&& newName)
	{ name = std::forward<T>(newName); }
};
```

Using `std::move` on universal reference is problematic:

```c++
class Widget {
public:
	template<typename T>
	void setName(T&& newName)
	{ name = std::move(newName); } // Bad! Bad! Bad!
}

std::string getWidgetName();
Widget w;
auto n = getWidgetName();	// n is local variable
w.setName(n); 				// moves n into w
// n's value now unknown
```

## 5.4 Understand reference collapsing

References to references are illegal in C++. When these happen in universal reference, *reference collapsing* is applied. 

> When compliers generate a reference to a reference in a reference collapsing context, the result becomes a single reference. 
>
> If either reference is an lvalue reference, the result is an lvalue reference. Otherwise (i.e., if both are rvalue references) the result is an rvalue reference. 

Here's how `std::forward` can be implemented with reference collapsing:

```c++
template<typename T>
T&& forward(typename remove_reference<T>::type& param) {
  return static_cast<T&&>(param);
}

// use case
template<typename T>
void f(T&& fParam) {
  someFunc(std::forward<T>(fParam));
}
```

Since universal reference ignores reference qualifier (both lvalue reference and rvalue reference), there are two passible cases:

- If an lvalue is passed into `f` (lvalue / lvalue reference), `T` is deduced as `Widget&`, the type passed in `forward` is `Widget&`. Therefore, `Widget& &&` collapses to `Widget&` and an lvalue reference is returned. 
- If an rvalue is passed into `f` (rvalue / rvalue reference), `T` is deduced `Widget`, the type passed in `forward` is `Widget`. Therefore, an rvalue reference `Widget&&` is returned (no reference collapsing applied here). 

Rvalue reference returned from functions is defined to be rvalue. So in this case, `std::forward` will turn `f`'s parameter `fParam` (an lvalue) into an rvalue. 

A universal reference isn't a new kind of reference, it's actually an rvalue reference in a context where two conditions are satisfied:

- Type deduction distinguishes lvalues from rvalues.
- Reference collapsing occurs. 

## 5.5 Assume that move operations are not present, not cheap, and not used

All containers in the standard C++11 library support moving, but it would be a mistake to assume that moving all containers in cheap. 

`std::vector` stores its content on the heap. Objects of such container hold only a pointer to the heap memory storing the contents of the container (simplified). Moving the contents of an entire container is no more than copying the pointer from the source container to the target, and set the source's pointer to null. 

<img src = "\pictures\5-1.png" width = 500>

`std::array` objects lack such a pointer, because the data for a `std::array`'s contents are stored directly in the `std::array` objects:

<img src="\pictures\5-2.png" width = 500>

There are thus several scenarios in which C++11's move semantics do you no good:

- *No move operations*: The object to be moved from fails to offer move operations. The move request therefore becomes a copy request.
- *Move not faster*: The object to be moved from has move operations that are no faster than its copy operations. 
- *Move not usable*: The context in which the moving would take place requires a move operation that emits no exceptions, but that operation isn't declared `noexcept`. 