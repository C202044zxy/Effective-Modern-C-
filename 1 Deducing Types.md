## 1.1 Template Type Deduction

The General form of a function template goes like this:

```c++
template<typename T>
void f(ParamType param);
f(expr);
```

During the compilation, compilers use `expr` to deduce two types: one for `T` and one for `ParamType`. The type deduced for `T` is dependent not just on the type of `expr`, but also on the form of `ParamType`. There are three cases:

**Case1: ParamType is a Reference or a Pointer**

`T&` can only bind to L-values, and `T` is always deduced to as a non-reference type.  

```c++
template<typename T>
void f(T& param); 

int x = 27;			// x is an int 
const int cx = x;	// cx is a const int 
const int& rx = x;	// rx is a reference to x as a const int

f(x);	// T is int, param's type is int&
f(cx);	// T is const int, param's type is const int&
f(rx);	// T is const int, param's type is const int&
```

In the second and third calls, notice that because `cx` and `rx` designate `const` values, `T` is deduced to be `const int`. Therefore, passing a `const` object to a template taking a `T&` parameter is safe. 

If `param` were a pointer instead of a reference, things would work in the same way. 

**Case2: ParamType is a Universal Reference**

If `T&&` is used in template, it becomes a *universal reference*. It can bind to both L-value and R-value, but things will be different for L-value or R-value:

- If `expr` is an L-value, both `T` and `ParamType` are deduced to be L-value reference. 
- If `expr` is an R-value, the normal (Case 1) rules apply. 

```c++
template<typename T>
void f(T&& param); 	// param is now a universal reference

f(x);	// x is lvalue, so T is int&
		// param's type is int&

f(cx); 	// cx is lvalue, so T is const int&
		// param's type is const int&

f(rx); 	// rx is lvalue, so T is const int&
		// param's type is const int&

f(27); 	// 27 is rvalue, so T is int
		// param's type is therefore int&&
```

**Case 3: ParamType is Neither a Pointer nor a Reference**

We are now dealing with passed by value:

```c++
template<typename T>
void f(T param); 
```

That means that `param` will be a copy of whatever is passed in — a completely new object. This fact inspires us with the rules of type deduction:

- If `expr`'s type is a reference, ignore the reference part.
- If `expr` is a const, ignore that too.

**Things to Remember**

- During template type deduction, arguments that are references are treated as non-references, i.e., their reference-ness is ignored. 
- When deducing types for universal reference parameters, L-value arguments get special treatment. 
- When deducing types for *by-value* parameters, const or volatile arguments are treated as non-const and non-volatile. 
- Arguments that are array or function names decay to pointers, unless they're used to initialize references. 

## 1.2 Auto Type Deduction

Auto type deduction is usually the same as template type deduction, but auto type deduction assumes that a braced initializer represents a `std::initializer_list`, and template type deduction doesn't.

```c++
auto x = { 11, 23, 9 }; 	// std::initializer_list<int>

template<typename T>
void f(T param);
f({ 11, 23, 9 });			// error! Can't deduce type for T
```

This pitfall is one of the reasons some developers put braces around their initializers only when they have to. 

## 1.3 decltype

`decltype` almost always yields the type of a variable or expression without any modifications (preserve their reference or const qualifier). 

C++14 supports `decltype(auto)`, which deduces a type from its initializer, but it performs the type deduction using the `decltype` rules.

```c++
template<typename Container, typename Index>
decltype(auto) get(Container& c, Index i) {
    return c[i];  // Preserves reference
}
std::vector<int> v = { 1, 2, 3 };
get(v, 1) = 5;  // v[1] thus can be modified
```

For L-value expressions of type `T` other than names, `decltype` always reports a type of `T&`. For example, `decltype((x))` is therefore `int&`.

