# Standard Library Implementations
This repository contains a custom standard library implementation which I made to learn and practice or/and out of curiosity and fun. <br>
Documentation for the functions/methods is provided via doxygen descriptions from inside the IDE/text editor.

#### Project Information
- Project namespace: ext
- Automated testing using Boost.Test

## Vector
##### TODO: Add construction/assignment with iterable type
##### TODO: Add insertion with iterators (maybe not)
##### TODO: Add insert/push_back initializer list
Header only implementation of a vector.
```cpp
ext::vector<T> vec;
```

## Array
##### TODO: Investigate weird memory leaks
##### TODO: Add construction/assignment with iterable type
##### TODO: Add insertion with iterators (maybe not)
Header only implementation of an array.
```cpp
ext::array<T, N> arr;
```

## Heap
##### TODO: Add heap moving and copying (figure out how to copy functions)
Header only implementation of a dynamic sized heap (sometimes priority queue).
```cpp
ext::heap<T> heap;
```

## Linked list
Header only implementation of a linked list.
```cpp
ext::linked_list<T> list;
```

## Thanks, Credits and Sources
Thanks to [Martin York](https://github.com/Loki-Astari) for the really good [series on vectors](https://lokiastari.com/blog/2016/02/27/vector/val.html). <br>
Thanks to [Abdul Bari](https://www.youtube.com/channel/UCZCFT11CWBi3MHNlGf019nw) for his great video about [heaps](https://www.youtube.com/watch?v=HqPJF2L5h9U).
