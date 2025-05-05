# Introduction

`libpdb` provides means for creating Pattern Databases in different permutation
state spaces. In particular, it assumes that either full or partial states are
represented with a vector of integers which are all different. Eligible domains
are the N-Pancake, the sliding-tile puzzle or the TopSpin.

It supports both *MAX* and *ADD* pattern databases.

# Dependencies

Tests have been created using the [Google Test
Framework](https://github.com/google/googletest) which is necessary for both
compiling and runing the tests, see below.designed using

# Install #

To download the code type the following:

``` sh
    $ git clone https://github.com/clinaresl/libpdb.git
```

To compile the source code, create first the `Makefile` with:

``` sh
    $ cmake . -DCMAKE_BUILD_TYPE=Release
    $ make
```

from the `libpdb/` directory created after the clone. It will build the library,
and a generator for every domain defined under `domains/`, and all the unit
tests.

It is also possible to compile separately different parts of the whole bundle.
To compile only the library do:

``` sh
   $ make libpdb
```

which generates a static library.

To generate an executable to solve instances in a specific domain use `make
domain` where *domain* can be any of the following: `npancake` or `npuzzle`.

# Tests #

This library comes with a number of unit tests that have been implemented with
[Google Test](https://github.com/google/googletest). If Google Test is available
in your computer, then you can compile and run the tests with:

``` sh
    $ make gtest
    $ tests/gtest
```

from the `libpdb/` directory created after the clone.

# License #

MIT License

Copyright (c) 2025, Carlos Linares López

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
SOFTWARE.


# Authors #

Carlos Linares Lopez <carlos.linares@uc3m.es>  
Computer Science and Engineering Department <https://www.inf.uc3m.es/en>  
Universidad Carlos III de Madrid <https://www.uc3m.es/home>

