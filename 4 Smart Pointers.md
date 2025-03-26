## 4.1 std::unique_ptr

By default, `std::unique_ptr` are the same size as raw pointers. If a raw pointer is small enough and fast enough for you, a `std::unique_ptr` almost certainly is.

`std::unique_ptr` embodies *exclusive ownership* semantics. A non-null `std::unique_ptr` always owns what it points to. Moving a `std::unique_ptr` transfers ownership from source pointer to the destination pointer. 

`std::unique_ptr` is thus a *move-only* type. Upon destruction, a non-null `std::unique_ptr` destroys its resource.

```c++
class Investment {
public:
  Investment() = default;	// explicit constructor
  ~Investment() = default;	// explicit destructor
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
} // output : the object was deleted. 
```

`delFunc` is the custom delete function. All custom delete functions accept a raw pointer to the object to be destroyed, and then destroy that object.

When a custom delete is to be used, its type must be specified as the second type argument to `std::unique_ptr`. 

Converting a `std::unique_ptr` to a `std::shared_ptr` is easy. Therefore, `std::unique_ptr` is well suited as a factory function return type. 

## 4.2 std::shared_ptr

An object accessed via `std::shared_ptr` has its lifetime managed by those pointers through *shared ownership*. When the last `std::shared_ptr` pointing to an object stops pointing there, that `std::shared_ptr` destroys the object it points to. 

A `std::shared_ptr` can tell whether it's the last one pointing to a resource by consulting the resource's *reference count*. The member function `use_count()` returns the reference count.

Move-constructing a `std::shared_ptr` from another `std::shared_ptr` sets the old pointer to null, and the new `std::shared_ptr` starts pointing to the object. In this case, the reference count remains the same. 

`std::shared_ptr` is twice the size of a raw pointer, because they internally contain a raw pointer to the object as well as a raw pointer to the corresponding control block. 

![4-1](\pictures\4-1.png)

An object's control block is set up by the function creating the first `std::shared_ptr` to the object. Multiple control blocks means multiple reference counts, and multiple reference counts means the object will be destroyed multiple times. 

A consequence of these rules is that constructing more than one `std::shared_ptr` from a single raw pointer will lead to undefined behaviour:

```c++
auto pw = new Widget;
std::shared_ptr<Widget> spw1(pw);	// create control block for pw
std::shared_ptr<Widget> spw2(pw);	// create 2nd control block. Bad!
// pw will be deleted twice after the shared_ptr goes out of the scope
```

There are at least two lessons regarding `std::shared_ptr` use here. First, try to avoid passing raw pointers to a `std::shared_ptr` constructor. Second, if you must pass a raw pointer to a `std::shared_ptr` constructor, ass the result of `new` directly instead of going through a raw pointer variable. 

## 4.3 std::weak_ptr

`std::weak_ptr`s are typically created from `std::shared_ptr`s. They point to the same place as the `std::shared_ptr` initializing them, but they don't affect the reference count of the object they point to: 

```c++
auto spw = std::make_shared<Widget>();	// ref count = 1
std::weak_ptr<Widget> wpw(spw);			// ref count remains 1
spw = nullptr;							// ref count goes 0
// the Widget is destroyed. wpw now dangles.
// wpw that dangle are said to have expired (ref count = 0)
if (wpw.expired()) { ... }
```

`std::weak_ptr` lacks dereferencing operation due to safety concerns in multi-threading environment. 

What you need is an **atomic** operation that checks to see if the `std::weak_ptr` has expired and, if not, gives you access to the object it points to. This is done by member function `lock()`, which creates a `std::shared_ptr` from the `std::weak_ptr`.

```c++
std::shared_ptr<Widget> spw1 = wpw.lock();
// if wpw's expired, spw1 is null
```

The another form is the `std::shared_ptr` constructor taking a `std::weak_ptr` as an argument:

```c++
std::shared_ptr<Widget> spw2(wpw);
// if wpw's expired, throw std::bad_weak_ptr
```

A potential use case for `std::weak_ptr` is caching. For a caching factory function, a `std::unique_ptr` return type is not a good fit. Callers should certainly determine the lifetime of those objects, but the cache needs a pointer to the objects too. Therefore, the cached pointers should therefore be `std::weak_ptr`s.

```c++
std::shared_ptr<const Widget> fastLoadWidget(WidgetID id) {
	static std::unordered_map<WidgetID,
		std::weak_ptr<const Widget>> cache;
    if (cache.find(id) == cache.end()) {
        return nullptr;
    }
    auto objPtr = cache[id].lock();
    if (objPtr == nullptr) {
        objPtr = loadWidget(id);
        cache[id] = objPtr;
    }
    return objPtr;
}
```

## 4.4 std::make_unique and std::make_shared

`std::make_unique` and `std::make_shared` are two of the make functions — functions that take an arbitrary set of arguments, perfect-forward them to the constructor for a dynamically allocated object, and return a smart pointer to that object.

```c++
auto upw1(std::make_unique<Widget>());		// with make func
std::unique_ptr<Widget> upw2(new Widget); 	// without make func
```

The version using `new` repeat the type being created, but the make functions don't. Thus the make functions are more efficient. 

The second reason to prefer make functions has to do with exception safety. Suppose that we use `new` in a call to `processWidget`.

```c++
processWidget(std::shared_ptr<Widget>(new Widget),
				computePriority());	// potential resource leak!
```

Compiler may emit code to execute the operations in this order:

1. Perform `new Widget`.
2. Execute `computePriority`.
3. Run `std::shared_ptr` constructor.

If such code is generated and, at runtime, `computePriority` produces an exception, the dynamically allocated `Widget` from step 1 will be leaked. 

Using `std::make_shared` avoids this problem:

```c++
processWidget(std::make_shared<Widget>(), computePriority());
```

