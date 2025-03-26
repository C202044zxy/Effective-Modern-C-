## 7.1 Prefer task-based programming to  thread-based

You can create a `std::thread` and run `doAsyncWork` on it, thus employing a *thread-based* approach:

```c++
int doAsyncWork();
std::thread t(doAsyncWork);
```

Or you can pass `doAsyncWork` to `std::async`, a strategy known as *task-based*:

```c++
auto fut = std::async(doAsyncWork);
```

In such work, the function object passed to `std::async` is considered as *task*.

The task-based approach is typically superior to its thread-based counterpart. With the thread-based invocation, there's no straightforward way to get access to the return value. 

With the task-based approach, it's easy, because the future returned from `std::async` offers the `get` function. The `get` function is even more important if `doAsyncWork` emits an exception. 

## 7.2 Specify std::launch::async

Assuming a function f is passed to `std::async` for execution, there are two possible launch policies:

- `std::launch::async` launch policy means that f must be run asynchronously on a different thread. 
- `std::launch::deferred` launch policy means that f may run only when `get` or `wait` is called on the future. When `get` or `wait` is invoked, f will execute synchronously, i.e., the caller will block until f finishes running. 

```c++
auto fut1 = std::async(f);	// defualt launch policy
auto fut2 = std::async(std::launch::async, f);
auto fut3 = std::async(std::launch::deferred, f);
```

Specify `std::launch::async` if asynchronous task execution is essential. It's not possible to predict the behaviour of default launch policy.

## 7.3 Make std::thread unjoinable on all paths

Every `std::thread` object is in one of two states: *joinable* or *unjoinable*. Unjoinable `std::thread` objects include:

- Default-constructed `std::thread` objects.
- `std::thread` objects that have been moved from. 
- `std::thread` objects that have been joined.
- `std::thread` objects that have been detached. 

Both `t.join()` and `t.detach()` can only be invoked on unjoinable thread t. Two functions manage the lifecycle of threads in different ways:

- The `join()` function is used to block the calling thread until the thread t finishes its execution. (The thread t starts execution immediately once it is created.) The thread becomes unjoinable after invoking `join()`.
- The `detach()` function allows the thread t to execute independently from the calling thread. The detached thread will continue executing its task even if the calling thread terminates. The thread becomes unjoinable after invoking `detach()`. 

The Standardization Committee decided that the consequences of destroying a joinable thread were sufficiently dire (may cause an implicit join or implicit detach). Therefore, they essentially banned it by specifying that destruction of a joinable thread causes program termination. 

The following class allows callers to specify whether join or detach should be called when a joinable `ThreadRAII` object is destroyed:

```c++
// RAII stands for "Resource Acquisition Is Initialization"
class ThreadRAII {
public:
    enum class DtorAction { join, detach };
    ThreadRAII(std::thread&& t, DtorAction a)
    : action(a), t(std::move(t)) {}
    ~ThreadRAII() {
        if (t.joinable()) {
            if (action == DtorAction::join) {
                t.join();
            } else {
                t.detach();
            }
        }
    }
    std::thread& get() { return t; }
private:
    DtorAction action;
    std::thread t;
};
```

## 7.4 Be aware of varying thread handle destructor behavior

`std::promise` is used to manage a value or an exception that will be retrieved later by a future. `std::future` thus retrieves a value or an exception from a promise. They comprise the communication channel where a callee transmits a result to a caller.

![](\pictures\6-1.png)

But where is the callee's result stored? The callee could finish before the caller invokes `get` on the future, so the result can't be stored in the caller's `std::promise`.

The result can't be stored in the caller's future, because a `std::future` may be used to create a `std::shared_future`. Note that not all result types can be copied and that the result must live as long as the last future referring to it.

To this end, the shared state is created. It is an internal object that connects the promise and future. It stores the result (value or exception) of the asynchronous computation. Thus, the revised model looks like this:

![6-2](\pictures\6-2.png)

The following code snippet shows how to manually manage promise, future and shared state:

```c++
int compute() {
    std::this_thread::sleep_for(std::chrono::seconds(2));
    return 42;
}
int main() {
    // Create promise.
    std::promise<int> prom;
    // Bind promise to the future
    std::future<int> fut = prom.get_future();
    // Callee
    std::thread t([&prom]() {
        prom.set_value(compute()); // Store result in shared state
    });
    // Get result from shared state
    int result = fut.get(); 
}
```

When you use `std::async`, an implicit `std::promise` is created behind the scene.

The existence of the shared state is important, because the behavior of a future's destructor is determined by the shared state associated with the future:

- The destructor for the **last** future referring to a shared state for a **non-deferred** task launched via **std::async** blocks until the task completes.
- The destructor for all other futures simply destroys the future object.

Well, actually, destructor does one more thing. It decrements the reference count inside the shared state that's manipulated by both the futures referring to it and the callee's `std::promise`. This reference count makes it possible for the library to know when the shared state can be destroyed.

## 7.5 Consider void futures for one-shot event communication

Before diving into the topic, let's get to know the condition variable and `std::condition_variable::wait` behavior. 

```c++
cv.wait(lck)
```

`wait` atomically releases the lock `lck` and blocks the current thread. When another thread calls `notify_one` or `notify_all` on the condition variable, the waiting thread wakes up. Before returning, `wait` relocks the lock `lck`.

But `wait` suffers from **spurious wakeups**. A fact of threading APIs is that code waiting on a condition variable may be awakened even if the condition variable was not notified. 

The C++ condition variable API permits a function object that tests whether the wakeup events have really occurred:

```c++
cv.wait(lck, predicate);
```

Internally, it is similar to:

```c++
while (!predicate()) { cv.wait(lck); }
```

An alternative is to avoid condition variables, mutexes, and flags by having the reacting task wait on a future that is set by the detecting task.

So given:

```c++
std::promise<void> p;
```

the detecting task's code is trivial:

```c++
p.set_value();			// tell the reacting task
```

and the reacting task's code is equally simple:

```c++
p.get_future().wait();	// wait on future
```

The communications channel between a `std::promise` and a future is a one-shot mechanism: it can't be used repeatedly.

## 7.6 Use std::atomic for concurrency, volatile for special memory

**std::atomic**

Once a `std::atomic` object has been constructed, operations on it behave as if they were inside a mutex-protected critical section, but the operations are generally implemented using special machine instructions that are more efficient than would be the case if a mutex were employed. 

```c++
std::atomic<int> ai(0);
ai = 10; 			// atomically set ai to 10
std::cout << ai; 	// atomically read ai's value
++ai;				// atomically increment ai to 11
--ai;				// atomically decrement ai to 10
```

Two aspects of this example are worth noting. First, in the `std::cout << ai;` statement, `std::atomic` only guarantees that the read of ai is atomic. There is no guarantee that the entire statement proceeds atomically.

The second noteworthy aspect of the example is the behavior of the last two statement — the increment and decrement of ai. These are each read-modify-write (RMW) operations, yet they execute atomically. Once `std::atomic` object has been constructed, all member functions on it, including those comprising RMW operations are guaranteed to be atomic.

Copy operations for `std::atomic` are deleted. For example, neither of these two statement will compile when x is `std::atomic`:

```c++
auto y = x;	// error
y = x;		// error
```

In order for the copy construction of $y$ from $x$ to be atomic, compilers would have to generate code to read $x$ and write $y$ in a single atomic operation. Hardware generally can't do that, so copy construction isn't supported for `std::atomic` types.

To initialize $y$ with $x$, the code must be written like this:

```c++
std::atomic<int> y(x.load());
y.store(x.load());
```

Reading $x$ (via `x.load()`) is a separate function call from initializing or storing to $y$. Therefore, there is no reason to expect either statement as a whole to execute as a single atomic operation. 

**volatile**

As a general rule, compilers are permitted to reorder such unrelated assignments. 

Normal memory has the characteristic that if you write a value to a memory location, the value remains there until something overwrites it. For example, 

```c++
auto y = x;	// read x
y = x;		// read x again
```

The compiler thus eliminates the second assignment to $y$. 

Normal memory also has the characteristic that if you write a value to a memory location, never read it, and then write to that memory location again, the first write can be eliminated. 

But it is not the case for special memory. The most common kind of special memory is memory used for *memory-mapped I/O* (external sensors, network ports, etc.). Consider again the code with seemingly redundant reads:

```c++
auto y = x;	// read x
y = x;		// read x again
```

If x is a value reported by a temperature sensor, the second read is not redundant.

`volatile` thus comes to rescue. `volatile` is the way we tell compilers that we're dealing with special memory. Its meaning to compilers is that don't perform any optimizations on operations on this memory. 

Poor `volatile`. It has nothing to do with concurrent programming in C++. But some programmers misunderstood it!

