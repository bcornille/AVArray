# AVArray
Auto-Vectorizable Multidimensional Array Class

### Compiling Performance Tests
```
>$ mkdir build && cd build
>$ cmake ../
>$ make perftest
```
Feedback on vectorization should be given on GNU, Clang, Intel, and Cray compilers.
Specifying compiler can be done by prepending `CC=<compiler>` and `CXX=<compiler>` to the `cmake` command.
Turning on optimizations may also require appending `-DCMAKE_BUILD_TYPE=Release` to the `cmake` command.