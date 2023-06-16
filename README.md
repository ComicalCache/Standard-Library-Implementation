# Standard Library Implementations
This repository contains a custom standard library implementation which I made to learn and practice or/and out of curiosity and fun. <br>
Documentation for the functions/methods is provided via doxygen descriptions from inside the IDE/text editor or when build from the project root.

#### Project Information
- Project namespace: ext
- Automated testing using Boost.Test

## Vector
Header only implementation of a vector.
```cpp
ext::vector<T> vec;
```

## Dynamic Array
##### TODO: change docs
##### TODO: Add move constructor from other dynamic_array
##### TODO: Add constructor with init value
##### TODO: Add constructor with values from initializer list
##### TODO: Add constructor from iterable/list/dynamic_array with values from iterable
##### TODO: Add constructor from iterable/list/dynamic_array with dimension and sizes
##### TODO: Add all assignment cases
##### TODO: Add slice insert
##### TODO: Add slice get (possible?, typesafe?)
Header only implementation of an n-dimensional dynamic array.
```cpp
ext::dynamic_array<T> arr;
```

## Static array
##### TODO: everything
Header only implementation of an n-dimensional static array.
```cpp
ext::static_array<T, D, N...> arr;
```

## Heap
##### TODO: Add heap moving and copying (figure out how to copy functions)
Header only implementation of a dynamic sized heap (sometimes priority queue).
```cpp
ext::heap<T> heap;
```

## Thanks, Credits and Sources
Thanks to [Martin York](https://github.com/Loki-Astari) for the really good [series on vectors](https://lokiastari.com/posts/Vector-ResourceManagementAllocation). <br>
Thanks to [Abdul Bari](https://www.youtube.com/channel/UCZCFT11CWBi3MHNlGf019nw) for his great video about [heaps](https://www.youtube.com/watch?v=HqPJF2L5h9U).
