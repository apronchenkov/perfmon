Performance monitor library
===========================

A simple performance monitor for C++11.


Usage
-----

You could use the PERFMON_FUNCTION_SCOPE macro for functions:

```cpp
void function()
{
    PERFMON_FUNCTION_SCOPE;
    ... // function body
}
```

or the PERFMON_SCOPE for any scope

```cpp
{
    PERFMON_SCOPE("scope_name");
    ... // scope body
}
```

WARNING! Most probably you would not like to use the PERFMON macros in
recursive scopes. You could, if you understand what you want; but values of
tick counter would be a bit crazy for a human being.

You could get current statistics in the following way:

```cpp
for (const auto& counter : PERFMON_COUNTERS) {
    std::cout << counter.name << ' ' << counter.calls << ' ' << counter.Seconds() << std::endl;
}
```
