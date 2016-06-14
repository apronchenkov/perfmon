Performance counters library
============================

Simple performance counters for C++11.


Usage
-----

You could use the PERFMON_SCOPE for a scope inside of a function:

```cpp
{
    PERFMON_SCOPE("scope_name");
    ... // scope body
}
```

or the PERFMON_STATEMENT for a statement:

```cpp
PERFMON_STATEMENT("counter_name") {
    ... // statement
}
```

WARNING! Most probably you would not like to use the PERFMON macros in
recursive scopes. You could, if you understand what you want; but values of
tick counter would be a bit crazy for a human being.


You could get current statistics in the following way:

```cpp
for (const auto& counter : PERFMON_COUNTERS()) {
    std::cout << counter.Name() << ' '
              << counter.Calls() << ' '
              << counter.Seconds() << std::endl;
}
```

NOTICE! Values of `PERFMON_COUNTERS()` can be imprecise because of
non-atomicity of the integer arithmetic. If you need to get a better
precision, you can call `PERFMON_COUNTERS()` multiple times and use the median.
